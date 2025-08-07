#include "DecayManager.h"
#include <mm/game/go/character.h>
#include <mm/game/charactermanager.h>
#include <cstdio>

void DecayManager::Update(float dt) {

    static float timer = 0.0f;
    timer += dt;

    if (timer >= 5.0f) {
        timer = 0.0f;

        CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
        if (player) {
            float health = player->GetHealth();
            printf("[DecayManager] currentHealth: %.2f\n", health);

            if (health > 1.0f) {
                player->SetHealth(health - 1.0f);
            }
        }
    }
}