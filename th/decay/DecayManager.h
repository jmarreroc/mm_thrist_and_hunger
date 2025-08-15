#pragma once

class DecayManager {
public:
    static DecayManager& Instance();

    void Update(float dt);

private:
    DecayManager() = default;
    DecayManager(const DecayManager&) = delete;
    DecayManager& operator=(const DecayManager&) = delete;

    float timer = 0.0f;
};