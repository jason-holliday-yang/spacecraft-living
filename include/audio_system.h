#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <stdbool.h>
#include "c_compat.h"
#include "raylib.h"
#include "save_system.h"

/* Public C ABI for optional cue/music playback and scene-based audio state. */

SCL_EXTERN_C_BEGIN

typedef enum AudioCue {
    AUDIO_CUE_CONFIRM = 0,
    AUDIO_CUE_OPEN,
    AUDIO_CUE_CLOSE,
    AUDIO_CUE_WARNING,
    AUDIO_CUE_CRAFT,
    AUDIO_CUE_COLLECT,
    AUDIO_CUE_LOG,
    AUDIO_CUE_REPAIR,
    AUDIO_CUE_HURT,
    AUDIO_CUE_MELEE,
    AUDIO_CUE_MONSTER,
    AUDIO_CUE_MONOLITH,
    AUDIO_CUE_STEP_FOREST,
    AUDIO_CUE_STEP_METAL,
    AUDIO_CUE_STEP_SWAMP,
    AUDIO_CUE_LASER,
    AUDIO_CUE_ENDING,
    AUDIO_CUE_ENDING_PEACEFUL,
    AUDIO_CUE_ENDING_SETTLEMENT
} AudioCue;

typedef enum AudioScene {
    AUDIO_SCENE_NONE = 0,
    AUDIO_SCENE_MENU,
    AUDIO_SCENE_BASE,
    AUDIO_SCENE_FOREST,
    AUDIO_SCENE_FOREST_ROUTE,
    AUDIO_SCENE_SWAMP,
    AUDIO_SCENE_SWAMP_DEEP,
    AUDIO_SCENE_RUINS,
    AUDIO_SCENE_RUINS_FACILITY,
    AUDIO_SCENE_BOSS,
    AUDIO_SCENE_BOSS_ARENA,
    AUDIO_SCENE_ENDING
} AudioScene;

typedef enum AudioMusicStage {
    AUDIO_MUSIC_NONE = 0,
    AUDIO_MUSIC_MENU,
    AUDIO_MUSIC_STAGE_1_WAKE,
    AUDIO_MUSIC_STAGE_2_FIRST_STEPS,
    AUDIO_MUSIC_STAGE_3_WILD,
    AUDIO_MUSIC_STAGE_4_RISK,
    AUDIO_MUSIC_STAGE_5_POWER,
    AUDIO_MUSIC_STAGE_6_RELICS,
    AUDIO_MUSIC_STAGE_7_CHOICE,
    AUDIO_MUSIC_ROUTE_HERO,
    AUDIO_MUSIC_ROUTE_PEACEFUL,
    AUDIO_MUSIC_ROUTE_SETTLEMENT,
    AUDIO_MUSIC_BOSS,
    AUDIO_MUSIC_ENDING
} AudioMusicStage;

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
    bool sceneTransitionActive;
    bool activeSceneStopped;
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    float sceneTransitionTimer;
    int activeSceneVariant;
    int pendingSceneVariant;
    int activeMusicVariant;
    int pendingMusicVariant;
    AudioScene activeScene;
    AudioScene requestedScene;
    AudioScene pendingScene;
    AudioMusicStage activeMusicStage;
    AudioMusicStage requestedMusicStage;
    AudioMusicStage pendingMusicStage;
    OptionalSound confirm;
    OptionalSound open;
    OptionalSound close;
    OptionalSound warning;
    OptionalSound craft;
    OptionalSound collect;
    OptionalSound log;
    OptionalSound repair;
    OptionalSound hurt;
    OptionalSound melee;
    OptionalSound monster;
    OptionalSound monolith;
    OptionalSound stepForest;
    OptionalSound stepMetal;
    OptionalSound stepSwamp;
    OptionalSound laser;
    OptionalSound ending;
    OptionalSound endingPeaceful;
    OptionalSound endingSettlement;
    OptionalMusic menuLoop;
    OptionalMusic menuLoopAlt;
    OptionalMusic baseLoop;
    OptionalMusic baseLoopAlt;
    OptionalMusic stage3WildLoop;
    OptionalMusic stage3WildLoopAlt;
    OptionalMusic stage4RiskLoop;
    OptionalMusic stage4RiskLoopAlt;
    OptionalMusic stage5PowerLoop;
    OptionalMusic stage5PowerLoopAlt;
    OptionalMusic stage7ChoiceLoop;
    OptionalMusic stage7ChoiceLoopAlt;
    OptionalMusic forestLoop;
    OptionalMusic forestRouteLoop;
    OptionalMusic swampLoop;
    OptionalMusic swampDeepLoop;
    OptionalMusic ruinsLoop;
    OptionalMusic ruinsFacilityLoop;
    OptionalMusic bossLoop;
    OptionalMusic bossArenaLoop;
    OptionalMusic settlementLoop;
    OptionalMusic settlementLoopAlt;
    OptionalMusic endingLoop;
    OptionalMusic endingLoopAlt;
} AudioManager;

void Audio_Init(AudioManager *audio);
void Audio_Shutdown(AudioManager *audio);
void Audio_Update(AudioManager *audio);
void Audio_ApplySettings(AudioManager *audio, const GameSettings *settings);
void Audio_SetMasterVolumeSetting(AudioManager *audio, float volume);
void Audio_SetMusicVolumeSetting(AudioManager *audio, float volume);
void Audio_SetSfxVolumeSetting(AudioManager *audio, float volume);
void Audio_SetSfxEnabled(AudioManager *audio, bool enabled);
void Audio_SetMusicStage(AudioManager *audio, AudioMusicStage stage);
void Audio_SetScene(AudioManager *audio, AudioScene scene);
void Audio_PlayCue(AudioManager *audio, AudioCue cue);

SCL_EXTERN_C_END

#endif
