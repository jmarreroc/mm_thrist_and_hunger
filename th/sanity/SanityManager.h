#pragma once
#include <vector>
#include <string>
#include <th/tools/SDLPlayer2.h>

class SanityManager {
public:
    static SanityManager& Instance();

    void Update(float dt);

    void IncSanity(float inc);

    // ==================== LOW ====================
    std::vector<std::string> low_sounds = {
        "soundscape01.mp3",
        "soundscape06.mp3",
        "gutural-singing01.mp3",
        "low-man-whisper1.mp3",
        "low-man-whisper2.mp3",
        "low-man-whisper3.mp3",
        "low-woman-whisper11.mp3",
        "low-woman-whisper12.mp3",
        "low-woman-whisper13.mp3",
        "low-woman-whisper14.mp3",
        "low-woman-whisper15.mp3",
        "low-woman-whisper16.mp3",
        "low-woman-whisper21.mp3",
        "low-woman-whisper22.mp3",
        "low-woman-whisper23.mp3",
        "low-woman-whisper24.mp3",
        "low-woman-whisper25.mp3",
        "low-woman-whisper26.mp3",
        "low-woman-whisper3.mp3",
        "low-woman-whisper4.mp3",
        "low-woman-cry1.mp3",
        "woman-beware01.mp3",
        "woman-beware02.mp3",
        "woman-can-you-help-me.mp3",
        "woman-where-is-my-son.mp3",
        "woman-run.mp3",
        "woman-run-away.mp3",
        "woman-im-dead.mp3",
        "woman-there-was-blood.mp3"
    };

    // ==================== MID ====================
    std::vector<std::string> mid_sounds = {
        "soundscape02.mp3",
        "soundscape03.mp3",
        "gutural-singing02.mp3",
        "mid-desert-horror1.mp3",
        "mid-desert-horror2.mp3",
        "mid-whispers1.mp3",
        "mid-whispers2.mp3",
        "mid-woman-whisper2.mp3",
        "mid-woman-whisper13.mp3",
        "mid-woman-whisper3.mp3",
        "woman-ghost-singing.mp3"

    };

    // ==================== HIGH ====================
    std::vector<std::string> high_sounds = {
        "soundscape05.mp3",
        "soundscape07.mp3",
        "gutural-singing03.mp3",
        "high-woman-whisper1.mp3",
        "high-desert-horror1.mp3",
        "high-whispers1.mp3",
        "high-whispers2.mp3"
    };

    // ==================== VERY HIGH ====================
    std::vector<std::string> veryhigh_sounds = {
        "soundscape04.mp3",
        "soundscape07.mp3",
        "soundscape08.mp3",
        "veryhigh-whispers1.mp3",
        "veryhigh-whispers2.mp3",
        "veryhigh-whispers3.mp3",
        "veryhigh-whispers4.mp3",
        "veryhigh-possessed-laugh.mp3"
    };

    // ==================== INSANITY ====================
    std::vector<std::string> insanity_sounds = {
        "insanity_baby_cry1.ogg",
        "insanity_baby_cry2.ogg",
        "insanity_baby_cry3.ogg",
        "insanity_monster_roar01.ogg",
        "insanity_monster_roar02.ogg",
        "insanity_monster_roar03.ogg",
        "insanity_whisper01.ogg",
        "insanity_whisper02.ogg",
        "insanity_whisper03.ogg"
    };

    // ==================== SANITY ====================
    std::vector<std::string> baby_cries_sounds = {
        "insanity_baby_cry1.ogg",
        "insanity_baby_cry2.ogg",
        "insanity_baby_cry3.ogg"
    };


    std::vector<std::string> sanity_possession_sounds = {
        "sanity_random_10.mp3",
        "sanity_random_4.mp3",
        "sanity_random_5.mp3",
        "sanity_random_6.mp3",
        "sanity_random_7.mp3",
        "sanity_random_8.mp3",
        "sanity_random_9.mp3",
        "sanity_random_10.mp3",
        "sanity_random_11.mp3",
        "sanity_random_12.mp3",
        "sanity_random_13.mp3",
        "sanity_random_14.mp3",
        "sanity_random_15.mp3",
        "sanity_random_16.mp3",
        "sanity_random_17.mp3",
        "sanity_random_18.mp3",
        "sanity_random_19.mp3"
    };

    std::vector<std::string> sanity_pulse_sounds = {
        "sanity_random_pulse_1.mp3",
        "sanity_random_pulse_2.mp3",
        "sanity_random_pulse_3.mp3",
        "sanity_random_pulse_4.mp3",
        "sanity_random_pulse_5.mp3",
        "sanity_random_pulse_6.mp3",
        "sanity_random_pulse_7.mp3",
        "sanity_random_pulse_8.mp3",
        "sanity_random_pulse_9.mp3",
        "sanity_random_pulse_10.mp3",
        "sanity_random_pulse_11.mp3",
        "sanity_random_pulse_12.mp3",
        "sanity_random_pulse_13.mp3"
    };


    // ==================== SHADOWHAND ====================
    std::vector<std::string> shadowhand_sounds = {
        "shadowhand_creep_1.mp3",
        "shadowhand_creep_2.mp3",
        "shadowhand_creep_3.mp3",
        "shadowhand_creep_4.mp3",
        "shadowhand_creep_5.mp3",
        "shadowhand_creep_6.mp3",
        "shadowhand_creep_7.mp3",
        "shadowhand_creep_8.mp3",
        "shadowhand_creep_9.mp3",
        "shadowhand_creep_10.mp3",
        "shadowhand_creep_11.mp3",
        "shadowhand_creep_12.mp3",
        "shadowhand_creep_13.mp3",
        "shadowhand_creep_14.mp3",
        "shadowhand_creep_15.mp3"
    };

    // =============== METALLIC SCREECHES ==================
    std::vector<std::string> metallic_screeches = {
        "metallic-screeches01.mp3",
        "metallic-screeches02.mp3",
        "metallic-screeches03.mp3",
        "metallic-screeches04.mp3",
        "metallic-screeches05.mp3"
    };


    struct FlashbackEntry {
        std::string image;
        std::vector<std::string> sounds;
    };


    std::vector<FlashbackEntry> flashes_paced = {
        { "scripts/th/textures/mad/flashes/mid2.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/female-horror-voice-i-remember-it-all.mp3", "flashes/woman-cry01.mp3", "flashes/woman-cry02.mp3", "flashes/woman-cry03.mp3", "flashes/woman-ghost01.mp3"}},
        { "scripts/th/textures/mad/flashes/mid3.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/demonic-boy-scream01.mp3", "flashes/demonic-boy-scream02.mp3", "flashes/attackmix_01.mp3", "flashes/attackmix_02.mp3", "flashes/attackmix_03.mp3"} },
        { "scripts/th/textures/mad/flashes/mid4.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/girl-scream02.mp3", "flashes/girl-scream03.mp3", "flashes/girl-scream04.mp3"} },
        { "scripts/th/textures/mad/flashes/mid6.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/help01.mp3", "flashes/walking_cry_1.mp3", "flashes/walking_cry_2.mp3"} },
        { "scripts/th/textures/mad/flashes/high21.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/female-horror-voice-i-remember-it-all.mp3", "flashes/woman-cry01.mp3", "flashes/woman-cry02.mp3", "flashes/woman-cry03.mp3", "flashes/woman-ghost01.mp3" } },
        { "scripts/th/textures/mad/flashes/high22.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/female-horror-pain.mp3", "flashes/woman-cry01.mp3", "flashes/woman-cry02.mp3", "flashes/woman-cry03.mp3", "flashes/woman-ghost01.mp3" } },
        { "scripts/th/textures/mad/flashes/high3.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/demonic-boy-scream01.mp3", "flashes/demonic-boy-scream02.mp3", "flashes/attackmix_01.mp3", "flashes/attackmix_02.mp3", "flashes/attackmix_03.mp3" } },
        { "scripts/th/textures/mad/flashes/high4.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/girl-scream02.mp3", "flashes/girl-scream03.mp3", "flashes/girl-scream04.mp3", "flashes/shout-hurt-scream.mp3" } },        
        { "scripts/th/textures/mad/flashes/high6.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/girl-scream01.mp3", "flashes/woman_scream_01.mp3", "flashes/woman_scream_02.mp3", "flashes/woman-scary-scream01.mp3"}},
        { "scripts/th/textures/mad/flashes/veryhigh21.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/woman-ghost01.mp3" } },
    };


    std::vector<FlashbackEntry> flashes_mid = {
        { "scripts/th/textures/mad/flashes/mid1.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/screech-01.mp3", "flashes/dragon-death.mp3", "flashes/pathetic-screaming.mp3"}},
        { "scripts/th/textures/mad/flashes/mid2.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/female-horror-voice-i-remember-it-all.mp3", "flashes/woman-cry01.mp3", "flashes/woman-cry02.mp3", "flashes/woman-cry03.mp3", "flashes/woman-ghost01.mp3"}},
        { "scripts/th/textures/mad/flashes/mid3.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/demonic-boy-scream01.mp3", "flashes/demonic-boy-scream02.mp3", "flashes/attackmix_01.mp3", "flashes/attackmix_02.mp3", "flashes/attackmix_03.mp3"} },
        { "scripts/th/textures/mad/flashes/mid4.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/girl-scream02.mp3", "flashes/girl-scream03.mp3", "flashes/girl-scream04.mp3"} },
        { "scripts/th/textures/mad/flashes/mid5.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/max-scream01.mp3" } },
        { "scripts/th/textures/mad/flashes/mid6.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/help01.mp3", "flashes/walking_cry_1.mp3", "flashes/walking_cry_2.mp3"} },
        { "scripts/th/textures/mad/flashes/mid7.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/laughter1.mp3", "flashes/laughter2.mp3", "flashes/laughter3.mp3"} },
        { "scripts/th/textures/mad/flashes/mid8.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/laughter_grave1.mp3", "flashes/laughter_grave2.mp3", "flashes/laughter_grave3.mp3"} },
        { "scripts/th/textures/mad/flashes/mid9.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/join-me.mp3", "flashes/inmortan-low01.mp3", "flashes/inmortan-low02.mp3", "flashes/undead-dragon-screech.mp3", "flashes/monster-scream-low01.mp3"}},
        { "scripts/th/textures/mad/flashes/mid10.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/monster-growl.mp3", "flashes/inmortan-low01.mp3", "flashes/inmortan-low02.mp3", "flashes/undead-dragon-screech.mp3", "flashes/dragon-roar01.mp3", "flashes/dragon-roar02.mp3", "flashes/dragon-roar03.mp3", "flashes/monster-scream-low01.mp3"} },
        { "scripts/th/textures/mad/flashes/mid11.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/laughter_grave1.mp3", "flashes/laughter_grave2.mp3", "flashes/laughter_grave3.mp3", "flashes/demon-laugh01.mp3"}},
        { "scripts/th/textures/mad/flashes/mid12.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/monster-laugh01.mp3", "flashes/monster-laugh02.mp3", "flashes/monster-laugh03.mp3", "flashes/monster-growl.mp3", "flashes/inmortan-low01.mp3", "flashes/inmortan-low02.mp3", "flashes/undead-dragon-screech.mp3", "flashes/dragon-roar01.mp3", "flashes/dragon-roar02.mp3", "flashes/dragon-roar03.mp3", "flashes/monster-scream-low01.mp3"} },
        { "scripts/th/textures/mad/flashes/mid13.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/maniac-laugh.mp3", "flashes/maniac-laugh.mp3", "flashes/maniac-laugh.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3"} },
        { "scripts/th/textures/mad/flashes/mid15.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3"} }
    };

    std::vector<FlashbackEntry> flashes_high = {
        { "scripts/th/textures/mad/flashes/high1.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/screech-01.mp3", "flashes/dragon-death.mp3", "flashes/attackmix_01.mp3", "flashes/attackmix_02.mp3", "flashes/attackmix_03.mp3", "flashes/pathetic-screaming.mp3"}},
        { "scripts/th/textures/mad/flashes/high2.png", {  "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/inmortan-low01.mp3", "flashes/inmortan-low02.mp3", "flashes/undead-dragon-screech.mp3"}},
        { "scripts/th/textures/mad/flashes/high21.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/female-horror-voice-i-remember-it-all.mp3", "flashes/woman-cry01.mp3", "flashes/woman-cry02.mp3", "flashes/woman-cry03.mp3", "flashes/woman-ghost01.mp3" } },
        { "scripts/th/textures/mad/flashes/high22.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/female-horror-pain.mp3", "flashes/woman-cry01.mp3", "flashes/woman-cry02.mp3", "flashes/woman-cry03.mp3", "flashes/woman-ghost01.mp3" } },
        { "scripts/th/textures/mad/flashes/high3.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/demonic-boy-scream01.mp3", "flashes/demonic-boy-scream02.mp3", "flashes/attackmix_01.mp3", "flashes/attackmix_02.mp3", "flashes/attackmix_03.mp3" } },
        { "scripts/th/textures/mad/flashes/high4.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/girl-scream02.mp3", "flashes/girl-scream03.mp3", "flashes/girl-scream04.mp3", "flashes/shout-hurt-scream.mp3" } },
        { "scripts/th/textures/mad/flashes/high5.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/max-scream01.mp3" } },
        { "scripts/th/textures/mad/flashes/high51.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/max-scream01.mp3" } },
        { "scripts/th/textures/mad/flashes/high6.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/girl-scream01.mp3", "flashes/woman_scream_01.mp3", "flashes/woman_scream_02.mp3", "flashes/woman-scary-scream01.mp3"}},
        { "scripts/th/textures/mad/flashes/high7.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/dragon-roar01.mp3", "flashes/dragon-roar02.mp3", "flashes/dragon-roar03.mp3" } },
        { "scripts/th/textures/mad/flashes/high8.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/laughter1.mp3", "flashes/laughter2.mp3", "flashes/laughter3.mp3"} },
        { "scripts/th/textures/mad/flashes/high9.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/laughter_grave1.mp3", "flashes/laughter_grave2.mp3", "flashes/laughter_grave3.mp3"} },
        { "scripts/th/textures/mad/flashes/high10.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/dragon-roar01.mp3", "flashes/dragon-roar02.mp3", "flashes/dragon-roar03.mp3", "flashes/max-scream03.mp3"} },
        { "scripts/th/textures/mad/flashes/high11.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/join-me.mp3", "flashes/inmortan-low01.mp3", "flashes/inmortan-low02.mp3", "flashes/undead-dragon-screech.mp3" } },
        { "scripts/th/textures/mad/flashes/high12.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-laugh01.mp3", "flashes/monster-laugh02.mp3", "flashes/monster-laugh03.mp3", "flashes/monster-growl.mp3", "flashes/dragon-roar01.mp3", "flashes/dragon-roar02.mp3", "flashes/dragon-roar03.mp3" } },
        { "scripts/th/textures/mad/flashes/high13.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/max-scream01.mp3", "flashes/dragon-roar01.mp3", "flashes/dragon-roar02.mp3", "flashes/dragon-roar03.mp3" } },
        { "scripts/th/textures/mad/flashes/high14.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/maniac-laugh.mp3", "flashes/maniac-laugh.mp3", "flashes/maniac-laugh.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3"} },
        { "scripts/th/textures/mad/flashes/high15.png", { "flashes/choir-of-death.mp3", "flashes/scales-of-justice.mp3", "flashes/sing-for-me.mp3" }}


    };

    std::vector<FlashbackEntry> flashes_veryhigh = {
        { "scripts/th/textures/mad/flashes/veryhigh1.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/screech-01.mp3", "flashes/shout-hurt-scream.mp3", "flashes/dragon-death.mp3", "flashes/banshee-screaming.mp3", "flashes/pathetic-screaming.mp3"} },
        { "scripts/th/textures/mad/flashes/veryhigh2.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/max-scream03.mp3", "flashes/inmortan-mid01.mp3", "flashes/inmortan-mid02.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh21.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/woman-ghost01.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh3.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/max-scream03.mp3", "flashes/inmortan-mid01.mp3", "flashes/inmortan-mid02.mp3", "flashes/echo-jumpscare.mp3"} },
        { "scripts/th/textures/mad/flashes/veryhigh4.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/girl-scream02.mp3", "flashes/girl-scream03.mp3", "flashes/girl-scream04.mp3", "flashes/shout-hurt-scream.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh5.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/max-scream03.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh51.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/max-scream03.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh6.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/girl-scream01.mp3", "flashes/woman_scream_01.mp3", "flashes/woman_scream_02.mp3", "flashes/woman-scary-scream01.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh7.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/laughter1.mp3", "flashes/laughter2.mp3", "flashes/laughter3.mp3"} },
        { "scripts/th/textures/mad/flashes/veryhigh8.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/dragon-death.mp3", "flashes/banshee-screaming.mp3", "flashes/max-scream03.mp3"} },
        { "scripts/th/textures/mad/flashes/veryhigh9.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/max-scream03.mp3", "flashes/inmortan-mid01.mp3", "flashes/inmortan-mid02.mp3", "flashes/echo-jumpscare.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh10.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/max-scream03.mp3", "flashes/inmortan-mid01.mp3", "flashes/inmortan-mid02.mp3", "flashes/echo-jumpscare.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh11.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3", "flashes/monster-growl.mp3", "flashes/max-scream03.mp3", "flashes/inmortan-mid01.mp3", "flashes/inmortan-mid02.mp3", "flashes/echo-jumpscare.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh12.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/spine-chilling-screeching.mp3","flashes/monster-laugh01.mp3", "flashes/monster-laugh02.mp3", "flashes/monster-laugh03.mp3", "flashes/monster-growl.mp3", "flashes/max-scream03.mp3", "flashes/inmortan-mid01.mp3", "flashes/inmortan-mid02.mp3", "flashes/echo-jumpscare.mp3" } },
        { "scripts/th/textures/mad/flashes/veryhigh13.png", { "flashes/flash_impact.mp3", "flashes/flush.mp3", "flashes/scary-woosh-laugh.mp3", "flashes/boom.mp3", "flashes/maniac-laugh.mp3", "flashes/maniac-laugh.mp3", "flashes/maniac-laugh.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3", "flashes/evil-laughter.mp3"} },
        { "scripts/th/textures/mad/flashes/veryhigh15.png", { "flashes/choir-of-death-mg-vh.mp3", "flashes/scales-of-justice-mg-vh.mp3", "flashes/sing-for-me-mg-vh.mp3" } }


    };


    void playShuffle(const std::string& path, std::vector<std::string> files, int time);
    void playSanity(const std::string& path, float sanity);
    void triggerFlashback(float sanity);
    void triggerPacedFlashback();
    void PreloadFlashTextures(const std::vector<FlashbackEntry>& entries);
    void PreloadAllFlashTextures() {
        PreloadFlashTextures(flashes_paced);
        PreloadFlashTextures(flashes_mid);
        PreloadFlashTextures(flashes_high);
    }
private:

    SanityManager()
        : g(std::random_device{}()), dist(0.0f, 1.0f) {
    }
    
    SanityManager(const SanityManager&) = delete;
    SanityManager& operator=(const SanityManager&) = delete;
    float lastPlayerHealth = -1.0f;
    float timer = 0.0f;
    float director = 0.0f;
    float directorProbabilityBoost = 0.0f;
    float flashbackProbabilityBoost = 0.0f;
    float lastFlashBack = 0.0f;

    bool diedRun = false;
    bool diedLoad = false;

    bool flashesEnabled = false;
    float flashesEnableTimerTriggered = false;
    float flashesEnableTimer = 0.0f;


    float lastScrap = -1.0f;

    std::mt19937 g;
    std::uniform_real_distribution<float> dist;

};