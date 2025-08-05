#include "plugin.h"
#include <mm/game/go/character.h>
#include "mm/core/input.h"
#include <mm/game/charactermanager.h>

DLLATTATCH;

void PluginAttach(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
    
    // Ejecutar PluginHooks en un hilo aparte con un delay
    std::thread([]() {
        printf("[Thrist&Hunger] Waiting before PluginHooks...\n");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        PluginHooks();
        }).detach();

}

DEFHOOK(void, CPlayer__Decay, (void* thiz, float dt)) {
    static float timer = 0.0f;
    timer += dt;

    if (timer >= 5.0f) {
        timer = 0.0f;

        CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
        if (player) {
            float health = player->GetHealth();
            printf("[Thrist&Hunger] currentHealth (0x180): %.2f\n", health);

            if (health > 1.0f) {
                player->SetHealth(health - 1.0f);
            }
        }
    }
    return CPlayer__Decay_orig(thiz, dt);
}



void PluginHooks() {
    HookMgr::Install(ADDRESS(0x1404C6670, 0x1420DEAC0), CPlayer__Decay_hook, CPlayer__Decay_orig);
}