﻿#include "plugin.h"
#include "th/decay/DecayManager.h"
#include "th/sanity/SanityManager.h"
#include "th/vehicle/VehicleDamageProxy.h"
#include "th/roguelite/ScrapLossManager.h"
#include "th/scrap/ScrapInlineHook.h"
#include "mm/imgui/imgui.h"


DLLATTATCH;

void PluginAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
    
    std::thread([]() {
        printf("[Thrist&Hunger] Waiting before PluginHooks...\n");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        PluginHooks();
        }).detach();
}

DEFHOOK(void, Thirst__Hunger, (void* thiz, float dt)) {
    DecayManager::Instance().Update(dt);
    VehicleDamageProxy::Instance().Update(dt);
    ScrapLossManager::Instance().Update(dt);
    SanityManager::Instance().Update(dt);
    return Thirst__Hunger_orig(thiz, dt);
}

void PluginHooks() {
    ImGuiRenderer::Install();
    SDLPlayer::Instance().init();
    HookMgr::Install(ADDRESS(0x1404C6670, 0x1420DEAC0), Thirst__Hunger_hook, Thirst__Hunger_orig);

    if (!InstallScrapMidHook()) {
        printf("[Plugin] ScrapHook::Installation Failed\n");
    }
    else {
        printf("[Plugin] ScrapHook::Installed\n");
    }

}
