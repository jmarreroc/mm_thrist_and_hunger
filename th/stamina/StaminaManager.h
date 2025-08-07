#pragma once

class StaminaManager {
public:
    static StaminaManager& Instance();

    void Update(float dt);
    void SetKeyCodes(int forwardKey, int sprintKey);

private:
    StaminaManager() = default;

    StaminaManager(const StaminaManager&) = delete;
    StaminaManager& operator=(const StaminaManager&) = delete;

    float maxStamina = 100.0f;
    float currentStamina = 100.0f;
    int keyMoveForward = -1;
    int keySprint = -1;
};