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
    const std::array<OptionalMusic *, 7> tracks = {{
        &audio->menuLoop,
        &audio->baseLoop,
        &audio->forestLoop,
        &audio->swampLoop,
        &audio->ruinsLoop,
        &audio->bossLoop,
        &audio->endingLoop
    }};

    for (OptionalMusic *track : tracks) {
        if (track->loaded) {
            StopMusicStream(track->music);
        }
    }
}

static OptionalMusic *GetSceneMusic(AudioManager *audio, AudioScene scene) {
    switch (scene) {
        case AUDIO_SCENE_MENU:
            return &audio->menuLoop;
        case AUDIO_SCENE_BASE:
            return &audio->baseLoop;
        case AUDIO_SCENE_FOREST:
            return &audio->forestLoop;
        case AUDIO_SCENE_SWAMP:
            return &audio->swampLoop;
        case AUDIO_SCENE_RUINS:
            return &audio->ruinsLoop;
        case AUDIO_SCENE_BOSS:
            return &audio->bossLoop;
        case AUDIO_SCENE_ENDING:
            return &audio->endingLoop;
        case AUDIO_SCENE_NONE:
        default:
            return NULL;
    }
}

static void StopSceneMusic(AudioManager *audio, AudioScene scene) {
    OptionalMusic *music = GetSceneMusic(audio, scene);

    if (music != NULL && music->loaded) {
        StopMusicStream(music->music);
    }
}

static void PlaySceneMusic(AudioManager *audio, AudioScene scene, float volume) {
    OptionalMusic *music = GetSceneMusic(audio, scene);

    if (music != NULL && music->loaded) {
        PlayMusicStream(music->music);
        SetMusicVolume(music->music, volume);
    }
}

static void SetSceneMusicVolume(AudioManager *audio, AudioScene scene, float volume) {
    OptionalMusic *music = GetSceneMusic(audio, scene);

    if (music != NULL && music->loaded) {
        SetMusicVolume(music->music, volume);
    }
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
    audio->baseLoop = LoadOptionalMusicAsset("resources/audio/bgm_base_1.ogg");
    audio->forestLoop = LoadOptionalMusicAsset("resources/audio/bgm_forest_1.ogg");
    audio->swampLoop = LoadOptionalMusicAsset("resources/audio/bgm_swamp_1.ogg");
    audio->ruinsLoop = LoadOptionalMusicAsset("resources/audio/bgm_ruins_1.ogg");
    audio->bossLoop = LoadOptionalMusicAsset("resources/audio/bgm_boss_1.ogg");
    audio->endingLoop = LoadOptionalMusicAsset("resources/audio/bgm_endings_1.ogg");
    audio->masterVolume = 0.80f;
    audio->sfxEnabled = true;
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
    UnloadOptionalMusicAsset(&audio->baseLoop);
    UnloadOptionalMusicAsset(&audio->forestLoop);
    UnloadOptionalMusicAsset(&audio->swampLoop);
    UnloadOptionalMusicAsset(&audio->ruinsLoop);
    UnloadOptionalMusicAsset(&audio->bossLoop);
    UnloadOptionalMusicAsset(&audio->endingLoop);

    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }

    std::memset(audio, 0, sizeof(*audio));
}

void Audio_Update(AudioManager *audio) {
    const float deltaTime = GetFrameTime();
    OptionalMusic *activeMusic;
    OptionalMusic *pendingMusic;

    if (!audio->ready) {
        return;
    }

    activeMusic = GetSceneMusic(audio, audio->activeScene);
    if (activeMusic != NULL && activeMusic->loaded) {
        UpdateMusicStream(activeMusic->music);
    }

    if (audio->pendingScene != AUDIO_SCENE_NONE && audio->pendingScene != audio->activeScene) {
        pendingMusic = GetSceneMusic(audio, audio->pendingScene);
        if (pendingMusic != NULL && pendingMusic->loaded) {
            UpdateMusicStream(pendingMusic->music);
        }
    }

    if (!audio->sceneTransitionActive) {
        if (audio->requestedScene == audio->activeScene) {
            return;
        }

        if (audio->activeScene == AUDIO_SCENE_NONE || activeMusic == NULL || !activeMusic->loaded) {
            StopAllMusic(audio);
            audio->activeScene = audio->requestedScene;
            audio->pendingScene = AUDIO_SCENE_NONE;
            audio->activeSceneStopped = false;
            audio->sceneTransitionTimer = 0.0f;
            if (audio->activeScene != AUDIO_SCENE_NONE) {
                PlaySceneMusic(audio, audio->activeScene, kSceneMusicVolume);
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
        audio->sceneTransitionTimer = 0.0f;
        SetSceneMusicVolume(audio, audio->activeScene, kSceneMusicVolume);
        return;
    }

    audio->sceneTransitionTimer += deltaTime;

    if (!audio->activeSceneStopped) {
        SetSceneMusicVolume(audio,
                            audio->activeScene,
                            kSceneMusicVolume * (1.0f - Clamp01(audio->sceneTransitionTimer / kSceneFadeDuration)));
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
            if (audio->requestedScene != AUDIO_SCENE_NONE) {
                PlaySceneMusic(audio, audio->requestedScene, 0.0f);
                audio->pendingScene = audio->requestedScene;
            }
        }

        if (audio->pendingScene != AUDIO_SCENE_NONE) {
            SetSceneMusicVolume(audio,
                                audio->pendingScene,
                                kSceneMusicVolume
                                    * Clamp01((audio->sceneTransitionTimer - kSceneFadeDuration) / kSceneFadeDuration));
        }
    }

    if (audio->sceneTransitionTimer >= kSceneTransitionDuration) {
        audio->sceneTransitionActive = false;
        audio->activeSceneStopped = false;
        audio->sceneTransitionTimer = 0.0f;
        audio->activeScene = audio->requestedScene;
        if (audio->activeScene != AUDIO_SCENE_NONE) {
            SetSceneMusicVolume(audio, audio->activeScene, kSceneMusicVolume);
        }
        audio->pendingScene = AUDIO_SCENE_NONE;
    }
}

void Audio_ApplySettings(AudioManager *audio, const GameSettings *settings) {
    Audio_SetMasterVolumeSetting(audio, settings->masterVolume);
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

    if (!audio->ready || !audio->sfxEnabled) {
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
        PlaySound(target->sound);
    }
}
