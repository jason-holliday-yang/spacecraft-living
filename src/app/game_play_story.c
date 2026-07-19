#include "game_play_internal.h"

static bool HasEnergyCoreInInventory(const Game *game) {
    return game != NULL && Player_HasResources(&game->runtime.player, RESOURCE_ENERGY_CORE, 1);
}

static bool IsCrossX1Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW3Completed
        && tasks->southS2Completed;
}

static bool IsCrossX2Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW4Completed
        && tasks->southS4Completed;
}

static bool IsCrossX3Ready(const TaskSystem *tasks) {
    return tasks != NULL
        && tasks->westW5Completed
        && tasks->southS5Completed;
}

static StoryScene GetAutoOpenedSceneForCollectedLog(int logIndex) {
    const int blackBoxResidueIndex =
        STORY_SCENE_LOG_BLACK_BOX_RESIDUE - STORY_SCENE_LOG_THE_CRASH;
    const int purifierRingControlBriefIndex =
        STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF - STORY_SCENE_LOG_THE_CRASH;

    if (logIndex == blackBoxResidueIndex) {
        return STORY_SCENE_MAIN_EAST_WRECK_CONFIRMATION;
    }
    if (logIndex == purifierRingControlBriefIndex) {
        return STORY_SCENE_MAIN_MONOLITH_TRUE_ROLE;
    }
    if (logIndex >= 0 && logIndex < STORY_LOG_SCENE_COUNT) {
        return (StoryScene)(STORY_SCENE_LOG_THE_CRASH + logIndex);
    }
    return STORY_SCENE_NONE;
}

void GamePlay_CaptureStoryTriggerSnapshot(const Game *game, StoryTriggerSnapshot *snapshot) {
    int index;

    if (game == NULL || snapshot == NULL) {
        return;
    }

    snapshot->oxygenRepairLevel = game->runtime.tasks.oxygenRepairLevel;
    snapshot->communicatorUnlocked = game->runtime.tasks.communicatorUnlocked;
    snapshot->swampOuterUnlocked = Map_IsSwampOuterUnlocked(&game->runtime.map);
    snapshot->commRepairLevel = game->runtime.tasks.commRepairLevel;
    snapshot->crashClueFound = game->runtime.tasks.crashClueFound;
    snapshot->energyCoreRecovered = HasEnergyCoreInInventory(game);
    snapshot->energyRepairLevel = game->runtime.tasks.energyRepairLevel;
    snapshot->amplifierUnlocked = game->runtime.tasks.amplifierUnlocked;
    snapshot->westW1Started = game->runtime.tasks.westW1Started;
    snapshot->westW2Started = game->runtime.tasks.westW2Started;
    snapshot->westW3Started = game->runtime.tasks.westW3Started;
    snapshot->westW4Started = game->runtime.tasks.westW4Started;
    snapshot->westW5Started = game->runtime.tasks.westW5Started;
    snapshot->southS1Started = game->runtime.tasks.southS1Started;
    snapshot->southS2Started = game->runtime.tasks.southS2Started;
    snapshot->southS3Started = game->runtime.tasks.southS3Started;
    snapshot->westW4Completed = game->runtime.tasks.westW4Completed;
    snapshot->westW5Completed = game->runtime.tasks.westW5Completed;
    snapshot->southS4Started = game->runtime.tasks.southS4Started;
    snapshot->southS4Completed = game->runtime.tasks.southS4Completed;
    snapshot->southS5Completed = game->runtime.tasks.southS5Completed;
    snapshot->crossX3Ready = IsCrossX3Ready(&game->runtime.tasks);
    snapshot->monolithPuzzleActive = game->runtime.tasks.monolithPuzzle.active;
    snapshot->monolithSolved = game->runtime.tasks.monolithPuzzle.solved;
    snapshot->endingArchiveReviewed = game->runtime.tasks.endingArchiveReviewed;
    snapshot->selectedEndingRoute = game->runtime.tasks.selectedEndingRoute;
    snapshot->bossDefeated = game->runtime.tasks.bossDefeated;
    snapshot->signalTowerActivated = game->runtime.tasks.signalTowerActivated;
    snapshot->ending = game->runtime.tasks.ending;
    snapshot->crossX1Ready = IsCrossX1Ready(&game->runtime.tasks);
    snapshot->crossX2Ready = IsCrossX2Ready(&game->runtime.tasks);
    for (index = 0; index < MAX_LOGS; index++) {
        snapshot->logCollected[index] = index < game->runtime.tasks.logCount ? game->runtime.tasks.logs[index].collected : false;
    }
}

void GamePlay_TryOpenStorySceneFromSnapshot(Game *game, const StoryTriggerSnapshot *before) {
    int logIndex;

    if (game == NULL || before == NULL || game->flow.storySceneOpen) {
        return;
    }

    for (logIndex = 0; logIndex < game->runtime.tasks.logCount && logIndex < MAX_LOGS; logIndex++) {
        if (!before->logCollected[logIndex] && game->runtime.tasks.logs[logIndex].collected) {
            StoryScene scene = GetAutoOpenedSceneForCollectedLog(logIndex);

            if (scene != STORY_SCENE_NONE) {
                Game_OpenStoryScene(game, scene);
            }
            return;
        }
    }

    if (before->oxygenRepairLevel < 1 && game->runtime.tasks.oxygenRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY);
        return;
    }
    if (before->oxygenRepairLevel < 2 && game->runtime.tasks.oxygenRepairLevel >= 2) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_OXYGEN_CYCLE_RESTORED);
        return;
    }
    if (!before->communicatorUnlocked && game->runtime.tasks.communicatorUnlocked) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_FULL_SYNC);
        return;
    }
    if (!before->swampOuterUnlocked && Map_IsSwampOuterUnlocked(&game->runtime.map)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_AIRLOCK_OPENING);
        return;
    }
    if (before->commRepairLevel < 1 && game->runtime.tasks.commRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SIGNAL_ANSWERS_BACK);
        return;
    }
    if (!before->crashClueFound && game->runtime.tasks.crashClueFound) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_CRASH_NOT_ACCIDENT);
        return;
    }
    if (!before->energyCoreRecovered && HasEnergyCoreInInventory(game)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_BASIN_ACCESS_QUALIFICATION);
        return;
    }
    if (before->energyRepairLevel < 1 && game->runtime.tasks.energyRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_DEEP_SCAN_ONLINE);
        return;
    }
    if (!before->amplifierUnlocked && game->runtime.tasks.amplifierUnlocked) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_RELIC_PATTERN_DECODE);
        return;
    }
    if (!before->westW1Started && game->runtime.tasks.westW1Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_WEST_ROUTE_CONFIRMED);
        return;
    }
    if (!before->westW2Started && game->runtime.tasks.westW2Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SURVEY_BREAK_RELAY);
        return;
    }
    if (!before->westW3Started && game->runtime.tasks.westW3Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_CANOPY_HANDOFF);
        return;
    }
    if (!before->westW4Completed && game->runtime.tasks.westW4Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ECHO_BASIN_RECONSTRUCTION);
        return;
    }
    if (!before->westW5Completed && game->runtime.tasks.westW5Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LAST_CAMP_POSITIONS);
        return;
    }
    if (!before->southS1Started && game->runtime.tasks.southS1Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SOUTH_FACILITY_WAKES);
        return;
    }
    if (!before->southS2Started && game->runtime.tasks.southS2Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_VENT_NETWORK_CALIBRATED);
        return;
    }
    if (!before->southS3Started && game->runtime.tasks.southS3Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SERVICE_SHAFT_BACKBONE);
        return;
    }
    if (!before->southS4Started && game->runtime.tasks.southS4Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_PURIFIER_RING_SEQUENCE);
        return;
    }
    if (!before->southS5Completed && game->runtime.tasks.southS5Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ROOT_VAULT_TRUTH);
        return;
    }
    if (!before->crossX1Ready && IsCrossX1Ready(&game->runtime.tasks)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_WEST_SOUTH_CORRELATION);
        return;
    }
    if (!before->crossX2Ready && IsCrossX2Ready(&game->runtime.tasks)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_CONCLUSION_REWRITE);
        return;
    }
    if (!before->monolithPuzzleActive && game->runtime.tasks.monolithPuzzle.active) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_NORTH_ROUTE_COMMITMENT);
        return;
    }
    if (!before->monolithSolved && game->runtime.tasks.monolithPuzzle.solved) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_TOWER_NOT_BUTTON);
        return;
    }
    if (!before->endingArchiveReviewed && game->runtime.tasks.endingArchiveReviewed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_THREE_COSTS_REVEALED);
        return;
    }
    if (before->selectedEndingRoute == ENDING_NONE
        && game->runtime.tasks.selectedEndingRoute == ENDING_SETTLEMENT) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT);
        return;
    }
    if (before->selectedEndingRoute == ENDING_NONE
        && (game->runtime.tasks.selectedEndingRoute == ENDING_HEROIC
            || game->runtime.tasks.selectedEndingRoute == ENDING_PEACEFUL)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ACTION_DECLARATION);
        return;
    }
    if (!before->signalTowerActivated && game->runtime.tasks.signalTowerActivated && game->runtime.tasks.ending == ENDING_HEROIC) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_HEROIC_ROUTE_COMMITMENT);
        return;
    }
    if (!before->signalTowerActivated && game->runtime.tasks.signalTowerActivated && game->runtime.tasks.ending == ENDING_PEACEFUL) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_PEACEFUL_ROUTE_COMMITMENT);
    }
}
