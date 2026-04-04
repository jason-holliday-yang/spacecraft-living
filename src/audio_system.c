#include "audio_system.h"

#include <string.h>

static OptionalSound LoadOptionalSoundAsset(const char *path) {
    OptionalSound asset;

    memset(&asset, 0, sizeof(asset));
    if (FileExists(path)) {
        asset.sound = LoadSound(path);
        asset.loaded = IsSoundValid(asset.sound);
    }
    return asset;
}

static OptionalMusic LoadOptionalMusicAsset(const char *path) {
    OptionalMusic asset;

    memset(&asset, 0, sizeof(asset));
    if (FileExists(path)) {
        asset.music = LoadMusicStream(path);
        asset.loaded = IsMusicValid(asset.music);
    }
    return asset;
}

static void UnloadOptionalSoundAsset(OptionalSound *asset) {
    if (asset->loaded) {
        UnloadSound(asset->sound);
    }
    memset(asset, 0, sizeof(*asset));
}

static void UnloadOptionalMusicAsset(OptionalMusic *asset) {
    if (asset->loaded) {
        UnloadMusicStream(asset->music);
    }
    memset(asset, 0, sizeof(*asset));
}

static void StopAllMusic(AudioManager *audio) {
    OptionalMusic *tracks[] = {
        &audio->menuLoop,
        &audio->baseLoop,
        &audio->forestLoop,
        &audio->swampLoop,
        &audio->ruinsLoop,
        &audio->bossLoop,
        &audio->endingLoop
    };
    int index;

    for (index = 0; index < (int)(sizeof(tracks) / sizeof(tracks[0])); index++) {
        if (tracks[index]->loaded) {
            StopMusicStream(tracks[index]->music);
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

void Audio_Init(AudioManager *audio) {
    memset(audio, 0, sizeof(*audio));

    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    audio->ready = IsAudioDeviceReady();
    if (!audio->ready) {
        return;
    }

    audio->confirm = LoadOptionalSoundAsset("resources/audio/ui_confirm.wav");
    audio->open = LoadOptionalSoundAsset("resources/audio/ui_open.wav");
    audio->warning = LoadOptionalSoundAsset("resources/audio/ui_warning.wav");
    audio->craft = LoadOptionalSoundAsset("resources/audio/ui_craft.wav");
    audio->collect = LoadOptionalSoundAsset("resources/audio/ui_collect.wav");
    audio->repair = LoadOptionalSoundAsset("resources/audio/ui_repair.wav");
    audio->laser = LoadOptionalSoundAsset("resources/audio/combat_laser.wav");
    audio->ending = LoadOptionalSoundAsset("resources/audio/ending_heroic.wav");
    audio->menuLoop = LoadOptionalMusicAsset("resources/audio/bgm_menu.ogg");
    audio->baseLoop = LoadOptionalMusicAsset("resources/audio/bgm_base.ogg");
    audio->forestLoop = LoadOptionalMusicAsset("resources/audio/bgm_forest.ogg");
    audio->swampLoop = LoadOptionalMusicAsset("resources/audio/bgm_swamp.ogg");
    audio->ruinsLoop = LoadOptionalMusicAsset("resources/audio/bgm_ruins.ogg");
    audio->bossLoop = LoadOptionalMusicAsset("resources/audio/bgm_boss.ogg");
    audio->endingLoop = LoadOptionalMusicAsset("resources/audio/bgm_endings.ogg");
    audio->masterVolume = 0.80f;
    audio->sfxEnabled = true;
    audio->activeScene = AUDIO_SCENE_NONE;
    SetMasterVolume(audio->masterVolume);
}

void Audio_Shutdown(AudioManager *audio) {
    if (!audio->ready) {
        return;
    }

    StopAllMusic(audio);
    UnloadOptionalSoundAsset(&audio->confirm);
    UnloadOptionalSoundAsset(&audio->open);
    UnloadOptionalSoundAsset(&audio->warning);
    UnloadOptionalSoundAsset(&audio->craft);
    UnloadOptionalSoundAsset(&audio->collect);
    UnloadOptionalSoundAsset(&audio->repair);
    UnloadOptionalSoundAsset(&audio->laser);
    UnloadOptionalSoundAsset(&audio->ending);
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

    memset(audio, 0, sizeof(*audio));
}

void Audio_Update(AudioManager *audio) {
    OptionalMusic *music;

    if (!audio->ready) {
        return;
    }

    music = GetSceneMusic(audio, audio->activeScene);
    if (music != NULL && music->loaded) {
        UpdateMusicStream(music->music);
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
    OptionalMusic *music;

    if (!audio->ready || audio->activeScene == scene) {
        return;
    }

    StopAllMusic(audio);
    audio->activeScene = scene;
    music = GetSceneMusic(audio, scene);
    if (music != NULL && music->loaded) {
        PlayMusicStream(music->music);
        SetMusicVolume(music->music, 0.72f);
    }
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
        case AUDIO_CUE_WARNING:
            target = &audio->warning;
            break;
        case AUDIO_CUE_CRAFT:
            target = &audio->craft;
            break;
        case AUDIO_CUE_COLLECT:
            target = &audio->collect;
            break;
        case AUDIO_CUE_REPAIR:
            target = &audio->repair;
            break;
        case AUDIO_CUE_LASER:
            target = &audio->laser;
            break;
        case AUDIO_CUE_ENDING:
            target = &audio->ending;
            break;
        default:
            break;
    }

    if (target != NULL && target->loaded) {
        PlaySound(target->sound);
    }
}
