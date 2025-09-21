#include <cstdint>

class VehicleDamageProxy {

private:
    VehicleDamageProxy() = default;
    VehicleDamageProxy(const VehicleDamageProxy&) = delete;
    VehicleDamageProxy& operator=(const VehicleDamageProxy&) = delete;

    float lastVehicleHealth = -1.0f;

    uintptr_t pCarfuel = 0;
    float lastFuel = -1.0f;

public:
    static VehicleDamageProxy& Instance();

    void Update(float dt);

    void SetFuelPointer(uintptr_t ptr) { pCarfuel = ptr; }
    float GetFuel() const { return pCarfuel ? *(float*)(pCarfuel + 0x0C) : -1.0f; }
    void SetFuel(float value) { if (pCarfuel) *(float*)(pCarfuel + 0x0C) = value; }

};