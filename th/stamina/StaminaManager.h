#pragma once

class StaminaManager {
public:
    static StaminaManager& Instance();

    void Update(float dt);
    float GetStamina() const;
    float GetMaxStamina() const;

private:
    StaminaManager();

    float maxStamina = 100.0f;
    float currentStamina = 100.0f;
};