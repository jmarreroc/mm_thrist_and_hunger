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

        if (val >= 0.0f) {
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

        if (val > 0.0f) {
            float prev = prevVehicleValues[offset];
            if (std::abs(prev - val) > 0.01f) {
                printf("Offset 0x%03X = %.4f (prev: %.4f)\n", offset, val, prev);
                prevVehicleValues[offset] = val;
            }
        }
    }

    printf("==== [End ScanVehicleFields] ====\n");
}


struct FuelCandidate {
    int offset;
    float lastValue;
    int stabilityScore = 0;
};

std::vector<FuelCandidate> fuelCandidates;
int confirmedFuelOffset = -1;


void ScanVehicleForFuel(CCharacter* vehicle) {
    if (!vehicle) return;

    uint8_t* base = reinterpret_cast<uint8_t*>(vehicle);
    static int scanStep = 0;
    const int stepSize = 0x1200;

    int startOffset = scanStep * stepSize;
    int endOffset = startOffset + stepSize;
    scanStep++;

    if (startOffset > 0x2400) scanStep = 0;

    printf("[Scanner] Explorando offsets %X - %X\n", startOffset, endOffset);

    // Explorar nuevos candidatos
    for (int offset = startOffset; offset < endOffset; offset += 4) {
        float val = *(float*)(base + offset);
        if (val >= 40.0f && val <= 50.0f) {
            bool exists = false;
            for (auto& c : fuelCandidates) {
                if (c.offset == offset) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                fuelCandidates.push_back({ offset, val, 1 });
                printf("  → Nuevo candidato +0x%X = %.2f\n", offset, val);
            }
        }
    }

    // Reevaluar todos los candidatos
    for (auto& candidate : fuelCandidates) {
        float val = *(float*)(base + candidate.offset);
        if (val >= 0.0f && val <= 100.0f) {
            if (fabs(val - candidate.lastValue) > 0.01f) {
                candidate.stabilityScore++;
                candidate.lastValue = val;
                printf("  → Candidato +0x%X cambió a %.2f (score: %d)\n", candidate.offset, val, candidate.stabilityScore);
            }
        }
    }

    // Confirmar el mejor candidato (con mayor score)
    int bestScore = 0;
    int bestOffset = -1;
    for (auto& candidate : fuelCandidates) {
        if (candidate.stabilityScore > bestScore) {
            bestScore = candidate.stabilityScore;
            bestOffset = candidate.offset;
        }
    }

    if (bestScore >= 3) {
        confirmedFuelOffset = bestOffset;
        printf("[✔] Offset confirmado: +0x%X\n", confirmedFuelOffset);
    }

    // Mostrar valor actual si ya hay offset confirmado
    if (confirmedFuelOffset >= 0) {
        float fuel = *(float*)(base + confirmedFuelOffset);
        printf("[Fuel] Confirmed offset +0x%X → %.2f\n", confirmedFuelOffset, fuel);
    }
}



void ScanVehicleForFuel2(CVehicle* vehicle) {
    if (!vehicle) {
        return;
    }

    uint8_t* base = reinterpret_cast<uint8_t*>(vehicle);
    printf("Escaneando vehículo en %p buscando valores tipo gasolina (0.0 - 50.0)\n", vehicle);

    for (int offset = 0; offset <= 0x2400; offset += 4) {
        float val = *reinterpret_cast<float*>(base + offset);

        // Filtrar valores dentro del rango esperado
        if (val >= 40.0f && val <= 50.0f) {
            printf("Posible gasolina en offset +0x%X -> %.2f\n", offset, val);
        }
    }
}
