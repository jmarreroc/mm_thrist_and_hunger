#include "plugin.h"
#include "mm/core/input.h"
#include "th/decay/DecayManager.h"
#include "th/stamina/StaminaManager.h"
#include "th/vehicle/VehicleDamageProxy.h"

DLLATTATCH;

void PluginAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    
    // Ejecutar PluginHooks en un hilo aparte con un delay
    std::thread([]() {
        printf("[Thrist&Hunger] Waiting before PluginHooks...\n");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        PluginHooks();
        }).detach();

}


DEFHOOK(void, Thirst__Hunger, (void* thiz, float dt)) {
    DecayManager::Instance().Update(dt);
    VehicleDamageProxy::Instance().Update(dt);
    //StaminaManager::Instance().Update(dt); pure sh*t....
    return Thirst__Hunger_orig(thiz, dt);
}


void PluginHooks() {
    HookMgr::Install(ADDRESS(0x1404C6670, 0x1420DEAC0), Thirst__Hunger_hook, Thirst__Hunger_orig);
}
