#include "ScrapInlineHook.h"
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <atomic>
#include <cstring>
#include <cmath>
#include <mm/game/go/go.h>
#include <mm/game/game.h>

// Sólo necesitamos esta variable atómica para guardar el puntero de scraps
std::atomic<void*> g_var_scraps{ nullptr };
std::atomic<void*> g_var_canteen{ nullptr };


// Helpers para generar stub (C++11 friendly)
template<typename T>
inline void emit(std::vector<uint8_t>& buf, T v) {
    uint8_t* p = reinterpret_cast<uint8_t*>(&v);
    buf.insert(buf.end(), p, p + sizeof(T));
}
inline void emit_u8(std::vector<uint8_t>& buf, uint8_t b) {
    buf.push_back(b);
}
inline void emit_bytes(std::vector<uint8_t>& buf, const uint8_t* p, size_t n) {
    buf.insert(buf.end(), p, p + n);
}

// Buscar patrón en memoria
uint8_t* FindPattern(uint8_t* base, size_t size, const char* pattern, const char* mask) {
    size_t plen = std::strlen(mask);
    for (size_t i = 0; i + plen <= size; ++i) {
        bool ok = true;
        for (size_t j = 0; j < plen; ++j) {
            if (mask[j] == 'x' && base[i + j] != static_cast<uint8_t>(pattern[j])) {
                ok = false;
                break;
            }
        }
        if (ok) return base + i;
    }
    return nullptr;
}

// Reservar code cave lo más cerca posible
void* AllocNear(void* target, size_t size) {
    const uintptr_t pageSize = 0x1000;
    uintptr_t start = reinterpret_cast<uintptr_t>(target) & ~((uintptr_t)0x7FFFFFFF);
    start += 0x100000;
    for (int i = 0; i < 0x4000; ++i) {
        void* p = VirtualAlloc(reinterpret_cast<void*>(start), size,
            MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (p) return p;
        start += pageSize;
    }
    // fallback: puede quedar fuera del rango de rel32
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

// Escribir memoria con RWX temporal y flush de cache
bool WriteMem(void* dst, const void* src, size_t sz) {
    DWORD oldProt;
    if (!VirtualProtect(dst, sz, PAGE_EXECUTE_READWRITE, &oldProt)) return false;
    std::memcpy(dst, src, sz);
    DWORD tmp;
    VirtualProtect(dst, sz, oldProt, &tmp);
    FlushInstructionCache(GetCurrentProcess(), dst, sz);
    return true;
}

bool InstallScrapMidHook() {
    HMODULE hMod = GetModuleHandleW(L"MadMax.exe");
    if (!hMod) { std::printf("[ScrapHook] MadMax.exe not loaded yet\n"); return false; }

    uint8_t* dos = reinterpret_cast<uint8_t*>(hMod);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(dos + reinterpret_cast<IMAGE_DOS_HEADER*>(dos)->e_lfanew);
    uint8_t* codeBase = dos + nt->OptionalHeader.BaseOfCode;
    size_t codeSize = nt->OptionalHeader.SizeOfCode;

    const char pat[] = "\xC8\x0F\xB6\x44\xC8\x08\x48\x8D\x14";
    const char mask[] = "xxxxxxxxx";
    uint8_t* hit = FindPattern(codeBase, codeSize, pat, mask);
    if (!hit) { std::printf("[ScrapHook] Pattern not found\n"); return false; }

    uint8_t* inj = hit + 1;
    uint8_t* ret = inj + 5;
    uint8_t* cave = reinterpret_cast<uint8_t*>(AllocNear(inj, 0x200));
    if (!cave) { std::printf("[ScrapHook] Couldn´t reserve code cave\n"); return false; }

    std::vector<uint8_t> stub;
    size_t je1_pos, jne2_pos, jmp_end_pos, store_scraps_pos, endstore_pos, jmp_back_pos;

    // prologue save
    emit_u8(stub, 0x50); emit_u8(stub, 0x51); emit_u8(stub, 0x52);
    emit_bytes(stub, (uint8_t*)"\x41\x50\x41\x51\x41\x52\x41\x53", 8);
    emit_u8(stub, 0x9C);

    // if (rcx == 0x11CD) { [g_var_canteen] = r8; goto done; }
    emit_bytes(stub, (uint8_t*)"\x48\x81\xF9", 3); emit<uint32_t>(stub, 0x11CD); // cmp rcx, 11CD
    size_t jmp_not_canteen = stub.size(); emit_bytes(stub, (uint8_t*)"\x0F\x85", 2); emit<uint32_t>(stub, 0); // jne .check_scraps
    // store canteen
    emit_bytes(stub, (uint8_t*)"\x48\xB8", 2); emit<uint64_t>(stub, (uint64_t)&g_var_canteen); // mov rax, &g_var_canteen
    emit_bytes(stub, (uint8_t*)"\x4C\x89\x00", 3); // mov [rax], r8
    size_t jmp_done_from_canteen = stub.size(); emit_u8(stub, 0xE9); emit<uint32_t>(stub, 0); // jmp .done

    // .check_scraps:
    size_t check_scraps = stub.size();
    emit_bytes(stub, (uint8_t*)"\x48\x81\xF9", 3); emit<uint32_t>(stub, 0x11D0); // cmp rcx, 11D0
    size_t jmp_not_scraps = stub.size(); emit_bytes(stub, (uint8_t*)"\x0F\x85", 2); emit<uint32_t>(stub, 0); // jne .done
    // store scraps
    emit_bytes(stub, (uint8_t*)"\x48\xB8", 2); emit<uint64_t>(stub, (uint64_t)&g_var_scraps); // mov rax, &g_var_scraps
    emit_bytes(stub, (uint8_t*)"\x4C\x89\x00", 3); // mov [rax], r8

    // .done:
    size_t done_label = stub.size();
    // patch forwards
    auto patch_rel = [&](size_t at, size_t target) {
        int64_t src = (int64_t)(cave + at + 6);
        int64_t dst = (int64_t)(cave + target);
        int32_t rel = (int32_t)(dst - src);
        std::memcpy(&stub[at + 2], &rel, sizeof(rel));
        };
    auto patch_jmp = [&](size_t at, size_t target) {
        int64_t src = (int64_t)(cave + at + 5);
        int64_t dst = (int64_t)(cave + target);
        int32_t rel = (int32_t)(dst - src);
        std::memcpy(&stub[at + 1], &rel, sizeof(rel));
        };
    patch_rel(jmp_not_canteen, check_scraps);
    patch_jmp(jmp_done_from_canteen, done_label);
    patch_rel(jmp_not_scraps, done_label);

    // epilogue restore + original byte
    emit_u8(stub, 0x9D);
    emit_bytes(stub, (uint8_t*)"\x41\x5B\x41\x5A\x41\x59\x41\x58", 8);
    emit_u8(stub, 0x5A); emit_u8(stub, 0x59); emit_u8(stub, 0x58);
    emit_bytes(stub, (uint8_t*)"\x0F\xB6\x44\xC8\x08", 5);

    // jump back
    jmp_back_pos = stub.size(); emit_u8(stub, 0xE9); emit<uint32_t>(stub, 0);
    {
        int64_t src = (int64_t)(cave + jmp_back_pos + 5);
        int64_t dst = (int64_t)ret;
        int32_t rel = (int32_t)(dst - src);
        std::memcpy(&stub[jmp_back_pos + 1], &rel, sizeof(rel));
    }

    if (!WriteMem(cave, stub.data(), stub.size())) {
        std::printf("[ScrapHook] Didn´t write in the stub\n");
        return false;
    }

    // parchear el sitio original con JMP rel32 al cave
    uint8_t jmp5[5] = { 0xE9, 0,0,0,0 };
    {
        int64_t src = reinterpret_cast<int64_t>(inj + 5);
        int64_t dst = reinterpret_cast<int64_t>(cave);
        int32_t rel = static_cast<int32_t>(dst - src);
        std::memcpy(&jmp5[1], &rel, sizeof(rel));
    }

    if (!WriteMem(inj, jmp5, sizeof(jmp5))) {
        std::printf("[ScrapHook] Couldn´t write JMP to original place\n");
        return false;
    }

    FlushInstructionCache(GetCurrentProcess(), inj, sizeof(jmp5));
    return true;
}

void ScanScrapCandidatesFloat(float expected_val) {
    void* p = g_var_canteen.load(std::memory_order_acquire);
    if (!p) return;
    

    uint8_t* base = reinterpret_cast<uint8_t*>(p);
    printf("Looking value %.3f in block %p\n", expected_val, p);

    for (int offset = 0; offset <= 0x400 - sizeof(float); ++offset) {
        float val = *reinterpret_cast<float*>(base + offset);
        if (fabs(val - expected_val) < 0.0001f) {
            printf("¡Match float found in offset +0x%02X! (%.3f)\n", offset, val);
        }
    }
}

float GetScraps() {
    uint8_t* base = reinterpret_cast<uint8_t*>(g_var_canteen.load());
    return *reinterpret_cast<float*>(base + 0x24);
}

void SetScraps(float scraps) {
    void* p = g_var_canteen.load(std::memory_order_acquire);
    if (!p) return;
    uint8_t* base = reinterpret_cast<uint8_t*>(p);
    *reinterpret_cast<float*>(base + 0x24) = scraps;
}