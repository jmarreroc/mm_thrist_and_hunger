#include "DecayManager.h"
#include <mm/game/go/character.h>
#include <mm/game/charactermanager.h>
#include "mm/game/game.h"
#include "th/scrap/ScrapInlineHook.h"
#include "th/fuel/FuelInlineHook.h"
#include <cstdio>

DecayManager& DecayManager::Instance() {
    static DecayManager instance;
    return instance;
}

void DecayManager::Update(float dt) {
    if (CGameState::m_InMainMenu || CGameState::m_State != CGameState::E_GAME_RUN) {
        initialized = false;
    }
    
    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    if (!player) return;

    
    float health = player->GetHealth();
    float healthPercent = health / 2000.0f;

    // Dynamic interval
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