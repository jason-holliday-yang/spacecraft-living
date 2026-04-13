#include "game_play_internal.h"

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
    snapshot->monolithPuzzleActive = game->tasks.monolithPuzzle.active;
    snapshot->monolithSolved = game->tasks.monolithPuzzle.solved;
    snapshot->bossDefeated = game->tasks.bossDefeated;
    snapshot->signalTowerActivated = game->tasks.signalTowerActivated;
    snapshot->ending = game->tasks.ending;
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
            if (logIndex == 0) {
                Game_OpenStoryScene(game, STORY_SCENE_LOG_THE_CRASH);
            } else if (logIndex == 1) {
                Game_OpenStoryScene(game, STORY_SCENE_LOG_MISSING_CREW);
            } else if (logIndex == 2) {
                Game_OpenStoryScene(game, STORY_SCENE_LOG_ALIEN_ECOLOGY);
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
