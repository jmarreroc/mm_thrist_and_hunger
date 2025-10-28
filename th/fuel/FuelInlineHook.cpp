#include "FuelInlineHook.h"
#include "th/tools/MidHookHelper.h"
#include <cstdio>
#include <vector>
#include <cstring>

// Definiciones de los globals
std::atomic<float*> g_var_fuel{ nullptr };
std::atomic<uintptr_t> g_var_vehicleBase{ 0 };

// Emit helpers
inline void emit_u8(std::vector<uint8_t>& buf, uint8_t v) { buf.push_back(v); }
template<typename T>
inline void emit(std::vector<uint8_t>& buf, T v) {
    size_t pos = buf.size();
    buf.resize(pos + sizeof(T));
    std::memcpy(buf.data() + pos, &v, sizeof(T));
}
inline void emit_bytes(std::vector<uint8_t>& buf, const uint8_t* data, size_t len) {
    buf.insert(buf.end(), data, data + len);
}

bool InstallFuelMidHook() {
    HMODULE hMod = GetModuleHandleW(L"MadMax.exe");
    if (!hMod) {
        return false;
    }

    uint8_t* dos = reinterpret_cast<uint8_t*>(hMod);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(dos + reinterpret_cast<IMAGE_DOS_HEADER*>(dos)->e_lfanew);
    uint8_t* codeBase = dos + nt->OptionalHeader.BaseOfCode;
    size_t codeSize = nt->OptionalHeader.SizeOfCode;

    // Patrón: F3 0F 11 43 0C 76 (movss [rbx+0x0C],xmm0 ; j? ...)
    const char pat[] = "\xF3\x0F\x11\x43\x0C\x76";
    const char mask[] = "xxxxxx";

    uint8_t* hit = FindPattern(codeBase, codeSize, pat, mask);
    if (!hit) {
        return false;
    }

    // INYECTA EN EL INICIO DE LA INSTRUCCIÓN (NO +1)
    uint8_t* inj = hit;        // inicio del movss
    uint8_t* ret = hit + 5;    // después de la instrucción original

    uint8_t* cave = reinterpret_cast<uint8_t*>(AllocNear(inj, 0x200));
    if (!cave) {
        return false;
    }

    std::vector<uint8_t> stub;

    // Prologue save: preserva rbx también (no lo tocamos, pero seguridad)
    emit_u8(stub, 0x50); // push rax
    emit_u8(stub, 0x51); // push rcx
    emit_u8(stub, 0x52); // push rdx
    emit_u8(stub, 0x53); // push rbx
    emit_bytes(stub, (uint8_t*)"\x41\x50\x41\x51\x41\x52\x41\x53", 8); // push r8,r9,r10,r11
    emit_u8(stub, 0x9C); // pushfq

    // Guardar vehicle base (rbx) en g_var_vehicleBase
    // mov rax, &g_var_vehicleBase ; mov [rax], rbx
    emit_bytes(stub, (uint8_t*)"\x48\xB8", 2); emit<uint64_t>(stub, (uint64_t)&g_var_vehicleBase);
    emit_bytes(stub, (uint8_t*)"\x48\x89\x18", 3);

    // Guardar fuel ptr (rbx+0x0C) en g_var_fuel SIN MODIFICAR rbx:
    // mov rax, &g_var_fuel
    // lea rdx, [rbx+0x0C]
    // mov [rax], rdx
    emit_bytes(stub, (uint8_t*)"\x48\xB8", 2); emit<uint64_t>(stub, (uint64_t)&g_var_fuel);
    emit_bytes(stub, (uint8_t*)"\x48\x8D\x53\x0C", 4); // lea rdx, [rbx+0x0C]
    emit_bytes(stub, (uint8_t*)"\x48\x89\x10", 3);     // mov [rax], rdx

    // Re-emitir instrucción original EXACTA: movss [rbx+0x0C], xmm0
    emit_bytes(stub, (uint8_t*)"\xF3\x0F\x11\x43\x0C", 5);

    // Epilogue restore
    emit_u8(stub, 0x9D); // popfq
    emit_bytes(stub, (uint8_t*)"\x41\x5B\x41\x5A\x41\x59\x41\x58", 8); // pop r11,r10,r9,r8
    emit_u8(stub, 0x5B); // pop rbx
    emit_u8(stub, 0x5A); // pop rdx
    emit_u8(stub, 0x59); // pop rcx
    emit_u8(stub, 0x58); // pop rax

    // Jump back
    size_t jmp_back_pos = stub.size();
    emit_u8(stub, 0xE9); emit<uint32_t>(stub, 0);
    {
        int64_t src = (int64_t)(cave + jmp_back_pos + 5);
        int64_t dst = (int64_t)ret;
        int32_t rel = (int32_t)(dst - src);
        std::memcpy(&stub[jmp_back_pos + 1], &rel, sizeof(rel));
    }

    if (!WriteMem(cave, stub.data(), stub.size())) {
        printf("[FuelHook] WriteMem to cave failed\n");
        return false;
    }

    // Parchear sitio original con JMP rel32 al cave, reemplazando los 5 bytes del movss
    uint8_t jmp5[5] = { 0xE9, 0,0,0,0 };
    {
        int64_t src = reinterpret_cast<int64_t>(inj + 5);
        int64_t dst = reinterpret_cast<int64_t>(cave);
        int32_t rel = static_cast<int32_t>(dst - src);
        std::memcpy(&jmp5[1], &rel, sizeof(rel));
    }

    if (!WriteMem(inj, jmp5, sizeof(jmp5))) {
        printf("[FuelHook] WriteMem to inj failed\n");
        return false;
    }

    FlushInstructionCache(GetCurrentProcess(), inj, sizeof(jmp5));
    printf("[FuelHook] Installed successfully at %p -> cave %p\n", inj, cave);
    return true;
}

// Accesores
float GetFuel() {
    auto ptr = g_var_fuel.load(std::memory_order_relaxed);
    return ptr ? *ptr : 0.0f;
}

void SetFuel(float fuel) {
    auto ptr = g_var_fuel.load(std::memory_order_relaxed);
    if (ptr) *ptr = fuel;
}

uintptr_t GetVehicleBase() {
    return g_var_vehicleBase.load(std::memory_order_relaxed);
}