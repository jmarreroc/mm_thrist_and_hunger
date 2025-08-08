#include <Windows.h>
#define VK_W 0x57 // W
#define VK_S 0x53 // S
#define VK_A 0x41 // A
#define VK_D 0x44 // D

#include <cstdio>
#include "th/tools/InputInterceptor.h"
#include "StaminaManager.h"

StaminaManager::StaminaManager() {}

StaminaManager& StaminaManager::Instance() {
    static StaminaManager instance;
    return instance;
}

float StaminaManager::GetStamina() const {
    return currentStamina;
}

float StaminaManager::GetMaxStamina() const {
    return maxStamina;
}

void StaminaManager::Update(float dt) {
    bool shiftPressed =
        (GetAsyncKeyState(VK_LSHIFT) & 0x8000);

    bool moving =
        (GetAsyncKeyState(VK_W) & 0x8000) ||
        (GetAsyncKeyState(VK_S) & 0x8000) ||
        (GetAsyncKeyState(VK_A) & 0x8000) ||
        (GetAsyncKeyState(VK_D) & 0x8000);

    if (shiftPressed && moving) {
        currentStamina -= 15.0f * dt; 
        if (currentStamina < 0.0f) currentStamina = 0.0f;
        printf("[StaminaManager] Stamina: %.2f / %.2f\n", currentStamina, maxStamina);

        if (currentStamina <= 0.0f) {
            printf("[StaminaManager] Blocking Sprint!!");

            currentStamina = 0.0f;
            InputInterceptor::Instance().SetBlockSprint(true);
        }
        else {
            InputInterceptor::Instance().SetBlockSprint(false);
        }

    }
    else {
        currentStamina += 30.0f * dt;
        if (currentStamina > maxStamina) currentStamina = maxStamina;
    }

}