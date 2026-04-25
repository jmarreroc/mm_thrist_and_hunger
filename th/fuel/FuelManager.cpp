#include "FuelManager.h"
#include <mm/game/go/character.h>
#include <mm/game/go/vehicle.h>
#include "mm/game/game.h"
#include <mm/game/charactermanager.h>
#include "th/fuel/FuelInlineHook.h"
#include "th/Config.h"
#include <stdarg.h>

void FuelLog(const char* fmt, ...) {
    static FILE* f = nullptr;
    if (!f) fopen_s(&f, "fuel_debug.log", "a");
    if (!f) return;
    va_list args; va_start(args, fmt);
    char buf[512]; vsnprintf(buf, sizeof(buf), fmt, args); va_end(args);
    fprintf(f, "%s", buf); fflush(f);
    printf("%s", buf);
}

FuelManager& FuelManager::Instance() { static FuelManager i; return i; }

void FuelManager::Update(float dt) {
    const auto& cfg = Config::instance();
    if (CGameState::m_InMainMenu || CGameState::m_State != CGameState::E_GAME_RUN) {
        lastVehicleHealth = -1.0f; lastObservedFuel = -1.0f; return;
    }

    CCharacter* player = CvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    if (!player) return;
    CVehicle* vehicle = player->GetVehiclePtr();
    if (!vehicle) { lastVehicleHealth = -1.0f; lastObservedFuel = -1.0f; return; }

    // ---- PROBAR DIFERENTES OFFSETS DEL VEHÍCULO ----
    float* fuel0C = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vehicle) + 0x0C);
    float* fuel10 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vehicle) + 0x10);
    float* fuel14 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vehicle) + 0x14);
    float* fuel18 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vehicle) + 0x18);
    float* fuel1C = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vehicle) + 0x1C);
    float* fuel20 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(vehicle) + 0x20);

    // Log CADA 60 frames para ver cuál offset tiele el valor del HUD
    static int frame = 0; frame++;
    if (frame % 60 == 1) {
        FuelLog("[Fuel] v=%p, 0x0C=%.4f, 0x10=%.4f, 0x14=%.4f, 0x18=%.4f, 0x1C=%.4f, 0x20=%.4f\n",
                (void*)vehicle, *fuel0C, *fuel10, *fuel14, *fuel18, *fuel1C, *fuel20);
    }

    // Usar el offset que el usuario confirme (por ahora 0x0C)
    float* fuelPtr = fuel0C;
    float curFuel = *fuelPtr;

    // Daño
    float h = vehicle->GetHealth();
    if (lastVehicleHealth >= 0.0f) {
        float dmg = lastVehicleHealth - h;
        if (dmg > 0.0f && cfg.roguelite().loss_fuel_on_car_damage) {
            float lost = cfg.roguelite().loss_fuel_on_car_damage_factor * dmg;
            float next = curFuel - lost;
            if (next < 0.0f) next = 0.0f;
            *fuelPtr = next; curFuel = next;
            FuelLog("[Fuel] Damage: lost %.4f, now %.4f\n", lost, next);
        }
    }
    lastVehicleHealth = h;

    // Consumo normal + multiplier
    if (lastObservedFuel >= 0.0f) {
        float delta = lastObservedFuel - curFuel;
        if (delta > fuelEpsilon) {
            float mult = (float)cfg.roguelite().fuel_consumption_multiplier;
            FuelLog("[Fuel] Consumo! delta=%.4f, mult=%.2f, fuel %.4f -> ",
                    delta, mult, curFuel);
            if (mult > 1.0f) {
                float extra = delta * (mult - 1.0f);
                float next = curFuel - extra;
                if (next < 0.0f) next = 0.0f;
                *fuelPtr = next; curFuel = next;
                FuelLog("%.4f (extra=%.4f)\n", next, extra);
            }
        }
    }

    lastObservedFuel = curFuel;
}
