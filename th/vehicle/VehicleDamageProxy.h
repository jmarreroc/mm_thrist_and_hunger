class VehicleDamageProxy {
public:
    static VehicleDamageProxy& Instance();

    void Update(float dt);

private:
    VehicleDamageProxy() = default;
    VehicleDamageProxy(const VehicleDamageProxy&) = delete;
    VehicleDamageProxy& operator=(const VehicleDamageProxy&) = delete;

    float lastVehicleHealth = -1.0f;
};