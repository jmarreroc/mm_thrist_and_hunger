#pragma once

class FuelManager {
public:
    static FuelManager& Instance();

    void Update(float dt);

private:
    FuelManager() = default;
    FuelManager(const FuelManager&) = delete;
    FuelManager& operator=(const FuelManager&) = delete;

    float lastVehicleHealth = -1.0f;

    //uintptr_t pCarfuel = 0;
    float lastFuel = -1.0f;

    float lastObservedFuel = -1.0f;
    float fuelEpsilon = 0.0001f; // float comparison tolerancy
    bool initialized = false;
    bool initializedFuel = false;

    /*
    void SetFuelPointer(uintptr_t ptr) { pCarfuel = ptr; }
    float GetFuel() const { return pCarfuel ? *(float*)(pCarfuel + 0x0C) : -1.0f; }
    void SetFuel(float value) { if (pCarfuel) *(float*)(pCarfuel + 0x0C) = value; }
    */
};