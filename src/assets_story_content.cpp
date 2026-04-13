#include "assets_internal.h"

void AssetsInternal_LoadNarrativeAssets(AssetBundle *assets) {
    const char *introSlidePaths[INTRO_CUTSCENE_SLIDE_COUNT] = {
        "resources/images/cutscenes/intro_01_signal.png",
        "resources/images/cutscenes/intro_02_crash.png",
        "resources/images/cutscenes/intro_03_wakeup.png",
        "resources/images/cutscenes/intro_04_planet.png",
        "resources/images/cutscenes/intro_05_loxi.png"
    };
    const char *storyMainPaths[STORY_MAIN_SCENE_COUNT] = {
        "resources/images/story/main/main_01_oxygen_patch.png",
        "resources/images/story/main/main_02_oxygen_restored.png",
        "resources/images/story/main/main_03_loxi_sync.png",
        "resources/images/story/main/main_04_airlock_open.png",
        "resources/images/story/main/main_05_comm_relay.png",
        "resources/images/story/main/main_06_crash_clue.png",
        "resources/images/story/main/main_07_energy_restored.png",
        "resources/images/story/main/main_08_loxi_analysis.png",
        "resources/images/story/main/main_09_monolith_awaken.png",
        "resources/images/story/main/main_10_monolith_solved.png",
        "resources/images/story/main/main_11_boss_fall.png",
        "resources/images/story/main/main_12_signal_tower_heroic.png",
        "resources/images/story/main/main_13_signal_tower_peaceful.png"
    };
    const char *storyLogPaths[STORY_LOG_SCENE_COUNT] = {
        "resources/images/story/logs/log_01_the_crash.png",
        "resources/images/story/logs/log_02_missing_crew.png",
        "resources/images/story/logs/log_03_alien_ecology.png"
    };
    const char *storyEndingPaths[STORY_ENDING_SCENE_COUNT] = {
        "resources/images/story/endings/ending_01_settlement.png",
        "resources/images/story/endings/ending_02_failure.png",
        "resources/images/story/endings/ending_03_heroic_rescue.png",
        "resources/images/story/endings/ending_04_peaceful_rescue.png"
    };
    int introIndex;
    int storyIndex;

    for (introIndex = 0; introIndex < INTRO_CUTSCENE_SLIDE_COUNT; introIndex++) {
        assets->introSlides[introIndex] = AssetsInternal_LoadOptionalTextureFittedPreserveFrame(introSlidePaths[introIndex], 1920, 1080);
    }
    for (storyIndex = 0; storyIndex < STORY_MAIN_SCENE_COUNT; storyIndex++) {
        assets->storyMainScenes[storyIndex] = AssetsInternal_LoadOptionalTextureFittedPreserveFrame(storyMainPaths[storyIndex], 1920, 1080);
    }
    for (storyIndex = 0; storyIndex < STORY_LOG_SCENE_COUNT; storyIndex++) {
        assets->storyLogScenes[storyIndex] = AssetsInternal_LoadOptionalTextureFittedPreserveFrame(storyLogPaths[storyIndex], 1920, 1080);
    }
    for (storyIndex = 0; storyIndex < STORY_ENDING_SCENE_COUNT; storyIndex++) {
        assets->storyEndingScenes[storyIndex] = AssetsInternal_LoadOptionalTextureFittedPreserveFrame(storyEndingPaths[storyIndex], 1920, 1080);
    }
}

void AssetsInternal_UnloadNarrativeAssets(AssetBundle *assets) {
    int introIndex;
    int storyIndex;

    for (introIndex = 0; introIndex < INTRO_CUTSCENE_SLIDE_COUNT; introIndex++) {
        AssetsInternal_UnloadTextureAsset(&assets->introSlides[introIndex]);
    }
    for (storyIndex = 0; storyIndex < STORY_MAIN_SCENE_COUNT; storyIndex++) {
        AssetsInternal_UnloadTextureAsset(&assets->storyMainScenes[storyIndex]);
    }
    for (storyIndex = 0; storyIndex < STORY_LOG_SCENE_COUNT; storyIndex++) {
        AssetsInternal_UnloadTextureAsset(&assets->storyLogScenes[storyIndex]);
    }
    for (storyIndex = 0; storyIndex < STORY_ENDING_SCENE_COUNT; storyIndex++) {
        AssetsInternal_UnloadTextureAsset(&assets->storyEndingScenes[storyIndex]);
    }
}
