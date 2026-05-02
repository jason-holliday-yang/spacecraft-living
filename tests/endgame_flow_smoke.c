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

static void CollectSupplementalLog(TaskSystem *tasks, int logIndex) {
    tasks->logs[logIndex].active = true;
    tasks->logs[logIndex].collected = true;
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
    Require(strcmp(tasks.objective, "Recover West Frontier, South Collapse, and the remaining mainline logs before choosing an ending with Loxi.") == 0,
            "stage 7 should now keep the player on archive completion before the branch point opens");

    PrepareMainArchiveOnly(&tasks);
    MovePlayerToLoxiLeftPickupTile(&player);
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_IsEndingBranchReady(&tasks),
            "mainline archive completion should be enough to open the ending branch");
    Require(strcmp(tasks.objective, "Return to Loxi and review the assembled archive before choosing the final route. The story is finally whole enough to judge what your answer should cost.") == 0,
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
    Require((strstr(tasks.objective, "Canopy Handoff") != NULL
                || strstr(tasks.objective, "林冠交接") != NULL)
                && strstr(tasks.objective, "Loxi") != NULL,
            "after the archive review, the objective should point to the next missing route unlock when no ending is unlocked yet");

    CollectSupplementalLog(&tasks, 7);
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetAvailableEndingCount(&tasks) == 0,
            "recovering only the last-camp settlement record should not unlock settlement before the guardian dies");
    Require(!Tasks_IsEndingAvailable(&tasks, ENDING_SETTLEMENT),
            "last-camp inheritance evidence alone should not unlock settlement without the guardian kill");
    Require(!Tasks_IsEndingAvailable(&tasks, ENDING_HEROIC),
            "settlement-only evidence should not auto-unlock heroic rescue");
    Require(!Tasks_IsEndingAvailable(&tasks, ENDING_PEACEFUL),
            "settlement-only evidence should not auto-unlock peaceful rescue");
    Require(strcmp(tasks.objective, "Recover the Canopy Handoff record, then return to Loxi. Heroic rescue should be chosen with the west crew's relay effort fully in view.") == 0,
            "without the guardian kill, the objective should still stay focused on the next missing route unlock");
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "loxi terminal should explain why settlement is still blocked");
    Require(strstr(message, "guardian") != NULL || strstr(message, "守卫") != NULL,
            "settlement-blocked Loxi guidance should explicitly mention the guardian gate");

    player.hasSignalAmplifier = false;
    CollectSupplementalLog(&tasks, 12);
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetAvailableEndingCount(&tasks) == 0,
            "adding peaceful-route evidence should still not unlock peaceful rescue before the Signal Amplifier is crafted");
    Require(!Tasks_IsEndingAvailable(&tasks, ENDING_PEACEFUL),
            "south maintenance control evidence should not unlock peaceful rescue without the Signal Amplifier");
    Require(strcmp(tasks.objective, "Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then return to Loxi so peaceful rescue can be argued from evidence instead of hope alone.") == 0,
            "without the Signal Amplifier, the objective should redirect the player to peaceful-route preparation instead of route confirmation");
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "loxi terminal should explain that peaceful rescue still needs the Signal Amplifier");
    Require(strstr(message, "Signal Amplifier") != NULL || strstr(message, "信号放大器") != NULL,
            "peaceful-route Loxi guidance should explicitly mention the missing Signal Amplifier gate");

    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetAvailableEndingCount(&tasks) == 1,
            "crafting the Signal Amplifier should finally unlock peaceful rescue while settlement stays boss-gated");
    Require(Tasks_GetAvailableEndingAt(&tasks, 0) == ENDING_PEACEFUL,
            "before the guardian dies, the only unlocked ending should become peaceful rescue once the amplifier exists");
    Require(Tasks_IsEndingAvailable(&tasks, ENDING_PEACEFUL),
            "south maintenance control evidence plus the Signal Amplifier should unlock peaceful rescue");
    Require(strcmp(tasks.objective, "Return to Loxi and confirm the only unlocked ending the archive still supports: Peaceful Rescue.") == 0,
            "once the Signal Amplifier exists, the objective should shift to single-ending confirmation");

    PrepareEndingBranch(&tasks);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Return to Loxi and choose among the unlocked endings.") == 0,
            "once every ending-line archive is complete, the objective should point back to Loxi for final choice");
    Require(Tasks_GetAvailableEndingCount(&tasks) == 2,
            "before the guardian dies, the fully recovered archive should still expose only heroic and peaceful");
    Require(Tasks_GetAvailableEndingAt(&tasks, 0) == ENDING_HEROIC
                && Tasks_GetAvailableEndingAt(&tasks, 1) == ENDING_PEACEFUL,
            "available ending order should stay heroic then peaceful before settlement's boss gate is cleared");

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "loxi terminal should remain interactive once the branch point opens");
    Require(tasks.ending == ENDING_NONE,
            "base terminal should not commit the settlement ending without confirmation");
    Require(!Tasks_CanChooseSettlement(&tasks),
            "base terminal should keep settlement unavailable until the guardian is defeated");
    Require(message[0] != '\0',
            "settlement interaction should explain that confirmation is required");
    Require(message[0] != '\0',
            "settlement interaction should explain that settlement closes both rescue routes");
    tasks.bossDefeated = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetAvailableEndingCount(&tasks) == 3,
            "defeating the guardian should add settlement back into the fully recovered archive set");
    Require(Tasks_GetAvailableEndingAt(&tasks, 0) == ENDING_HEROIC
                && Tasks_GetAvailableEndingAt(&tasks, 1) == ENDING_PEACEFUL
                && Tasks_GetAvailableEndingAt(&tasks, 2) == ENDING_SETTLEMENT,
            "available ending order should become heroic, peaceful, settlement once the guardian is defeated");
    Require(Tasks_CanChooseSettlement(&tasks),
            "base terminal should expose settlement as an explicit available choice after the guardian falls");
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
    Tasks_UpdateObjective(&tasks, &player);
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
    Require(strcmp(tasks.objective, "Heroic route chosen. Defeat the guardian in the northwest ruins, then return to the Signal Tower.") == 0,
            "heroic route should now point directly to the northwest guardian fight");
    Map_LockSwampOuter(&map);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "heroic airlock interaction should remain a normal door toggle");
    Require(player.gridX == AIRLOCK_CONSOLE_X - 1 && player.gridY == AIRLOCK_CONSOLE_Y,
            "heroic airlock interaction should no longer teleport the player away from the world-map guardian fight");
    Require(Map_IsSwampOuterUnlocked(&map),
            "heroic airlock interaction should still open the outer route");
    Require(strstr(message, "Airlock") != NULL || strstr(message, "气闸") != NULL,
            "heroic airlock interaction should explain the normal airlock cycle");
    Require(strcmp(tasks.objective, "Heroic route chosen. Defeat the guardian in the northwest ruins, then return to the Signal Tower.") == 0,
            "heroic airlock use should leave the route objective focused on the northwest guardian fight");

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
    Tasks_UpdateObjective(&tasks, &player);
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
