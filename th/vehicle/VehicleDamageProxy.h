#include <cstdint>
#include <atomic>
#include <random>

class VehicleDamageProxy {

private:
    VehicleDamageProxy() = default;
    VehicleDamageProxy(const VehicleDamageProxy&) = delete;
    VehicleDamageProxy& operator=(const VehicleDamageProxy&) = delete;

    float lastVehicleHealth = -1.0f;

public:
    static VehicleDamageProxy& Instance();

    void Update(float dt);

};