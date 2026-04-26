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

}