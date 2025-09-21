#include "SanityManager.h"
#include "th/sanity/SanityHUD.cpp"
#include "th/scrap/ScrapInlineHook.h"

SanityManager& SanityManager::Instance() {
    static SanityManager instance;
    return instance;
}

int GetGriffaValue() {
    uintptr_t* globalPtr = (uintptr_t*)(0x140000000 + 0x17FE7E0);
    if (!globalPtr || !*globalPtr) {
        return -1;
    }

    uintptr_t rax = *globalPtr;
    int* griffaValue = (int*)(rax + 0x128);

    if (griffaValue) {
        return *griffaValue;
    }

    return -1;
}


void ModifyGriffaValue(int newValue) {
    uintptr_t* globalPtr = (uintptr_t*)(0x140000000 + 0x17FE7E0);
    if (!globalPtr || !*globalPtr) {
        return;
    }

    uintptr_t rax = *globalPtr;
    int* griffaValue = (int*)(rax + 0x128);

    if (griffaValue) {
        *griffaValue = newValue;
    }
}

void SanityManager::Update(float dt) {

    if (diedRun) {
        sanityHud.SetForceHide(true);
        if (CGameState::m_State == CGameState::E_GAME_LOAD) {
            diedRun = false;
            diedLoad = true;
        }
    }
    else if (diedLoad && CGameState::m_State == CGameState::E_GAME_RUN) {
        diedLoad = false;
        ModifyGriffaValue(0);
        sanityHud.sanity = sanityHud.MAX_SANITY / 2;
    }

    if (CGameState::m_InMainMenu || CGameState::m_State != CGameState::E_GAME_RUN || IsGuiOccludingMainDraw()) {
        sanityHud.SetForceHide(true);
        return;
    }

    CCharacter* player = CAvaSingle<CCharacterManager>::Instance->GetPlayerCharacter();
    if (!player) {
        sanityHud.SetForceHide(true);
        return;
    }

    sanityHud.Touch(); // Touch for rendering

    CVehicle* vehicle = player->GetVehiclePtr();

    float health = player->GetHealth();
    float healthPercent = health / 2000.0f;
    float sanityPercent = sanityHud.sanityPercent();


    // First frame, just initialize lastPlayerHealth
    if (lastPlayerHealth < 0.0f) {
        lastPlayerHealth = health;
        return;
    }

    // First frame, just initialize lastScrap
    float scraps = GetScraps();
    if (lastScrap < 0.0f) {
        lastScrap = health;
        return;
    }
    float scrapsGain = scraps - lastScrap;
    lastScrap = scraps;
    if (scrapsGain > 0) {
        if (scrapsGain > 100.0f) IncSanity(100.0f);
        else IncSanity(scrapsGain);
    }

    // If player suffers damage or heal, the sanity suffers proportionally
    float damage = lastPlayerHealth - health;    
    if (damage > 0.0f) {
        float weight = 0.3f + 0.7f * sanityPercent;
        IncSanity(-(damage * weight));
    } else if (damage < 0.0f) { // healing
        float weight = 0.6f - 0.5f * sanityPercent;
        IncSanity(-(damage * weight));
    }
    lastPlayerHealth = health;

    float baseRegenInterval = 10.0f;   // sanity = 100%
    float minRegenInterval = 1.0f;    // sanity = 0%
    float regenIntervalSanity = baseRegenInterval - (baseRegenInterval - minRegenInterval) * (1.0f - sanityPercent);
    float regenInterval = regenIntervalSanity * (1.0f - 0.5f * healthPercent);
    float inc = 10.f;

    timer += dt;
    if (vehicle) {
        if (timer >= regenInterval) {
            timer = 0.0f;
            if (sanityHud.sanity < sanityHud.MAX_SANITY) IncSanity(inc);
        }
    }
 
    if (sanityHud.sanity <= 0) {
        player->SetHealth(0.0f);
    }

    if (health <= 0) {
        diedRun = true;
    }

    if (!vehicle) {
        if (damage > 10.0f) {
            float baseProbability = (damage / 100);
            float probability = baseProbability + directorProbabilityBoost;
            if (probability > 1.0f) probability = 1.0f;

            if (dist(g) < probability) {
                triggerFlashback(sanityHud.sanity);
                flashbackProbabilityBoost = 0.0f;
            }
            else {
                flashbackProbabilityBoost += 0.1f;
            }
        }
    }

    director += dt;
    if (director >= 30.0f) {        
        director = 0.0f;
        float baseProbability = (sanityHud.MAX_SANITY - sanityHud.sanity) / sanityHud.MAX_SANITY;
        float probability = baseProbability + directorProbabilityBoost;
        if (probability > 1.0f) probability = 1.0f; 

        if (dist(g) < probability) {
            SanityManager::Instance().playSanity("scripts/th/sounds/", sanityHud.sanity);
            directorProbabilityBoost = 0.0f;
        }
        else {
            directorProbabilityBoost += 0.1f;
        }
    }
}

void SanityManager::IncSanity(float inc) {
    sanityHud.sanity += inc;
    if (sanityHud.sanity > sanityHud.MAX_SANITY)
        sanityHud.sanity = sanityHud.MAX_SANITY;
    if (sanityHud.sanity <= 0.0f)
        sanityHud.sanity = 0.0f;
}


void SanityManager::triggerFlashback(float sanity) {
    const std::vector<SanityManager::FlashbackEntry>* pool = nullptr;

    if (sanity >= 600.0f) {
        pool = &flashes_mid;
    }
    else if (sanity >= 300.0f) {
        pool = &flashes_high;
    }
    else {
        pool = &flashes_veryhigh;
    }

    if (!pool || pool->empty()) return;

    // elegir un flash aleatorio
    int idx = rand() % pool->size();
    const FlashbackEntry& flash = (*pool)[idx];

    // decirle al HUD que muestre esta imagen
    sanityHud.fireFlashback(flash.image);

    // elegir un sonido de su lista
    if (!flash.sounds.empty()) {
        int sIdx = rand() % flash.sounds.size();
        std::string soundFile = flash.sounds[sIdx];
        SDLPlayer::Instance().playMP3Async("scripts/th/sounds/" + soundFile, 0);
    }
}




void SanityManager::playShuffle(const std::string& path, std::vector<std::string> files, int time) {
    if (files.empty()) return;
    std::vector<std::string> fullPaths;
    for (const auto& f : files) {
        fullPaths.push_back(path + f);
    }
    SDLPlayer::Instance().playPlaylistShuffle(fullPaths, true, time);
}


void SanityManager::playSanity(const std::string& path, float sanity) {
    std::vector<std::string> candidates;

    auto append = [&candidates](const std::vector<std::string>& vec) {
        candidates.insert(candidates.end(), vec.begin(), vec.end());
        };

    // --- decidir si toca shuffle o sonido normal ---
    std::uniform_real_distribution<float> chance(0.0f, 1.0f);

    bool doShuffle = (chance(g) < 0.25f); // 25% probabilidad de shuffle

    if (doShuffle) {
        std::vector<std::function<void()>> shuffleOptions;

        if (sanity >= 400.0f && sanity < 600.0f) {
            shuffleOptions.push_back([this, &path]() { playShuffle(path, shadowhand_sounds, 20000); });
            shuffleOptions.push_back([this, &path]() { playShuffle(path, baby_cries_sounds, 20000); });
        }
        else if (sanity >= 400.0f && sanity < 600.0f) {
            shuffleOptions.push_back([this, &path]() { playShuffle(path, shadowhand_sounds, 20000); });
            shuffleOptions.push_back([this, &path]() { playShuffle(path, baby_cries_sounds, 20000); });
        }
        else if (sanity >= 200.0f && sanity < 400.0f) {
            shuffleOptions.push_back([this, &path]() { playShuffle(path, baby_cries_sounds, 20000); });
            shuffleOptions.push_back([this, &path]() { playShuffle(path, sanity_pulse_sounds, 20000); });
        }
        else if (sanity > 0.0f && sanity < 200.0f) {
            shuffleOptions.push_back([this, &path]() { playShuffle(path, sanity_possession_sounds, 20000); });
            shuffleOptions.push_back([this, &path]() { playShuffle(path, sanity_pulse_sounds, 20000); });
        }
        if (!shuffleOptions.empty()) {
            std::uniform_int_distribution<size_t> dist(0, shuffleOptions.size() - 1);
            shuffleOptions[dist(g)]();
        }
        return;
    }

    if (sanity >= 800.0f) {
        append(low_sounds);
        append(metallic_screeches);
    }
    else if (sanity >= 600.0f) {
        append(low_sounds);
        append(mid_sounds);
        append(insanity_sounds);
        append(metallic_screeches);
    }
    else if (sanity >= 400.0f) {
        append(low_sounds);
        append(mid_sounds);
        append(high_sounds);
        append(insanity_sounds);
        append(metallic_screeches);
    }
    else if (sanity >= 200.0f) {
        append(low_sounds);
        append(mid_sounds);
        append(high_sounds);
        append(veryhigh_sounds);
        append(insanity_sounds);                
        append(metallic_screeches);
    }
    else {
        append(high_sounds);
        append(sanity_pulse_sounds);
        append(sanity_possession_sounds);
        append(insanity_sounds);
        append(veryhigh_sounds);
        append(metallic_screeches);
    }

    if (candidates.empty()) return;

    // --- elegir un sonido aleatorio de la lista ---
    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    std::string file = candidates[dist(g)];

    SDLPlayer::Instance().playMP3Async(path + file, 0);
}