#include "audio_system.h"
#include "resource_path.h"

#include <array>
#include <cstring>

static constexpr float kSceneMusicVolume = 0.72f;
static constexpr float kSceneTransitionDuration = 3.0f;
static constexpr float kSceneFadeDuration = kSceneTransitionDuration * 0.5f;

static float Clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

static float GetTargetMusicVolume(const AudioManager *audio) {
    if (audio == NULL) {
        return 0.0f;
    }

    return kSceneMusicVolume * Clamp01(audio->musicVolume);
}

static OptionalSound LoadOptionalSoundAsset(const char *path) {
    OptionalSound asset;
    char resolvedPath[2048];

    std::memset(&asset, 0, sizeof(asset));
    if (ResourcePath_Resolve(path, resolvedPath, sizeof(resolvedPath))) {
        asset.sound = LoadSound(resolvedPath);
        asset.loaded = IsSoundValid(asset.sound);
    }
    return asset;
}

static OptionalMusic LoadOptionalMusicAsset(const char *path) {
    OptionalMusic asset;
    char resolvedPath[2048];

    std::memset(&asset, 0, sizeof(asset));
    if (ResourcePath_Resolve(path, resolvedPath, sizeof(resolvedPath))) {
        asset.music = LoadMusicStream(resolvedPath);
        asset.loaded = IsMusicValid(asset.music);
        if (asset.loaded) {
            asset.music.looping = false;
        }
    }
    return asset;
}

static void UnloadOptionalSoundAsset(OptionalSound *asset) {
    if (asset->loaded) {
        UnloadSound(asset->sound);
    }
    std::memset(asset, 0, sizeof(*asset));
}

static void UnloadOptionalMusicAsset(OptionalMusic *asset) {
    if (asset->loaded) {
        UnloadMusicStream(asset->music);
    }
    std::memset(asset, 0, sizeof(*asset));
}

static void StopAllMusic(AudioManager *audio) {
    const std::array<OptionalMusic *, 24> tracks = {{
        &audio->menuLoop,
        &audio->menuLoopAlt,
        &audio->baseLoop,
        &audio->baseLoopAlt,
        &audio->stage3WildLoop,
        &audio->stage3WildLoopAlt,
        &audio->stage4RiskLoop,
        &audio->stage4RiskLoopAlt,
        &audio->stage5PowerLoop,
        &audio->stage5PowerLoopAlt,
        &audio->stage7ChoiceLoop,
        &audio->stage7ChoiceLoopAlt,
        &audio->forestLoop,
        &audio->forestRouteLoop,
        &audio->swampLoop,
        &audio->swampDeepLoop,
        &audio->ruinsLoop,
        &audio->ruinsFacilityLoop,
        &audio->bossLoop,
        &audio->bossArenaLoop,
        &audio->settlementLoop,
        &audio->settlementLoopAlt,
        &audio->endingLoop,
        &audio->endingLoopAlt
    }};

    for (OptionalMusic *track : tracks) {
        if (track->loaded) {
            StopMusicStream(track->music);
        }
    }
}

static AudioMusicStage MapSceneToMusicStage(AudioScene scene) {
    switch (scene) {
        case AUDIO_SCENE_MENU:
            return AUDIO_MUSIC_MENU;
        case AUDIO_SCENE_BASE:
            return AUDIO_MUSIC_STAGE_1_WAKE;
        case AUDIO_SCENE_FOREST:
            return AUDIO_MUSIC_STAGE_3_WILD;
        case AUDIO_SCENE_FOREST_ROUTE:
            return AUDIO_MUSIC_STAGE_4_RISK;
        case AUDIO_SCENE_SWAMP:
        case AUDIO_SCENE_SWAMP_DEEP:
            return AUDIO_MUSIC_STAGE_5_POWER;
        case AUDIO_SCENE_RUINS:
        case AUDIO_SCENE_RUINS_FACILITY:
            return AUDIO_MUSIC_STAGE_6_RELICS;
        case AUDIO_SCENE_BOSS:
        case AUDIO_SCENE_BOSS_ARENA:
            return AUDIO_MUSIC_BOSS;
        case AUDIO_SCENE_ENDING:
            return AUDIO_MUSIC_ENDING;
        case AUDIO_SCENE_NONE:
        default:
            return AUDIO_MUSIC_NONE;
    }
}

static void GetMusicStagePair(AudioManager *audio,
                              AudioMusicStage stage,
                              OptionalMusic **primary,
                              OptionalMusic **secondary) {
    if (primary != NULL) {
        *primary = NULL;
    }
    if (secondary != NULL) {
        *secondary = NULL;
    }

    switch (stage) {
        case AUDIO_MUSIC_MENU:
            if (primary != NULL) {
                *primary = &audio->menuLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->menuLoopAlt;
            }
            return;
        case AUDIO_MUSIC_STAGE_1_WAKE:
            if (primary != NULL) {
                *primary = &audio->baseLoop;
            }
            return;
        case AUDIO_MUSIC_STAGE_2_FIRST_STEPS:
            if (primary != NULL) {
                *primary = &audio->baseLoopAlt;
            }
            return;
        case AUDIO_MUSIC_STAGE_3_WILD:
            if (primary != NULL) {
                *primary = &audio->stage3WildLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->stage3WildLoopAlt;
            }
            return;
        case AUDIO_MUSIC_STAGE_4_RISK:
            if (primary != NULL) {
                *primary = &audio->stage4RiskLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->stage4RiskLoopAlt;
            }
            return;
        case AUDIO_MUSIC_STAGE_5_POWER:
            if (primary != NULL) {
                *primary = &audio->stage5PowerLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->stage5PowerLoopAlt;
            }
            return;
        case AUDIO_MUSIC_STAGE_6_RELICS:
            if (primary != NULL) {
                *primary = &audio->ruinsLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->ruinsFacilityLoop;
            }
            return;
        case AUDIO_MUSIC_STAGE_7_CHOICE:
            if (primary != NULL) {
                *primary = &audio->stage7ChoiceLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->stage7ChoiceLoopAlt;
            }
            return;
        case AUDIO_MUSIC_ROUTE_HERO:
        case AUDIO_MUSIC_BOSS:
            if (primary != NULL) {
                *primary = &audio->bossLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->bossArenaLoop;
            }
            return;
        case AUDIO_MUSIC_ROUTE_PEACEFUL:
            if (primary != NULL) {
                *primary = &audio->endingLoop;
            }
            return;
        case AUDIO_MUSIC_ROUTE_SETTLEMENT:
            if (primary != NULL) {
                *primary = &audio->settlementLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->settlementLoopAlt;
            }
            return;
        case AUDIO_MUSIC_ENDING:
            if (primary != NULL) {
                *primary = &audio->endingLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->endingLoopAlt;
            }
            return;
        case AUDIO_MUSIC_NONE:
        default:
            return;
    }
}

static OptionalMusic *GetMusicStageTrack(AudioManager *audio, AudioMusicStage stage, int variant) {
    OptionalMusic *primary;
    OptionalMusic *secondary;

    primary = NULL;
    secondary = NULL;
    GetMusicStagePair(audio, stage, &primary, &secondary);
    if (variant == 1 && secondary != NULL && secondary->loaded) {
        return secondary;
    }

    return primary;
}

static int GetDefaultMusicStageVariant(AudioMusicStage) {
    return 0;
}

static void StopMusicStage(AudioManager *audio, AudioMusicStage stage) {
    OptionalMusic *primary;
    OptionalMusic *secondary;

    primary = NULL;
    secondary = NULL;
    GetMusicStagePair(audio, stage, &primary, &secondary);
    if (primary != NULL && primary->loaded) {
        StopMusicStream(primary->music);
    }
    if (secondary != NULL && secondary->loaded && secondary != primary) {
        StopMusicStream(secondary->music);
    }
}

static void PlayMusicStage(AudioManager *audio, AudioMusicStage stage, int variant, float volume) {
    OptionalMusic *music = GetMusicStageTrack(audio, stage, variant);

    if (music != NULL && music->loaded) {
        StopMusicStream(music->music);
        PlayMusicStream(music->music);
        SetMusicVolume(music->music, volume);
    }
}

static void SetMusicStageVolume(AudioManager *audio, AudioMusicStage stage, int variant, float volume) {
    OptionalMusic *music = GetMusicStageTrack(audio, stage, variant);

    if (music != NULL && music->loaded) {
        SetMusicVolume(music->music, volume);
    }
}

static bool IsMusicNearEnd(const OptionalMusic *music) {
    float length;
    float played;

    if (music == NULL || !music->loaded) {
        return false;
    }

    length = GetMusicTimeLength(music->music);
    played = GetMusicTimePlayed(music->music);
    return length > 0.2f && played >= length - 0.08f;
}

static void UpdateMusicStage(AudioManager *audio,
                             AudioMusicStage stage,
                             int *variant,
                             float volume) {
    OptionalMusic *music;
    OptionalMusic *nextMusic;

    if (variant == NULL) {
        return;
    }

    music = GetMusicStageTrack(audio, stage, *variant);
    if (music == NULL || !music->loaded) {
        return;
    }

    UpdateMusicStream(music->music);
    if (!IsMusicNearEnd(music)) {
        return;
    }

    nextMusic = GetMusicStageTrack(audio, stage, (*variant + 1) % 2);
    StopMusicStream(music->music);
    if (nextMusic != NULL && nextMusic->loaded && nextMusic != music) {
        *variant = (*variant + 1) % 2;
    }
    PlayMusicStage(audio, stage, *variant, volume);
}

void Audio_Init(AudioManager *audio) {
    std::memset(audio, 0, sizeof(*audio));

    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    audio->ready = IsAudioDeviceReady();
    if (!audio->ready) {
        return;
    }

    audio->confirm = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_confirm.wav");
    audio->open = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_open.wav");
    audio->close = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_close.wav");
    audio->warning = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_warning.wav");
    audio->craft = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_craft.wav");
    audio->collect = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_collect.wav");
    audio->log = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_log.wav");
    audio->repair = LoadOptionalSoundAsset("resources/audio/sfx/ui/ui_repair.wav");
    audio->hurt = LoadOptionalSoundAsset("resources/audio/sfx/combat/combat_hurt.wav");
    audio->melee = LoadOptionalSoundAsset("resources/audio/sfx/combat/combat_melee.wav");
    audio->monster = LoadOptionalSoundAsset("resources/audio/sfx/combat/monster_roar.wav");
    audio->monolith = LoadOptionalSoundAsset("resources/audio/sfx/ambient/ambient_monolith.wav");
    audio->stepForest = LoadOptionalSoundAsset("resources/audio/sfx/footsteps/step_forest.wav");
    audio->stepMetal = LoadOptionalSoundAsset("resources/audio/sfx/footsteps/step_metal.wav");
    audio->stepSwamp = LoadOptionalSoundAsset("resources/audio/sfx/footsteps/step_swamp.wav");
    audio->laser = LoadOptionalSoundAsset("resources/audio/sfx/combat/combat_laser.wav");
    audio->ending = LoadOptionalSoundAsset("resources/audio/sfx/endings/ending_heroic.wav");
    audio->endingPeaceful = LoadOptionalSoundAsset("resources/audio/sfx/endings/ending_peaceful.wav");
    audio->endingSettlement = LoadOptionalSoundAsset("resources/audio/sfx/endings/ending_settlement.wav");
    audio->menuLoop = LoadOptionalMusicAsset("resources/audio/music/menu/bgm_menu_1.ogg");
    audio->menuLoopAlt = LoadOptionalMusicAsset("resources/audio/music/menu/bgm_menu_2.ogg");
    audio->baseLoop = LoadOptionalMusicAsset("resources/audio/music/base/bgm_base_1.ogg");
    audio->baseLoopAlt = LoadOptionalMusicAsset("resources/audio/music/base/bgm_base_2.ogg");
    audio->stage3WildLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage3_into_the_wild_1.ogg");
    audio->stage3WildLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage3_into_the_wild_2.ogg");
    audio->stage4RiskLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage4_rising_risk_1.ogg");
    audio->stage4RiskLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage4_rising_risk_2.ogg");
    audio->stage5PowerLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage5_power_mire_1.ogg");
    audio->stage5PowerLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage5_power_mire_2.ogg");
    audio->stage7ChoiceLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage7_final_choice_1.ogg");
    audio->stage7ChoiceLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage7_final_choice_2.ogg");
    audio->forestLoop = LoadOptionalMusicAsset("resources/audio/music/forest/bgm_forest_1.ogg");
    audio->forestRouteLoop = LoadOptionalMusicAsset("resources/audio/music/forest/bgm_forest_2.ogg");
    audio->swampLoop = LoadOptionalMusicAsset("resources/audio/music/swamp/bgm_swamp_1.ogg");
    audio->swampDeepLoop = LoadOptionalMusicAsset("resources/audio/music/swamp/bgm_swamp_2.ogg");
    audio->ruinsLoop = LoadOptionalMusicAsset("resources/audio/music/ruins/bgm_ruins_1.ogg");
    audio->ruinsFacilityLoop = LoadOptionalMusicAsset("resources/audio/music/ruins/bgm_ruins_2.ogg");
    audio->bossLoop = LoadOptionalMusicAsset("resources/audio/music/boss/bgm_boss_1.ogg");
    audio->bossArenaLoop = LoadOptionalMusicAsset("resources/audio/music/boss/bgm_boss_2.ogg");
    audio->settlementLoop = LoadOptionalMusicAsset("resources/audio/music/routes/bgm_route_settlement_1.ogg");
    audio->settlementLoopAlt = LoadOptionalMusicAsset("resources/audio/music/routes/bgm_route_settlement_2.ogg");
    audio->endingLoop = LoadOptionalMusicAsset("resources/audio/music/endings/bgm_endings_1.ogg");
    audio->endingLoopAlt = LoadOptionalMusicAsset("resources/audio/music/endings/bgm_endings_2.ogg");
    audio->masterVolume = 1.0f;
    audio->musicVolume = 1.0f;
    audio->sfxVolume = 1.0f;
    audio->sfxEnabled = true;
    audio->activeSceneVariant = 0;
    audio->pendingSceneVariant = 0;
    audio->activeMusicVariant = 0;
    audio->pendingMusicVariant = 0;
    audio->activeScene = AUDIO_SCENE_NONE;
    audio->requestedScene = AUDIO_SCENE_NONE;
    audio->pendingScene = AUDIO_SCENE_NONE;
    audio->activeMusicStage = AUDIO_MUSIC_NONE;
    audio->requestedMusicStage = AUDIO_MUSIC_NONE;
    audio->pendingMusicStage = AUDIO_MUSIC_NONE;
    SetMasterVolume(audio->masterVolume);
}

void Audio_Shutdown(AudioManager *audio) {
    if (!audio->ready) {
        return;
    }

    StopAllMusic(audio);
    UnloadOptionalSoundAsset(&audio->confirm);
    UnloadOptionalSoundAsset(&audio->open);
    UnloadOptionalSoundAsset(&audio->close);
    UnloadOptionalSoundAsset(&audio->warning);
    UnloadOptionalSoundAsset(&audio->craft);
    UnloadOptionalSoundAsset(&audio->collect);
    UnloadOptionalSoundAsset(&audio->log);
    UnloadOptionalSoundAsset(&audio->repair);
    UnloadOptionalSoundAsset(&audio->hurt);
    UnloadOptionalSoundAsset(&audio->melee);
    UnloadOptionalSoundAsset(&audio->monster);
    UnloadOptionalSoundAsset(&audio->monolith);
    UnloadOptionalSoundAsset(&audio->stepForest);
    UnloadOptionalSoundAsset(&audio->stepMetal);
    UnloadOptionalSoundAsset(&audio->stepSwamp);
    UnloadOptionalSoundAsset(&audio->laser);
    UnloadOptionalSoundAsset(&audio->ending);
    UnloadOptionalSoundAsset(&audio->endingPeaceful);
    UnloadOptionalSoundAsset(&audio->endingSettlement);
    UnloadOptionalMusicAsset(&audio->menuLoop);
    UnloadOptionalMusicAsset(&audio->menuLoopAlt);
    UnloadOptionalMusicAsset(&audio->baseLoop);
    UnloadOptionalMusicAsset(&audio->baseLoopAlt);
    UnloadOptionalMusicAsset(&audio->stage3WildLoop);
    UnloadOptionalMusicAsset(&audio->stage3WildLoopAlt);
    UnloadOptionalMusicAsset(&audio->stage4RiskLoop);
    UnloadOptionalMusicAsset(&audio->stage4RiskLoopAlt);
    UnloadOptionalMusicAsset(&audio->stage5PowerLoop);
    UnloadOptionalMusicAsset(&audio->stage5PowerLoopAlt);
    UnloadOptionalMusicAsset(&audio->stage7ChoiceLoop);
    UnloadOptionalMusicAsset(&audio->stage7ChoiceLoopAlt);
    UnloadOptionalMusicAsset(&audio->forestLoop);
    UnloadOptionalMusicAsset(&audio->forestRouteLoop);
    UnloadOptionalMusicAsset(&audio->swampLoop);
    UnloadOptionalMusicAsset(&audio->swampDeepLoop);
    UnloadOptionalMusicAsset(&audio->ruinsLoop);
    UnloadOptionalMusicAsset(&audio->ruinsFacilityLoop);
    UnloadOptionalMusicAsset(&audio->bossLoop);
    UnloadOptionalMusicAsset(&audio->bossArenaLoop);
    UnloadOptionalMusicAsset(&audio->settlementLoop);
    UnloadOptionalMusicAsset(&audio->settlementLoopAlt);
    UnloadOptionalMusicAsset(&audio->endingLoop);
    UnloadOptionalMusicAsset(&audio->endingLoopAlt);

    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }

    std::memset(audio, 0, sizeof(*audio));
}

void Audio_Update(AudioManager *audio) {
    const float deltaTime = GetFrameTime();
    OptionalMusic *activeMusic;

    if (!audio->ready) {
        return;
    }

    UpdateMusicStage(audio, audio->activeMusicStage, &audio->activeMusicVariant, GetTargetMusicVolume(audio));
    if (audio->pendingMusicStage != AUDIO_MUSIC_NONE && audio->pendingMusicStage != audio->activeMusicStage) {
        UpdateMusicStage(audio, audio->pendingMusicStage, &audio->pendingMusicVariant, 0.0f);
    }
    activeMusic = GetMusicStageTrack(audio, audio->activeMusicStage, audio->activeMusicVariant);

    if (!audio->sceneTransitionActive) {
        if (audio->requestedMusicStage == audio->activeMusicStage) {
            return;
        }

        if (audio->activeMusicStage == AUDIO_MUSIC_NONE || activeMusic == NULL || !activeMusic->loaded) {
            StopAllMusic(audio);
            audio->activeMusicStage = audio->requestedMusicStage;
            audio->pendingMusicStage = AUDIO_MUSIC_NONE;
            audio->activeSceneStopped = false;
            audio->activeMusicVariant = GetDefaultMusicStageVariant(audio->activeMusicStage);
            audio->pendingMusicVariant = 0;
            audio->sceneTransitionTimer = 0.0f;
            if (audio->activeMusicStage != AUDIO_MUSIC_NONE) {
                PlayMusicStage(audio, audio->activeMusicStage, audio->activeMusicVariant, GetTargetMusicVolume(audio));
            }
            return;
        }

        audio->sceneTransitionActive = true;
        audio->activeSceneStopped = false;
        audio->pendingMusicStage = AUDIO_MUSIC_NONE;
        audio->sceneTransitionTimer = 0.0f;
    }

    if (audio->requestedMusicStage == audio->activeMusicStage && !audio->activeSceneStopped) {
        audio->sceneTransitionActive = false;
        audio->pendingMusicStage = AUDIO_MUSIC_NONE;
        audio->pendingMusicVariant = 0;
        audio->sceneTransitionTimer = 0.0f;
        SetMusicStageVolume(audio, audio->activeMusicStage, audio->activeMusicVariant, GetTargetMusicVolume(audio));
        return;
    }

    audio->sceneTransitionTimer += deltaTime;

    if (!audio->activeSceneStopped) {
        SetMusicStageVolume(audio,
                            audio->activeMusicStage,
                            audio->activeMusicVariant,
                            GetTargetMusicVolume(audio) * (1.0f - Clamp01(audio->sceneTransitionTimer / kSceneFadeDuration)));
        if (audio->sceneTransitionTimer >= kSceneFadeDuration) {
            StopMusicStage(audio, audio->activeMusicStage);
            audio->activeSceneStopped = true;
        }
    }

    if (audio->activeSceneStopped) {
        if (audio->pendingMusicStage != audio->requestedMusicStage) {
            if (audio->pendingMusicStage != AUDIO_MUSIC_NONE) {
                StopMusicStage(audio, audio->pendingMusicStage);
            }
            audio->pendingMusicStage = AUDIO_MUSIC_NONE;
            audio->pendingMusicVariant = 0;
            if (audio->requestedMusicStage != AUDIO_MUSIC_NONE) {
                audio->pendingMusicVariant = GetDefaultMusicStageVariant(audio->requestedMusicStage);
                PlayMusicStage(audio, audio->requestedMusicStage, audio->pendingMusicVariant, 0.0f);
                audio->pendingMusicStage = audio->requestedMusicStage;
            }
        }

        if (audio->pendingMusicStage != AUDIO_MUSIC_NONE) {
            SetMusicStageVolume(audio,
                                audio->pendingMusicStage,
                                audio->pendingMusicVariant,
                                GetTargetMusicVolume(audio)
                                    * Clamp01((audio->sceneTransitionTimer - kSceneFadeDuration) / kSceneFadeDuration));
        }
    }

    if (audio->sceneTransitionTimer >= kSceneTransitionDuration) {
        audio->sceneTransitionActive = false;
        audio->activeSceneStopped = false;
        audio->sceneTransitionTimer = 0.0f;
        audio->activeMusicStage = audio->requestedMusicStage;
        audio->activeMusicVariant = audio->pendingMusicVariant;
        if (audio->activeMusicStage != AUDIO_MUSIC_NONE) {
            SetMusicStageVolume(audio, audio->activeMusicStage, audio->activeMusicVariant, GetTargetMusicVolume(audio));
        }
        audio->pendingMusicStage = AUDIO_MUSIC_NONE;
        audio->pendingMusicVariant = 0;
    }
}

void Audio_ApplySettings(AudioManager *audio, const GameSettings *settings) {
    Audio_SetMasterVolumeSetting(audio, settings->masterVolume);
    Audio_SetMusicVolumeSetting(audio, settings->musicVolume);
    Audio_SetSfxVolumeSetting(audio, settings->sfxVolume);
    Audio_SetSfxEnabled(audio, settings->sfxEnabled);
}

void Audio_SetMasterVolumeSetting(AudioManager *audio, float volume) {
    if (!audio->ready) {
        return;
    }

    if (volume < 0.0f) {
        volume = 0.0f;
    }
    if (volume > 1.0f) {
        volume = 1.0f;
    }

    audio->masterVolume = volume;
    SetMasterVolume(audio->masterVolume);
}

void Audio_SetMusicVolumeSetting(AudioManager *audio, float volume) {
    if (!audio->ready) {
        return;
    }

    audio->musicVolume = Clamp01(volume);
    if (audio->activeMusicStage != AUDIO_MUSIC_NONE) {
        SetMusicStageVolume(audio, audio->activeMusicStage, audio->activeMusicVariant, GetTargetMusicVolume(audio));
    }
    if (audio->pendingMusicStage != AUDIO_MUSIC_NONE) {
        SetMusicStageVolume(audio, audio->pendingMusicStage, audio->pendingMusicVariant, GetTargetMusicVolume(audio));
    }
}

void Audio_SetSfxVolumeSetting(AudioManager *audio, float volume) {
    if (!audio->ready) {
        return;
    }

    audio->sfxVolume = Clamp01(volume);
}

void Audio_SetSfxEnabled(AudioManager *audio, bool enabled) {
    audio->sfxEnabled = enabled;
}

void Audio_SetMusicStage(AudioManager *audio, AudioMusicStage stage) {
    if (!audio->ready) {
        return;
    }

    audio->requestedMusicStage = stage;
}

void Audio_SetScene(AudioManager *audio, AudioScene scene) {
    if (!audio->ready) {
        return;
    }

    Audio_SetMusicStage(audio, MapSceneToMusicStage(scene));
    audio->requestedScene = scene;
}

void Audio_PlayCue(AudioManager *audio, AudioCue cue) {
    OptionalSound *target;

    if (!audio->ready || !audio->sfxEnabled || audio->sfxVolume <= 0.001f) {
        return;
    }

    target = NULL;
    switch (cue) {
        case AUDIO_CUE_CONFIRM:
            target = &audio->confirm;
            break;
        case AUDIO_CUE_OPEN:
            target = &audio->open;
            break;
        case AUDIO_CUE_CLOSE:
            target = &audio->close;
            break;
        case AUDIO_CUE_WARNING:
            target = &audio->warning;
            break;
        case AUDIO_CUE_CRAFT:
            target = &audio->craft;
            break;
        case AUDIO_CUE_COLLECT:
            target = &audio->collect;
            break;
        case AUDIO_CUE_LOG:
            target = &audio->log;
            break;
        case AUDIO_CUE_REPAIR:
            target = &audio->repair;
            break;
        case AUDIO_CUE_HURT:
            target = &audio->hurt;
            break;
        case AUDIO_CUE_MELEE:
            target = &audio->melee;
            break;
        case AUDIO_CUE_MONSTER:
            target = &audio->monster;
            break;
        case AUDIO_CUE_MONOLITH:
            target = &audio->monolith;
            break;
        case AUDIO_CUE_STEP_FOREST:
            target = &audio->stepForest;
            break;
        case AUDIO_CUE_STEP_METAL:
            target = &audio->stepMetal;
            break;
        case AUDIO_CUE_STEP_SWAMP:
            target = &audio->stepSwamp;
            break;
        case AUDIO_CUE_LASER:
            target = &audio->laser;
            break;
        case AUDIO_CUE_ENDING:
            target = &audio->ending;
            break;
        case AUDIO_CUE_ENDING_PEACEFUL:
            target = &audio->endingPeaceful;
            break;
        case AUDIO_CUE_ENDING_SETTLEMENT:
            target = &audio->endingSettlement;
            break;
        default:
            break;
    }

    if (target != NULL && target->loaded) {
        SetSoundVolume(target->sound, Clamp01(audio->sfxVolume));
        PlaySound(target->sound);
    }
}
