#include "StaminaManager.h"
#include <cstdio>
#include <Windows.h>


// Singleton instance
StaminaManager& StaminaManager::Instance() {
    static StaminaManager instance;
    return instance;
}

void StaminaManager::Update(float dt) {
    bool isMovingForward = (GetAsyncKeyState(keyMoveForward) & 0x8000) != 0;
    bool isSprinting = (GetAsyncKeyState(keySprint) & 0x8000) != 0;

    if (isMovingForward && isSprinting) {
        currentStamina -= 20.0f * dt;
        if (currentStamina < 0) currentStamina = 0;
    }
    else {
        currentStamina += 10.0f * dt;
        if (currentStamina > maxStamina) currentStamina = maxStamina;
    }

    // Debug:
    printf("Stamina: %.2f\n", currentStamina);
}

void StaminaManager::SetKeyCodes(int forwardKey, int sprintKey) {
    keyMoveForward = forwardKey;
    keySprint = sprintKey;
}
