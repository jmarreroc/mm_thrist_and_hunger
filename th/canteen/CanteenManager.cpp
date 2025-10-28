#include "CanteenManager.h"
#include <mm/game/go/character.h>
#include <mm/game/charactermanager.h>
#include "mm/game/game.h"
#include "th/scrap/ScrapInlineHook.h"
#include <cstdio>

CanteenManager& CanteenManager::Instance() {
    static CanteenManager instance;
    return instance;
}

void CanteenManager::Update(float dt) {
    if (CGameState::m_InMainMenu || CGameState::m_State != CGameState::E_GAME_RUN) {
        initialized = false;
    }
    
    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    if (!player) return;

    if (CGameState::m_State == CGameState::E_GAME_RUN && !initialized) {
        float canteen = GetCanteen();
        SetCanteen(0.0f);
        initialized = true;
    }

}