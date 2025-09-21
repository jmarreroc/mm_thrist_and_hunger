#include "ScrapLossManager.h"
#include <mm/game/go/character.h>
#include <mm/game/charactermanager.h>
#include <th/scrap/ScrapInlineHook.h>
#include <mm/game/game.h>

ScrapLossManager& ScrapLossManager::Instance() {
    static ScrapLossManager instance;
    return instance;
}

void ScrapLossManager::Update(float dt) {
    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    if (!player) return;

    float health = player->GetHealth();

    if (health <= 0) {
        diedRun = true;
    }

    if (diedRun) {
        if (CGameState::m_State == CGameState::E_GAME_LOAD) {
            diedRun = false;
            diedLoad = true;
        }
    }
    else if (diedLoad && CGameState::m_State == CGameState::E_GAME_RUN) {
        diedLoad = false;
        SetScraps(0.0);
    }

}
