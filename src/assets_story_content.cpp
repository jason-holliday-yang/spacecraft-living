#include "assets_internal.h"

void AssetsInternal_LoadNarrativeAssets(AssetBundle *assets) {
    const char *introSlidePaths[INTRO_CUTSCENE_SLIDE_COUNT] = {
        "resources/images/cutscenes/intro_01_unmarked_call.png",
        "resources/images/cutscenes/intro_02_orbit_collapse.png",
        "resources/images/cutscenes/intro_03_barely_alive.png",
        "resources/images/cutscenes/intro_04_world_watches_back.png",
        "resources/images/cutscenes/intro_05_stay_alive_first.png"
    };
    const char *storyMainPaths[STORY_MAIN_SCENE_COUNT] = {
        "resources/images/story/main/main_01_air_for_one_more_day.png",
        "resources/images/story/main/main_02_breathing_room_restored.png",
        "resources/images/story/main/main_03_voice_in_the_wreck.png",
        "resources/images/story/main/main_04_world_outside_opens.png",
        "resources/images/story/main/main_05_signal_answers_back.png",
        "resources/images/story/main/main_06_not_an_accident.png",
        "resources/images/story/main/main_07_base_wakes_up.png",
        "resources/images/story/main/main_08_pattern_is_alien.png",
        "resources/images/story/main/main_09_ruins_notice_you.png",
        "resources/images/story/main/main_10_sequence_holds.png",
        "resources/images/story/main/main_11_last_barrier_breaks.png",
        "resources/images/story/main/main_12_beacon_through_force.png",
        "resources/images/story/main/main_13_beacon_through_understanding.png",
        "resources/images/story/main/main_14_echo_basin_lock.png",
        "resources/images/story/main/main_15_loxi_route_rewrite.png",
        "resources/images/story/main/main_16_last_camp_archive.png",
        "resources/images/story/main/main_17_purifier_ring_boot.png",
        "resources/images/story/main/main_18_global_risk_drop.png",
        "resources/images/story/main/main_19_root_vault_core.png",
        "resources/images/story/main/main_20_trace_correlation.png",
        "resources/images/story/main/main_21_loxi_sync_rewrite.png",
        "resources/images/story/main/main_22_final_stance.png"
    };
    const char *storyLogPaths[STORY_LOG_SCENE_COUNT] = {
        "resources/images/story/logs/log_01_impact_protocol.png",
        "resources/images/story/logs/log_02_split_roster.png",
        "resources/images/story/logs/log_03_pattern_not_wilderness.png",
        "resources/images/story/logs/log_04_west_signal_fragment_01.png",
        "resources/images/story/logs/log_05_survey_break_anchor_notes.png",
        "resources/images/story/logs/log_06_canopy_handoff_record.png",
        "resources/images/story/logs/log_07_echo_basin_topology_sketch.png",
        "resources/images/story/logs/log_08_last_camp_testament.png",
        "resources/images/story/logs/log_09_crash_recorder_black_box_residue.png",
        "resources/images/story/logs/log_10_purifier_outage_memo.png",
        "resources/images/story/logs/log_11_vent_calibration_handover.png",
        "resources/images/story/logs/log_12_service_shaft_sync_record.png",
        "resources/images/story/logs/log_13_purifier_ring_control_brief.png",
        "resources/images/story/logs/log_14_root_vault_core_dossier.png"
    };
    const char *storyEndingPaths[STORY_ENDING_SCENE_COUNT] = {
        "resources/images/story/endings/ending_01_alien_settlement.png",
        "resources/images/story/endings/ending_02_failed_survival.png",
        "resources/images/story/endings/ending_03_heroic_rescue.png",
        "resources/images/story/endings/ending_04_peaceful_rescue.png",
        "resources/images/story/endings/ending_05_heroic_with_records.png",
        "resources/images/story/endings/ending_06_peaceful_with_repair.png",
        "resources/images/story/endings/ending_07_settlement_with_legacy.png"
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
