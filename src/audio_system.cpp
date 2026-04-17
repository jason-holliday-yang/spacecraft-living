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
    const std::array<OptionalMusic *, 14> tracks = {{
        &audio->menuLoop,
        &audio->menuLoopAlt,
        &audio->baseLoop,
        &audio->baseLoopAlt,
        &audio->forestLoop,
        &audio->forestRouteLoop,
        &audio->swampLoop,
        &audio->swampDeepLoop,
        &audio->ruinsLoop,
        &audio->ruinsFacilityLoop,
        &audio->bossLoop,
        &audio->bossArenaLoop,
        &audio->endingLoop,
        &audio->endingLoopAlt
    }};

    for (OptionalMusic *track : tracks) {
        if (track->loaded) {
            StopMusicStream(track->music);
        }
    }
}

static void GetSceneMusicPair(AudioManager *audio,
                              AudioScene scene,
                              OptionalMusic **primary,
                              OptionalMusic **secondary) {
    if (primary != NULL) {
        *primary = NULL;
    }
    if (secondary != NULL) {
        *secondary = NULL;
    }

    switch (scene) {
        case AUDIO_SCENE_MENU:
            if (primary != NULL) {
                *primary = &audio->menuLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->menuLoopAlt;
            }
            return;
        case AUDIO_SCENE_BASE:
            if (primary != NULL) {
                *primary = &audio->baseLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->baseLoopAlt;
            }
            return;
        case AUDIO_SCENE_FOREST:
        case AUDIO_SCENE_FOREST_ROUTE:
            if (primary != NULL) {
                *primary = &audio->forestLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->forestRouteLoop;
            }
            return;
        case AUDIO_SCENE_SWAMP:
        case AUDIO_SCENE_SWAMP_DEEP:
            if (primary != NULL) {
                *primary = &audio->swampLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->swampDeepLoop;
            }
            return;
        case AUDIO_SCENE_RUINS:
        case AUDIO_SCENE_RUINS_FACILITY:
            if (primary != NULL) {
                *primary = &audio->ruinsLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->ruinsFacilityLoop;
            }
            return;
        case AUDIO_SCENE_BOSS:
        case AUDIO_SCENE_BOSS_ARENA:
            if (primary != NULL) {
                *primary = &audio->bossLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->bossArenaLoop;
            }
            return;
        case AUDIO_SCENE_ENDING:
            if (primary != NULL) {
                *primary = &audio->endingLoop;
            }
            if (secondary != NULL) {
                *secondary = &audio->endingLoopAlt;
            }
            return;
        case AUDIO_SCENE_NONE:
        default:
            return;
    }
}

static OptionalMusic *GetSceneMusic(AudioManager *audio, AudioScene scene, int variant) {
    OptionalMusic *primary;
    OptionalMusic *secondary;

    primary = NULL;
    secondary = NULL;
    GetSceneMusicPair(audio, scene, &primary, &secondary);
    if (variant == 1 && secondary != NULL && secondary->loaded) {
        return secondary;
    }

    return primary;
}

static void StopSceneMusic(AudioManager *audio, AudioScene scene) {
    OptionalMusic *primary;
    OptionalMusic *secondary;

    primary = NULL;
    secondary = NULL;
    GetSceneMusicPair(audio, scene, &primary, &secondary);
    if (primary != NULL && primary->loaded) {
        StopMusicStream(primary->music);
    }
    if (secondary != NULL && secondary->loaded && secondary != primary) {
        StopMusicStream(secondary->music);
    }
}

static void PlaySceneMusic(AudioManager *audio, AudioScene scene, int variant, float volume) {
    OptionalMusic *music = GetSceneMusic(audio, scene, variant);

    if (music != NULL && music->loaded) {
        StopMusicStream(music->music);
        PlayMusicStream(music->music);
        SetMusicVolume(music->music, volume);
    }
}

static void SetSceneMusicVolume(AudioManager *audio, AudioScene scene, int variant, float volume) {
    OptionalMusic *music = GetSceneMusic(audio, scene, variant);

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

static void UpdateSceneMusic(AudioManager *audio,
                             AudioScene scene,
                             int *variant,
                             float volume) {
    OptionalMusic *music;
    OptionalMusic *nextMusic;

    if (variant == NULL) {
        return;
    }

    music = GetSceneMusic(audio, scene, *variant);
    if (music == NULL || !music->loaded) {
        return;
    }

    UpdateMusicStream(music->music);
    if (!IsMusicNearEnd(music)) {
        return;
    }

    nextMusic = GetSceneMusic(audio, scene, (*variant + 1) % 2);
    StopMusicStream(music->music);
    if (nextMusic != NULL && nextMusic->loaded && nextMusic != music) {
        *variant = (*variant + 1) % 2;
    }
    PlaySceneMusic(audio, scene, *variant, volume);
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

    audio->confirm = LoadOptionalSoundAsset("resources/audio/ui_confirm.wav");
    audio->open = LoadOptionalSoundAsset("resources/audio/ui_open.wav");
    audio->close = LoadOptionalSoundAsset("resources/audio/ui_close.wav");
    audio->warning = LoadOptionalSoundAsset("resources/audio/ui_warning.wav");
    audio->craft = LoadOptionalSoundAsset("resources/audio/ui_craft.wav");
    audio->collect = LoadOptionalSoundAsset("resources/audio/ui_collect.wav");
    audio->log = LoadOptionalSoundAsset("resources/audio/ui_log.wav");
    audio->repair = LoadOptionalSoundAsset("resources/audio/ui_repair.wav");
    audio->hurt = LoadOptionalSoundAsset("resources/audio/combat_hurt.wav");
    audio->melee = LoadOptionalSoundAsset("resources/audio/combat_melee.wav");
    audio->monster = LoadOptionalSoundAsset("resources/audio/monster_roar.wav");
    audio->monolith = LoadOptionalSoundAsset("resources/audio/ambient_monolith.wav");
    audio->stepForest = LoadOptionalSoundAsset("resources/audio/step_forest.wav");
    audio->stepMetal = LoadOptionalSoundAsset("resources/audio/step_metal.wav");
    audio->stepSwamp = LoadOptionalSoundAsset("resources/audio/step_swamp.wav");
    audio->laser = LoadOptionalSoundAsset("resources/audio/combat_laser.wav");
    audio->ending = LoadOptionalSoundAsset("resources/audio/ending_heroic.wav");
    audio->endingPeaceful = LoadOptionalSoundAsset("resources/audio/ending_peaceful.wav");
    audio->endingSettlement = LoadOptionalSoundAsset("resources/audio/ending_settlement.wav");
    audio->menuLoop = LoadOptionalMusicAsset("resources/audio/bgm_menu_1.ogg");
    audio->menuLoopAlt = LoadOptionalMusicAsset("resources/audio/bgm_menu_2.ogg");
    audio->baseLoop = LoadOptionalMusicAsset("resources/audio/bgm_base_1.ogg");
    audio->baseLoopAlt = LoadOptionalMusicAsset("resources/audio/bgm_base_2.ogg");
    audio->forestLoop = LoadOptionalMusicAsset("resources/audio/bgm_forest_1.ogg");
    audio->forestRouteLoop = LoadOptionalMusicAsset("resources/audio/bgm_forest_2.ogg");
    audio->swampLoop = LoadOptionalMusicAsset("resources/audio/bgm_swamp_1.ogg");
    audio->swampDeepLoop = LoadOptionalMusicAsset("resources/audio/bgm_swamp_2.ogg");
    audio->ruinsLoop = LoadOptionalMusicAsset("resources/audio/bgm_ruins_1.ogg");
    audio->ruinsFacilityLoop = LoadOptionalMusicAsset("resources/audio/bgm_ruins_2.ogg");
    audio->bossLoop = LoadOptionalMusicAsset("resources/audio/bgm_boss_1.ogg");
    audio->bossArenaLoop = LoadOptionalMusicAsset("resources/audio/bgm_boss_2.ogg");
    audio->endingLoop = LoadOptionalMusicAsset("resources/audio/bgm_endings_1.ogg");
    audio->endingLoopAlt = LoadOptionalMusicAsset("resources/audio/bgm_endings_2.ogg");
    audio->masterVolume = 1.0f;
    audio->musicVolume = 1.0f;
    audio->sfxVolume = 1.0f;
    audio->sfxEnabled = true;
    audio->activeSceneVariant = 0;
    audio->pendingSceneVariant = 0;
    audio->activeScene = AUDIO_SCENE_NONE;
    audio->requestedScene = AUDIO_SCENE_NONE;
    audio->pendingScene = AUDIO_SCENE_NONE;
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
    UnloadOptionalMusicAsset(&audio->forestLoop);
    UnloadOptionalMusicAsset(&audio->forestRouteLoop);
    UnloadOptionalMusicAsset(&audio->swampLoop);
    UnloadOptionalMusicAsset(&audio->swampDeepLoop);
    UnloadOptionalMusicAsset(&audio->ruinsLoop);
    UnloadOptionalMusicAsset(&audio->ruinsFacilityLoop);
    UnloadOptionalMusicAsset(&audio->bossLoop);
    UnloadOptionalMusicAsset(&audio->bossArenaLoop);
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

    UpdateSceneMusic(audio, audio->activeScene, &audio->activeSceneVariant, GetTargetMusicVolume(audio));
    if (audio->pendingScene != AUDIO_SCENE_NONE && audio->pendingScene != audio->activeScene) {
        UpdateSceneMusic(audio, audio->pendingScene, &audio->pendingSceneVariant, 0.0f);
    }
    activeMusic = GetSceneMusic(audio, audio->activeScene, audio->activeSceneVariant);

    if (!audio->sceneTransitionActive) {
        if (audio->requestedScene == audio->activeScene) {
            return;
        }

        if (audio->activeScene == AUDIO_SCENE_NONE || activeMusic == NULL || !activeMusic->loaded) {
            StopAllMusic(audio);
            audio->activeScene = audio->requestedScene;
            audio->pendingScene = AUDIO_SCENE_NONE;
            audio->activeSceneStopped = false;
            audio->activeSceneVariant = 0;
            audio->pendingSceneVariant = 0;
            audio->sceneTransitionTimer = 0.0f;
            if (audio->activeScene != AUDIO_SCENE_NONE) {
                PlaySceneMusic(audio, audio->activeScene, audio->activeSceneVariant, GetTargetMusicVolume(audio));
            }
            return;
        }

        audio->sceneTransitionActive = true;
        audio->activeSceneStopped = false;
        audio->pendingScene = AUDIO_SCENE_NONE;
        audio->sceneTransitionTimer = 0.0f;
    }

    if (audio->requestedScene == audio->activeScene && !audio->activeSceneStopped) {
        audio->sceneTransitionActive = false;
        audio->pendingScene = AUDIO_SCENE_NONE;
        audio->pendingSceneVariant = 0;
        audio->sceneTransitionTimer = 0.0f;
        SetSceneMusicVolume(audio, audio->activeScene, audio->activeSceneVariant, GetTargetMusicVolume(audio));
        return;
    }

    audio->sceneTransitionTimer += deltaTime;

    if (!audio->activeSceneStopped) {
        SetSceneMusicVolume(audio,
                            audio->activeScene,
                            audio->activeSceneVariant,
                            GetTargetMusicVolume(audio) * (1.0f - Clamp01(audio->sceneTransitionTimer / kSceneFadeDuration)));
        if (audio->sceneTransitionTimer >= kSceneFadeDuration) {
            StopSceneMusic(audio, audio->activeScene);
            audio->activeSceneStopped = true;
        }
    }

    if (audio->activeSceneStopped) {
        if (audio->pendingScene != audio->requestedScene) {
            if (audio->pendingScene != AUDIO_SCENE_NONE) {
                StopSceneMusic(audio, audio->pendingScene);
            }
            audio->pendingScene = AUDIO_SCENE_NONE;
            audio->pendingSceneVariant = 0;
            if (audio->requestedScene != AUDIO_SCENE_NONE) {
                PlaySceneMusic(audio, audio->requestedScene, audio->pendingSceneVariant, 0.0f);
                audio->pendingScene = audio->requestedScene;
            }
        }

        if (audio->pendingScene != AUDIO_SCENE_NONE) {
            SetSceneMusicVolume(audio,
                                audio->pendingScene,
                                audio->pendingSceneVariant,
                                GetTargetMusicVolume(audio)
                                    * Clamp01((audio->sceneTransitionTimer - kSceneFadeDuration) / kSceneFadeDuration));
        }
    }

    if (audio->sceneTransitionTimer >= kSceneTransitionDuration) {
        audio->sceneTransitionActive = false;
        audio->activeSceneStopped = false;
        audio->sceneTransitionTimer = 0.0f;
        audio->activeScene = audio->requestedScene;
        audio->activeSceneVariant = audio->pendingSceneVariant;
        if (audio->activeScene != AUDIO_SCENE_NONE) {
            SetSceneMusicVolume(audio, audio->activeScene, audio->activeSceneVariant, GetTargetMusicVolume(audio));
        }
        audio->pendingScene = AUDIO_SCENE_NONE;
        audio->pendingSceneVariant = 0;
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
    if (audio->activeScene != AUDIO_SCENE_NONE) {
        SetSceneMusicVolume(audio, audio->activeScene, audio->activeSceneVariant, GetTargetMusicVolume(audio));
    }
    if (audio->pendingScene != AUDIO_SCENE_NONE) {
        SetSceneMusicVolume(audio, audio->pendingScene, audio->pendingSceneVariant, GetTargetMusicVolume(audio));
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

void Audio_SetScene(AudioManager *audio, AudioScene scene) {
    if (!audio->ready) {
        return;
    }

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
