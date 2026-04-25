#include "FuelManager.h"
#include <mm/game/go/character.h>
#include <mm/game/go/vehicle.h>
#include "mm/game/game.h"
#include <mm/game/charactermanager.h>
#include "th/fuel/FuelInlineHook.h"
#include "th/Config.h"

FuelManager& FuelManager::Instance() {
    static FuelManager instance;
    return instance;
}

void FuelManager::Update(float dt) {

    const auto& cfg = Config::instance();


    if (CGameState::m_InMainMenu || CGameState::m_State != CGameState::E_GAME_RUN) {
        initialized = false;
        initializedFuel = false;
        return;
    }

    if (CGameState::m_State == CGameState::E_GAME_RUN && !initialized) {
        initialized = true;
    }


    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    CVehicle* vehicle = player->GetVehiclePtr();

    auto fuelPtr = g_var_fuel.load(std::memory_order_relaxed);

    if (!player || !vehicle) return;


    if (fuelPtr && !initializedFuel) {
        float cur = *fuelPtr;
        float factor = cfg.roguelite().fuel_modifier_on_loading;
        float reduced = cur * (1.0f - factor);
        float amountReduced = cur - reduced;
        *fuelPtr = reduced;
        lastObservedFuel = reduced;
        initializedFuel = true;
    }

    float currentVehicleHealth = vehicle->GetHealth();
    // First frame, just initialize lastVehicleHealth
    if (lastVehicleHealth < 0.0f) {
        lastVehicleHealth = currentVehicleHealth;
        return;
    }

    float damage = lastVehicleHealth - currentVehicleHealth;
    if (damage <= 0.0f) {
        lastVehicleHealth = currentVehicleHealth;
        return;
    }

    if (damage > 0.0f) {
        if (fuelPtr && cfg.roguelite().loss_fuel_on_car_damage) {
            float fuelLoss = cfg.roguelite().loss_fuel_on_car_damage_factor * damage;
            float curFuel = *fuelPtr;
            float nextFuel = curFuel - fuelLoss;
            if (nextFuel <= 0.0f) {
                nextFuel = 0.0f;
            }
            *fuelPtr = nextFuel;
            lastObservedFuel = nextFuel;
        }
    }

    lastVehicleHealth = currentVehicleHealth;

    // Si el tanque está roto, vaciar según leakRate y detener al llegar a 0
    /*
    if (tankRuptured.load(std::memory_order_relaxed) && fuelPtr) {
        float leak = tankLeakRate * dt;
        float cur = *fuelPtr;
        float next = cur - leak;
        if (next <= 0.0f) {
            next = 0.0f;
            tankRuptured.store(false, std::memory_order_relaxed); // detener drenaje
            printf("[Fuel] Tank emptied due rupture, stopping leak\n");
        }
        *fuelPtr = next;
    }
    */

    /*
    if (fuelPtr) {
        float cur = *fuelPtr;
        if (lastObservedFuel < -0.5f) {
            // primera observación
            lastObservedFuel = cur;
        }
        else {
            // si fuel ha decrecido desde el último frame
            if (cur + fuelEpsilon < lastObservedFuel) {
                float delta = lastObservedFuel - cur; // cuanto ha consumido el juego este frame
                float extra = delta;                  // duplicar => aplicar la misma cantidad
                float next = cur - extra;
                if (next < 0.0f) next = 0.0f;
                *fuelPtr = next;
                // opcional: log cuando aplicas duplicación
                //printf("[Fuel] Duplicated consumption: delta=%.4f -> fuel now %.4f (veh=%p)\n", delta, next, (void*)vehicle);
                lastObservedFuel = next;
            }
            else {
                // no consumo este frame, solo actualizar la observación
                lastObservedFuel = cur;
            }
        }
    }
    else {
        // sin puntero válido, reset de observación
        lastObservedFuel = -1.0f;
    }
    */

}