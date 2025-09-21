#include "VehicleDamageProxy.h"
#include <mm/game/go/character.h>
#include <mm/game/go/vehicle.h>
#include <mm/game/charactermanager.h>
#include <cstdio>

VehicleDamageProxy& VehicleDamageProxy::Instance() {
    static VehicleDamageProxy instance;
    return instance;
}


void VehicleDamageProxy::Update(float dt) {
    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    CVehicle* vehicle = player->GetVehiclePtr();

    if (!player || !vehicle) return;

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

    // Classification and base multiplier depending on hit severity
    float baseMultiplier = 0.0f;

    if (damage <= 10.0f) {
        lastVehicleHealth = currentVehicleHealth;
        return; // No player damage for small hits
    }
    else if (damage <= 25.0f) {
        baseMultiplier = 6.0f; // Softer multiplier
    }
    else if (damage <= 50.0f) {
        baseMultiplier = 8.0f;
    }
    else if (damage <= 100.0f) {
        baseMultiplier = 10.0f;
    }
    else {
        baseMultiplier = 12.0f;
    }

    // Factor based on player health (more health → more damage taken)
    float playerHealth = player->GetHealth();
    float healthFactor = playerHealth / 2000.0f;

    // Final damage calculation
    float playerDamage = damage * baseMultiplier * healthFactor;
    player->SetHealth(playerHealth - playerDamage);

    // Store the new vehicle health for next frame comparison
    lastVehicleHealth = currentVehicleHealth;
}
