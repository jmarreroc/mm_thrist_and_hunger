#include "plugin.h"
#include "th/Config.h"
#include "th/decay/DecayManager.h"
#include "th/sanity/SanityManager.h"
#include "th/canteen/CanteenManager.h"
#include "th/vehicle/VehicleDamageProxy.h"
#include "th/roguelite/ScrapLossManager.h"
#include "th/scrap/ScrapInlineHook.h"
#include "th/fuel/FuelInlineHook.h"
#include "th/fuel/FuelManager.h"
#include "mm/imgui/imgui.h"

DLLATTATCH;

void PluginAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    Config::instance().load("th/config.ini");
    SyncFuelMultiplier();
    const auto& cfg = Config::instance();
    
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    
    std::thread([]() {
        printf("[Thrist&Hunger] Waiting before PluginHooks...\n");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        PluginHooks();
        }).detach();


    printf("=========================================\n");
    printf("[Thirst&Hunger] 1.3.5 Configuration Status\n");
    printf("=========================================\n");

    if (cfg.decay().enabled)
        printf("[Thirst&Hunger] Decay Enabled...\n");
    else
        printf("[Thirst&Hunger] Decay Disabled...\n");

    printf("  Base Interval: %.2f\n", cfg.decay().base_interval);
    printf("  Min Interval:  %.2f\n", cfg.decay().min_interval);
    printf("  Damage Range:  %.2f - %.2f\n",
        cfg.decay().min_damage, cfg.decay().max_damage);
    printf("\n");

    if (cfg.sanity().enabled)
        printf("[Thirst&Hunger] Sanity Enabled...\n");
    else
        printf("[Thirst&Hunger] Sanity Disabled...\n");
    
    if (cfg.sanity().show_hud)
        printf("  Sanity show HUD Enabled...\n");
    else
        printf("  Sanity show HUD Disabled...\n");

    printf("  Flash Fire Auto Trigger: %.2f\n", cfg.sanity().sanity_percentage_flashes_fire_automatically);
    printf("  Flash Fire HP Threshold: %.2f\n", cfg.sanity().flash_fire_health_percentage_threshold);
    printf("  Director Timer: %.2f | Booster: %.2f\n", cfg.sanity().director_timer, cfg.sanity().director_booster);
    printf("  Volume Modifier: %.2f\n", cfg.sanity().volume_modifier);
    printf("\n");

    printf("[Thirst&Hunger] Roguelite Settings:\n");
    printf("  loss_scrap_on_death:      %s\n", cfg.roguelite().loss_scrap_on_death ? "Enabled" : "Disabled");
    printf("  empty_canteen_on_loading: %s\n", cfg.roguelite().empty_canteen_on_loading ? "Enabled" : "Disabled");
    printf("  loss_fuel_on_car_damage:  %s\n", cfg.roguelite().loss_fuel_on_car_damage ? "Enabled" : "Disabled");
    printf("  fuel_modifier_on_loading:  %.2f\n", cfg.roguelite().fuel_modifier_on_loading);
    printf("  loss_fuel_on_car_damage_factor:  %.2f\n", cfg.roguelite().loss_fuel_on_car_damage_factor);
    printf("  fuel_consumption_multiplier:  %.2f\n", cfg.roguelite().fuel_consumption_multiplier);

    printf("=========================================\n");

}

DEFHOOK(void, Thirst__Hunger, (void* thiz, float dt)) {
    const auto& cfg = Config::instance();

    // Ejecutar el juego PRIMERO (para que consuma gasolina)
    Thirst__Hunger_orig(thiz, dt);

    // Ahora sí, leer el fuel después del consumo del juego
    if (cfg.decay().enabled)
        DecayManager::Instance().Update(dt);

    if (cfg.roguelite().loss_scrap_on_death)
        ScrapLossManager::Instance().Update(dt);

    if (cfg.sanity().enabled)
        SanityManager::Instance().Update(dt);

    if (cfg.roguelite().empty_canteen_on_loading)
        CanteenManager::Instance().Update(dt);

    FuelManager::Instance().Update(dt);
    VehicleDamageProxy::Instance().Update(dt);
}


void PluginHooks() {

    static bool installed = false;
    if (installed) return;
    installed = true;

    ImGuiRenderer::Install();
    SDLPlayer2::Instance().init();
    HookMgr::Install(ADDRESS(0x1404C6670, 0x1420DEAC0), Thirst__Hunger_hook, Thirst__Hunger_orig);

    if (!InstallScrapMidHook()) {
        printf("[Plugin] ScrapHook::Installation Failed\n");
    }
    else {
        printf("[Plugin] ScrapHook::Installed\n");
    }

    // Hook de Fuel
    if (!InstallFuelMidHook()) {
        printf("[Plugin] FuelHook::Installation Failed\n");
    }
    else {
        printf("[Plugin] FuelHook::Installed\n");
    }
}
