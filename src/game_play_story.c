#include "game_play_internal.h"

static bool HasEnergyCoreInInventory(const Game *game) {
    return game != NULL && Player_HasResources(&game->player, RESOURCE_ENERGY_CORE, 1);
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

    snapshot->oxygenRepairLevel = game->tasks.oxygenRepairLevel;
    snapshot->communicatorUnlocked = game->tasks.communicatorUnlocked;
    snapshot->swampOuterUnlocked = Map_IsSwampOuterUnlocked(&game->map);
    snapshot->commRepairLevel = game->tasks.commRepairLevel;
    snapshot->crashClueFound = game->tasks.crashClueFound;
    snapshot->energyCoreRecovered = HasEnergyCoreInInventory(game);
    snapshot->energyRepairLevel = game->tasks.energyRepairLevel;
    snapshot->amplifierUnlocked = game->tasks.amplifierUnlocked;
    snapshot->westW1Started = game->tasks.westW1Started;
    snapshot->westW2Started = game->tasks.westW2Started;
    snapshot->westW3Started = game->tasks.westW3Started;
    snapshot->westW4Started = game->tasks.westW4Started;
    snapshot->westW5Started = game->tasks.westW5Started;
    snapshot->southS1Started = game->tasks.southS1Started;
    snapshot->southS2Started = game->tasks.southS2Started;
    snapshot->southS3Started = game->tasks.southS3Started;
    snapshot->westW4Completed = game->tasks.westW4Completed;
    snapshot->westW5Completed = game->tasks.westW5Completed;
    snapshot->southS4Started = game->tasks.southS4Started;
    snapshot->southS4Completed = game->tasks.southS4Completed;
    snapshot->southS5Completed = game->tasks.southS5Completed;
    snapshot->crossX3Ready = IsCrossX3Ready(&game->tasks);
    snapshot->monolithPuzzleActive = game->tasks.monolithPuzzle.active;
    snapshot->monolithSolved = game->tasks.monolithPuzzle.solved;
    snapshot->endingArchiveReviewed = game->tasks.endingArchiveReviewed;
    snapshot->selectedEndingRoute = game->tasks.selectedEndingRoute;
    snapshot->bossDefeated = game->tasks.bossDefeated;
    snapshot->signalTowerActivated = game->tasks.signalTowerActivated;
    snapshot->ending = game->tasks.ending;
    snapshot->crossX1Ready = IsCrossX1Ready(&game->tasks);
    snapshot->crossX2Ready = IsCrossX2Ready(&game->tasks);
    for (index = 0; index < MAX_LOGS; index++) {
        snapshot->logCollected[index] = index < game->tasks.logCount ? game->tasks.logs[index].collected : false;
    }
}

void GamePlay_TryOpenStorySceneFromSnapshot(Game *game, const StoryTriggerSnapshot *before) {
    int logIndex;

    if (game == NULL || before == NULL || game->storySceneOpen) {
        return;
    }

    for (logIndex = 0; logIndex < game->tasks.logCount && logIndex < MAX_LOGS; logIndex++) {
        if (!before->logCollected[logIndex] && game->tasks.logs[logIndex].collected) {
            StoryScene scene = GetAutoOpenedSceneForCollectedLog(logIndex);

            if (scene != STORY_SCENE_NONE) {
                Game_OpenStoryScene(game, scene);
            }
            return;
        }
    }

    if (before->oxygenRepairLevel < 1 && game->tasks.oxygenRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY);
        return;
    }
    if (before->oxygenRepairLevel < 2 && game->tasks.oxygenRepairLevel >= 2) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_OXYGEN_CYCLE_RESTORED);
        return;
    }
    if (!before->communicatorUnlocked && game->tasks.communicatorUnlocked) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_FULL_SYNC);
        return;
    }
    if (!before->swampOuterUnlocked && Map_IsSwampOuterUnlocked(&game->map)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_AIRLOCK_OPENING);
        return;
    }
    if (before->commRepairLevel < 1 && game->tasks.commRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SIGNAL_ANSWERS_BACK);
        return;
    }
    if (!before->crashClueFound && game->tasks.crashClueFound) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_CRASH_NOT_ACCIDENT);
        return;
    }
    if (!before->energyCoreRecovered && HasEnergyCoreInInventory(game)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_BASIN_ACCESS_QUALIFICATION);
        return;
    }
    if (before->energyRepairLevel < 1 && game->tasks.energyRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_DEEP_SCAN_ONLINE);
        return;
    }
    if (!before->amplifierUnlocked && game->tasks.amplifierUnlocked) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_RELIC_PATTERN_DECODE);
        return;
    }
    if (!before->westW1Started && game->tasks.westW1Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_WEST_ROUTE_CONFIRMED);
        return;
    }
    if (!before->westW2Started && game->tasks.westW2Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SURVEY_BREAK_RELAY);
        return;
    }
    if (!before->westW3Started && game->tasks.westW3Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_CANOPY_HANDOFF);
        return;
    }
    if (!before->westW4Completed && game->tasks.westW4Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ECHO_BASIN_RECONSTRUCTION);
        return;
    }
    if (!before->westW5Completed && game->tasks.westW5Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LAST_CAMP_POSITIONS);
        return;
    }
    if (!before->southS1Started && game->tasks.southS1Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SOUTH_FACILITY_WAKES);
        return;
    }
    if (!before->southS2Started && game->tasks.southS2Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_VENT_NETWORK_CALIBRATED);
        return;
    }
    if (!before->southS3Started && game->tasks.southS3Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SERVICE_SHAFT_BACKBONE);
        return;
    }
    if (!before->southS4Started && game->tasks.southS4Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_PURIFIER_RING_SEQUENCE);
        return;
    }
    if (!before->southS5Completed && game->tasks.southS5Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ROOT_VAULT_TRUTH);
        return;
    }
    if (!before->crossX1Ready && IsCrossX1Ready(&game->tasks)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_WEST_SOUTH_CORRELATION);
        return;
    }
    if (!before->crossX2Ready && IsCrossX2Ready(&game->tasks)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_CONCLUSION_REWRITE);
        return;
    }
    if (!before->monolithPuzzleActive && game->tasks.monolithPuzzle.active) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_NORTH_ROUTE_COMMITMENT);
        return;
    }
    if (!before->monolithSolved && game->tasks.monolithPuzzle.solved) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_TOWER_NOT_BUTTON);
        return;
    }
    if (!before->endingArchiveReviewed && game->tasks.endingArchiveReviewed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_THREE_COSTS_REVEALED);
        return;
    }
    if (before->selectedEndingRoute == ENDING_NONE
        && game->tasks.selectedEndingRoute == ENDING_SETTLEMENT) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SETTLEMENT_ROUTE_COMMITMENT);
        return;
    }
    if (before->selectedEndingRoute == ENDING_NONE
        && (game->tasks.selectedEndingRoute == ENDING_HEROIC
            || game->tasks.selectedEndingRoute == ENDING_PEACEFUL)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ACTION_DECLARATION);
        return;
    }
    if (!before->signalTowerActivated && game->tasks.signalTowerActivated && game->tasks.ending == ENDING_HEROIC) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_HEROIC_ROUTE_COMMITMENT);
        return;
    }
    if (!before->signalTowerActivated && game->tasks.signalTowerActivated && game->tasks.ending == ENDING_PEACEFUL) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_PEACEFUL_ROUTE_COMMITMENT);
    }
}
