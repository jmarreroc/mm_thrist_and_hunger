#pragma once

class CanteenManager {
public:
    static CanteenManager& Instance();

    void Update(float dt);

private:
    CanteenManager() = default;
    CanteenManager(const CanteenManager&) = delete;
    CanteenManager& operator=(const CanteenManager&) = delete;

    float timer = 0.0f;
    bool initialized = false;
};