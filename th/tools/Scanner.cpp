#include "Scanner.h"
#include <cstdio>
#include <cmath>
#include <unordered_map>
#include <mm/game/go/character.h>  // Asegúrate que este path es correcto

static std::unordered_map<int, float> prevCharacterValues;

void ScanCharacterFields(CCharacter* player, size_t maxOffset) {
    if (!player) return;

    uint8_t* base = (uint8_t*)player;

    printf("==== [ScanCharacterFields] ====\n");

    for (int offset = 0; offset < static_cast<int>(maxOffset); offset += 4) {
        float val = *(float*)(base + offset);

        if (!std::isfinite(val)) continue;

        if (val > -10000.0f && val < 10000.0f) {
            float prev = prevCharacterValues[offset];
            if (std::abs(prev - val) > 0.01f) {
                printf("Offset 0x%03X = %.4f (prev: %.4f)\n", offset, val, prev);
                prevCharacterValues[offset] = val;
            }
        }
    }

    printf("-- [Possible bool flags] --\n");
    for (int offset = 0; offset < static_cast<int>(maxOffset); ++offset) {
        bool b = *(bool*)(base + offset);
        if (b) {
            printf("Offset 0x%03X = bool: true\n", offset);
        }
    }

    printf("==== [End Scan] ====\n");
}