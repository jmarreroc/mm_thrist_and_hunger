#include "DecayManager.h"
#include <mm/game/go/character.h>
#include <mm/game/charactermanager.h>
#include <cstdio>

DecayManager& DecayManager::Instance() {
    static DecayManager instance;
    return instance;
}

void DecayManager::Update(float dt) {
    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    if (!player) return;

    float health = player->GetHealth();
    float healthPercent = health / 2000.0f;

    // Intervalo dinámico
    float baseInterval = 30.0f;
    float minInterval = 10.0f;
    float interval = baseInterval - (baseInterval - minInterval) * healthPercent;

    float maxDamage = 10.0f;
    float minDamage = 1.0f;
    float damage = minDamage + (maxDamage - minDamage) * healthPercent;

    timer += dt;
    if (timer >= interval) {
        timer = 0.0f;
        if (health > 1.0f) {
            player->SetHealth(health - damage);
        }
    }
}