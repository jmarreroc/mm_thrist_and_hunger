#include "MidHookHelper.h"
#include <windows.h>
#include <vector>


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
