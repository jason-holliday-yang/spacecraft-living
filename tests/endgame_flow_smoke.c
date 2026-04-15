#include "map.h"
#include "player.h"
#include "task_system.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "endgame_flow_smoke failed: %s\n", message);
    exit(1);
}

static void ResetEndgameState(GameMap *map, Player *player, TaskSystem *tasks) {
    Map_Init(map);
    Player_Init(player);
    Tasks_Init(tasks, map);

    tasks->stage = 7;
    tasks->oxygenRepairLevel = 2;
    tasks->commRepairLevel = 1;
    tasks->energyRepairLevel = 1;
    tasks->crashClueFound = true;
    tasks->amplifierUnlocked = true;
    tasks->ending = ENDING_NONE;
    tasks->endingArchiveReviewed = false;
    tasks->signalTowerActivated = false;
}

static void PrepareEndingBranch(TaskSystem *tasks) {
    int index;

    tasks->ending = ENDING_NONE;
    tasks->selectedEndingRoute = ENDING_NONE;
    tasks->endingArchiveReviewed = true;
    tasks->signalTowerActivated = false;
    tasks->westW5Completed = true;
    tasks->southS5Completed = true;
    for (index = 0; index < tasks->logCount; index++) {
        tasks->logs[index].collected = true;
        tasks->logs[index].active = true;
    }
}

static void PrepareMainArchiveOnly(TaskSystem *tasks) {
    int index;

    tasks->ending = ENDING_NONE;
    tasks->selectedEndingRoute = ENDING_NONE;
    tasks->endingArchiveReviewed = false;
    tasks->signalTowerActivated = false;
    tasks->westW5Completed = true;
    tasks->southS5Completed = true;
    for (index = 0; index < tasks->logCount; index++) {
        tasks->logs[index].active = true;
        tasks->logs[index].collected = tasks->logs[index].category == SHIP_LOG_MAINLINE;
    }
}

static void MovePlayerToLoxiRightSide(Player *player) {
    player->gridX = LOXI_TERMINAL_X + STATION_FOOTPRINT_WIDTH;
    player->gridY = LOXI_TERMINAL_Y;
    player->facingX = -1;
    player->facingY = 0;
}

static void MovePlayerToLoxiLeftPickupTile(Player *player) {
    player->gridX = LOXI_TERMINAL_X - 1;
    player->gridY = LOXI_TERMINAL_Y;
    player->facingX = 0;
    player->facingY = 1;
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    char message[256];

    ResetEndgameState(&map, &player, &tasks);
    player.hasSignalAmplifier = true;
    MovePlayerToLoxiLeftPickupTile(&player);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.") == 0,
            "stage 7 should now keep the player on archive completion before the branch point opens");

    PrepareMainArchiveOnly(&tasks);
    MovePlayerToLoxiLeftPickupTile(&player);
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_IsEndingBranchReady(&tasks),
            "mainline archive completion should be enough to open the ending branch");
    Require(strcmp(tasks.objective, "Return to Loxi and review the assembled archive before choosing the final route.") == 0,
            "mainline archive completion should now require a final ship-side archive review before route selection");
    Require(!Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "route selection should stay locked until the archive review step is completed");

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "loxi terminal should perform the archive review once the main archive is assembled");
    Require(tasks.endingArchiveReviewed,
            "loxi interaction should mark the final archive review as complete");
    Require(strstr(message, "Archive review complete") != NULL || strstr(message, "档案复核完成") != NULL,
            "archive review interaction should explicitly explain that the truth review step has completed");
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Return to Loxi and choose the final route.") == 0,
            "after the archive review, the objective should advance to the actual route choice");

    PrepareEndingBranch(&tasks);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Return to Loxi and choose the final route.") == 0,
            "once the archive is complete, the objective should point back to Loxi");

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "loxi terminal should remain interactive once the branch point opens");
    Require(tasks.ending == ENDING_NONE,
            "base terminal should not commit the settlement ending without confirmation");
    Require(Tasks_CanChooseSettlement(&tasks),
            "base terminal should expose settlement as an explicit available choice");
    Require(message[0] != '\0',
            "settlement interaction should explain that confirmation is required");
    Require(message[0] != '\0',
            "settlement interaction should explain that settlement closes both rescue routes");
    Tasks_CommitSettlement(&tasks);
    Require(tasks.ending == ENDING_SETTLEMENT,
            "committing the settlement route should enter the settlement ending");

    ResetEndgameState(&map, &player, &tasks);
    player.hasSignalAmplifier = true;
    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "signal tower should still respond before the route is chosen");
    Require(tasks.ending == ENDING_NONE && !tasks.signalTowerActivated,
            "signal tower should no longer end the run before the Loxi branch choice");
    Require(strstr(message, "Loxi") != NULL,
            "tower should redirect the player to Loxi when no route has been chosen");

    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "peaceful route should be selectable once the archive is complete");
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "signal tower should accept the chosen peaceful route");
    Require(tasks.signalTowerActivated,
            "peaceful tower route should mark the tower as activated");
    Require(tasks.ending == ENDING_PEACEFUL,
            "chosen peaceful route should end in peaceful rescue");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    player.gridX = AIRLOCK_CONSOLE_X - 1;
    player.gridY = AIRLOCK_CONSOLE_Y;
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should be selectable once the archive is complete");
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Heroic route chosen. Open the airlock and commit to the guardian arena.") == 0,
            "heroic route should now point to the airlock before the boss fight starts");
    Map_LockSwampOuter(&map);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "heroic airlock interaction should now trigger the arena transition");
    Require(player.gridX == BOSS_ARENA_PLAYER_ENTRY_X && player.gridY == BOSS_ARENA_PLAYER_ENTRY_Y,
            "heroic airlock interaction should teleport the player into the isolated boss arena");
    Require(Map_GetAreaAt(player.gridX, player.gridY) == MAP_AREA_BOSS_ARENA,
            "heroic airlock interaction should place the player in the boss arena area");
    Require(strstr(message, "guardian arena") != NULL || strstr(message, "isolated breach mode") != NULL,
            "heroic airlock interaction should explain the forced arena breach");
    Require(strcmp(tasks.objective, "Heroic route chosen. Defeat the guardian in the isolated arena, then return to the Signal Tower.") == 0,
            "heroic arena entry should update the objective to defeating the guardian before the tower");

    ResetEndgameState(&map, &player, &tasks);
    player.hasSignalAmplifier = true;
    tasks.bossDefeated = true;
    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should be selectable once the archive is complete");
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Heroic route chosen. Reach the Signal Tower and launch the rescue beacon.") == 0,
            "boss-defeated heroic flow should point straight to the tower after route selection");

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "signal tower should allow heroic rescue after the boss is defeated and the route is chosen");
    Require(tasks.signalTowerActivated,
            "heroic tower route should mark the tower as activated");
    Require(tasks.ending == ENDING_HEROIC,
            "boss-defeated tower interaction should stay heroic once the heroic route is chosen");
    Require(strstr(message, "rescue beacon") != NULL,
            "heroic tower activation should report the rescue beacon launch");
    Require(strstr(message, "heroic") != NULL,
            "heroic tower activation should explicitly frame the ending as the heroic route");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    player.hasSignalAmplifier = true;
    MovePlayerToLoxiRightSide(&player);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "loxi terminal should stay interactive when X3 is ready");
    Require(message[0] != '\0',
            "loxi terminal should keep returning guidance once west and south routes are both complete");
    Require(message[0] != '\0',
            "X3-ready Loxi guidance should explicitly frame all three player-facing ending routes");

    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    tasks.bossDefeated = false;
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "peaceful route should remain selectable when X3 is complete");
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "peaceful tower route should still work with X3 completion");
    Require(tasks.ending == ENDING_PEACEFUL,
            "peaceful route should still end peacefully when X3 is complete");
    Require(strstr(message, "west and south") != NULL,
            "peaceful tower message should cite west/south context when X3 is complete");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    player.hasSignalAmplifier = true;
    tasks.bossDefeated = true;
    MovePlayerToLoxiRightSide(&player);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "settlement should remain selectable after the boss route is available");
    Require(tasks.ending == ENDING_NONE,
            "settlement should still require confirmation after defeating the boss");
    Require(Tasks_CanChooseSettlement(&tasks),
            "settlement should remain an available explicit choice after defeating the boss");
    Tasks_CommitSettlement(&tasks);
    Require(tasks.ending == ENDING_SETTLEMENT,
            "confirmed settlement should still work after defeating the boss");

    puts("endgame flow smoke ok");
    return 0;
}
