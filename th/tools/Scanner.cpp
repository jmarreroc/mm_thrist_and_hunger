#include "Scanner.h"
#include <cstdio>
#include <cmath>
#include <unordered_map>
#include <mm/game/go/character.h> 
#include <Windows.h>
#include <vector>

static std::unordered_map<int, float> prevCharacterValues;

void ScanCharacterFields(CCharacter* player, size_t maxOffset) {
    if (!player) return;

    uint8_t* base = (uint8_t*)player;

    printf("==== [ScanCharacterFields] ====\n");

    for (int offset = 0; offset < static_cast<int>(maxOffset); offset += 4) {
        float val = *(float*)(base + offset);

        if (!std::isfinite(val)) continue;

        if (val >= 0.0f && val <= 10000.0f) {
            float prev = prevCharacterValues[offset];
            if (std::abs(prev - val) > 0.01f) {
                printf("Offset 0x%03X = %.4f (prev: %.4f)\n", offset, val, prev);
                prevCharacterValues[offset] = val;
            }
        }
    }

    printf("==== [End Scan] ====\n");
}

struct MemoryRegion {
    uintptr_t start;
    uintptr_t end;
};

std::vector<MemoryRegion> GetMadMaxRegions() {
    std::vector<MemoryRegion> regions;
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t addr = 0;

    while (VirtualQuery((void*)addr, &mbi, sizeof(mbi))) {
        if ((mbi.State == MEM_COMMIT) &&
            (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE) &&
            mbi.Type == MEM_PRIVATE) {
            regions.push_back({ (uintptr_t)mbi.BaseAddress, (uintptr_t)mbi.BaseAddress + mbi.RegionSize });
        }
        addr += mbi.RegionSize;
    }
    return regions;
}


inline bool SafeReadFloat(uintptr_t addr, float& outVal) {
    __try {
        outVal = *(float*)addr;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void ScanAll() {
    auto regions = GetMadMaxRegions();
    printf("[Scanner] Escaneando %zu regiones...\n", regions.size());

    float val;
    for (auto& r : regions) {
        for (uintptr_t p = r.start; p < r.end; p += sizeof(float)) {
            if (SafeReadFloat(p, val) && val == 100.0f) {
                printf("[Candidate] Addr: 0x%p -> %.2f\n", (void*)p, val);
            }
        }
    }
}

static std::unordered_map<int, float> prevVehicleValues;

void ScanVehicleFields(CVehicle* vehicle, size_t maxOffset) {
    if (!vehicle) return;

    uint8_t* base = (uint8_t*)vehicle;

    printf("==== [ScanVehicleFields] ====\n");

    for (int offset = 0; offset < static_cast<int>(maxOffset); offset += 4) {
        float val = *(float*)(base + offset);

        if (!std::isfinite(val)) continue;

        // Filtrar solo floats entre 50 y 100
        if (val >= 50.0f && val <= 100.0f) {
            float prev = prevVehicleValues[offset];
            if (std::abs(prev - val) > 0.01f) {
                printf("Offset 0x%03X = %.4f (prev: %.4f)\n", offset, val, prev);
                prevVehicleValues[offset] = val;
            }
        }
    }

    printf("==== [End ScanVehicleFields] ====\n");
}