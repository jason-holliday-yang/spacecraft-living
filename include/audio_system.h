#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <stdbool.h>
#include "raylib.h"
#include "save_system.h"

typedef enum AudioCue {
    AUDIO_CUE_CONFIRM = 0,
    AUDIO_CUE_OPEN,
    AUDIO_CUE_WARNING,
    AUDIO_CUE_CRAFT,
    AUDIO_CUE_COLLECT,
    AUDIO_CUE_REPAIR,
    AUDIO_CUE_LASER,
    AUDIO_CUE_ENDING
} AudioCue;

typedef enum AudioScene {
    AUDIO_SCENE_NONE = 0,
    AUDIO_SCENE_MENU,
    AUDIO_SCENE_BASE,
    AUDIO_SCENE_FOREST,
    AUDIO_SCENE_SWAMP,
    AUDIO_SCENE_RUINS,
    AUDIO_SCENE_BOSS,
    AUDIO_SCENE_ENDING
} AudioScene;

typedef struct OptionalSound {
    Sound sound;
    bool loaded;
} OptionalSound;

typedef struct OptionalMusic {
    Music music;
    bool loaded;
} OptionalMusic;

typedef struct AudioManager {
    bool ready;
    bool sfxEnabled;
    float masterVolume;
    AudioScene activeScene;
    OptionalSound confirm;
    OptionalSound open;
    OptionalSound warning;
    OptionalSound craft;
    OptionalSound collect;
    OptionalSound repair;
    OptionalSound laser;
    OptionalSound ending;
    OptionalMusic menuLoop;
    OptionalMusic baseLoop;
    OptionalMusic forestLoop;
    OptionalMusic swampLoop;
    OptionalMusic ruinsLoop;
    OptionalMusic bossLoop;
    OptionalMusic endingLoop;
} AudioManager;

void Audio_Init(AudioManager *audio);
void Audio_Shutdown(AudioManager *audio);
void Audio_Update(AudioManager *audio);
void Audio_ApplySettings(AudioManager *audio, const GameSettings *settings);
void Audio_SetMasterVolumeSetting(AudioManager *audio, float volume);
void Audio_SetSfxEnabled(AudioManager *audio, bool enabled);
void Audio_SetScene(AudioManager *audio, AudioScene scene);
void Audio_PlayCue(AudioManager *audio, AudioCue cue);

#endif
