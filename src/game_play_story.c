#include "game_play_internal.h"

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

static bool ShouldAutoOpenLogStoryScene(int logIndex) {
    const int purifierRingControlBriefIndex =
        STORY_SCENE_LOG_PURIFIER_RING_CONTROL_BRIEF - STORY_SCENE_LOG_THE_CRASH;

    if (logIndex == purifierRingControlBriefIndex) {
        return false;
    }

    return true;
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
    snapshot->energyRepairLevel = game->tasks.energyRepairLevel;
    snapshot->amplifierUnlocked = game->tasks.amplifierUnlocked;
    snapshot->westW4Started = game->tasks.westW4Started;
    snapshot->westW4Completed = game->tasks.westW4Completed;
    snapshot->westW5Completed = game->tasks.westW5Completed;
    snapshot->southS4Started = game->tasks.southS4Started;
    snapshot->southS4Completed = game->tasks.southS4Completed;
    snapshot->southS5Completed = game->tasks.southS5Completed;
    snapshot->monolithPuzzleActive = game->tasks.monolithPuzzle.active;
    snapshot->monolithSolved = game->tasks.monolithPuzzle.solved;
    snapshot->endingArchiveReviewed = game->tasks.endingArchiveReviewed;
    snapshot->bossDefeated = game->tasks.bossDefeated;
    snapshot->signalTowerActivated = game->tasks.signalTowerActivated;
    snapshot->ending = game->tasks.ending;
    snapshot->crossX1Ready = IsCrossX1Ready(&game->tasks);
    snapshot->crossX2Ready = IsCrossX2Ready(&game->tasks);
    snapshot->crossX3Ready = IsCrossX3Ready(&game->tasks);
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
            if (logIndex < STORY_LOG_SCENE_COUNT && ShouldAutoOpenLogStoryScene(logIndex)) {
                Game_OpenStoryScene(game, (StoryScene)(STORY_SCENE_LOG_THE_CRASH + logIndex));
            }
            return;
        }
    }

    if (before->oxygenRepairLevel < 1 && game->tasks.oxygenRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_OXYGEN_PATCH);
        return;
    }
    if (before->oxygenRepairLevel < 2 && game->tasks.oxygenRepairLevel >= 2) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_OXYGEN_RESTORED);
        return;
    }
    if (!before->communicatorUnlocked && game->tasks.communicatorUnlocked) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_SYNC);
        return;
    }
    if (!before->swampOuterUnlocked && Map_IsSwampOuterUnlocked(&game->map)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_AIRLOCK_OPEN);
        return;
    }
    if (before->commRepairLevel < 1 && game->tasks.commRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_COMM_RELAY);
        return;
    }
    if (!before->crashClueFound && game->tasks.crashClueFound) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_CRASH_CLUE);
        return;
    }
    if (before->energyRepairLevel < 1 && game->tasks.energyRepairLevel >= 1) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ENERGY_RESTORED);
        return;
    }
    if (!before->amplifierUnlocked && game->tasks.amplifierUnlocked) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_ANALYSIS);
        return;
    }
    if (!before->westW4Started && game->tasks.westW4Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ECHO_BASIN_LOCK);
        return;
    }
    if (!before->westW4Completed && game->tasks.westW4Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_ROUTE_REWRITE);
        return;
    }
    if (!before->westW5Completed && game->tasks.westW5Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LAST_CAMP_ARCHIVE);
        return;
    }
    if (!before->southS4Started && game->tasks.southS4Started) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_PURIFIER_RING_BOOT);
        return;
    }
    if (!before->southS4Completed && game->tasks.southS4Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_GLOBAL_RISK_DROP);
        return;
    }
    if (!before->southS5Completed && game->tasks.southS5Completed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_ROOT_VAULT_CORE);
        return;
    }
    if (!before->crossX1Ready && IsCrossX1Ready(&game->tasks)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_TRACE_CORRELATION);
        return;
    }
    if (!before->crossX2Ready && IsCrossX2Ready(&game->tasks)) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_LOXI_SYNC_REWRITE);
        return;
    }
    if (!before->endingArchiveReviewed && game->tasks.endingArchiveReviewed) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_FINAL_STANCE);
        return;
    }
    if (!before->monolithPuzzleActive && game->tasks.monolithPuzzle.active) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_MONOLITH_AWAKEN);
        return;
    }
    if (!before->monolithSolved && game->tasks.monolithPuzzle.solved) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_MONOLITH_SOLVED);
        return;
    }
    if (!before->bossDefeated && game->tasks.bossDefeated) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_BOSS_FALL);
        return;
    }
    if (!before->signalTowerActivated && game->tasks.signalTowerActivated && game->tasks.ending == ENDING_HEROIC) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SIGNAL_TOWER_HEROIC);
        return;
    }
    if (!before->signalTowerActivated && game->tasks.signalTowerActivated && game->tasks.ending == ENDING_PEACEFUL) {
        Game_OpenStoryScene(game, STORY_SCENE_MAIN_SIGNAL_TOWER_PEACEFUL);
    }
}
