#ifndef GAME_PLAY_INTERNAL_H
#define GAME_PLAY_INTERNAL_H

#include "game_manager_internal.h"

/* Internal gameplay-loop helpers shared by input routing and story trigger slices. */

typedef struct StoryTriggerSnapshot {
    int oxygenRepairLevel;
    bool communicatorUnlocked;
    bool swampOuterUnlocked;
    int commRepairLevel;
    bool crashClueFound;
    bool energyCoreRecovered;
    int energyRepairLevel;
    bool amplifierUnlocked;
    bool westW1Started;
    bool westW2Started;
    bool westW3Started;
    bool westW4Started;
    bool westW5Started;
    bool southS1Started;
    bool southS2Started;
    bool southS3Started;
    bool westW4Completed;
    bool westW5Completed;
    bool southS4Started;
    bool southS4Completed;
    bool southS5Completed;
    bool crossX3Ready;
    bool monolithPuzzleActive;
    bool monolithSolved;
    bool endingArchiveReviewed;
    GameEnding selectedEndingRoute;
    bool bossDefeated;
    bool signalTowerActivated;
    GameEnding ending;
    bool crossX1Ready;
    bool crossX2Ready;
    bool logCollected[MAX_LOGS];
} StoryTriggerSnapshot;

SCL_EXTERN_C_BEGIN

void GamePlay_CaptureStoryTriggerSnapshot(const Game *game, StoryTriggerSnapshot *snapshot);
void GamePlay_TryOpenStorySceneFromSnapshot(Game *game, const StoryTriggerSnapshot *before);
void GamePlay_UpdateMovement(Game *game);
bool GamePlay_HandleImmediateInput(Game *game, char *actionMessage, size_t messageSize);

SCL_EXTERN_C_END

#endif
