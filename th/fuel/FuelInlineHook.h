#pragma once
#include <windows.h>
#include <cstdint>
#include <atomic>

// Punteros globales capturados por el hook
extern std::atomic<float*> g_var_fuel;
extern std::atomic<uintptr_t> g_var_vehicleBase;

// Instala el mid-hook sobre la instrucción movss [rbx+0x0C],xmm0
bool InstallFuelMidHook();

// Accesores de conveniencia
float GetFuel();
void SetFuel(float fuel);
uintptr_t GetVehicleBase();