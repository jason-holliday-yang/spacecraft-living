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

    fprintf(stderr, "endgame_new_survival_smoke failed: %s\n", message);
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
    tasks->signalTowerActivated = false;
    player->health = INITIAL_HEALTH;
    player->oxygen = INITIAL_OXYGEN;
    player->poison = 0.0f;
    player->hasProtectionSuit = false;
    Player_ClearAllStatuses(player);
}

static void PrepareEndingBranch(TaskSystem *tasks) {
    int index;

    tasks->westW5Completed = true;
    tasks->southS5Completed = true;
    for (index = 0; index < tasks->logCount; index++) {
        tasks->logs[index].collected = true;
        tasks->logs[index].active = true;
    }
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    Player preparedPlayer;
    char message[256];
    Player monolithPreparedPlayer;
    Player partiallyPreparedPlayer;
    Player peacefulRoutePlayer;
    float unpreparedHealth;
    float unpreparedOxygen;
    float preparedHealth;
    float preparedOxygen;
    float unlitTowerOxygen;
    float partialTowerOxygen;
    float litTowerOxygen;
    float peacefulTowerOxygen;
    float unlitBossHealth;
    float litBossHealth;

    ResetEndgameState(&map, &player, &tasks);
    player.gridX = EXTERIOR_X(84);
    player.gridY = EXTERIOR_Y(16);
    Tasks_Update(&tasks, &map, &player, 0.5f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK),
            "the stage 7 ruins should pressure the player with oxygen leak risk");
    unlitTowerOxygen = player.oxygen;

    ResetEndgameState(&map, &partiallyPreparedPlayer, &tasks);
    tasks.monolithsLit = 1;
    tasks.monolithActivated[1] = true;
    tasks.monolithPuzzle.active = true;
    tasks.monolithPuzzle.currentStep = 1;
    partiallyPreparedPlayer.gridX = EXTERIOR_X(84);
    partiallyPreparedPlayer.gridY = EXTERIOR_Y(16);
    Tasks_Update(&tasks, &map, &partiallyPreparedPlayer, 0.5f);
    partialTowerOxygen = partiallyPreparedPlayer.oxygen;
    Require(partialTowerOxygen > unlitTowerOxygen,
            "lighting the first monolith should already ease part of the final tower pressure");

    ResetEndgameState(&map, &monolithPreparedPlayer, &tasks);
    tasks.monolithsLit = 3;
    tasks.monolithActivated[0] = true;
    tasks.monolithActivated[1] = true;
    tasks.monolithActivated[2] = true;
    tasks.monolithPuzzle.solved = true;
    monolithPreparedPlayer.gridX = EXTERIOR_X(84);
    monolithPreparedPlayer.gridY = EXTERIOR_Y(16);
    Tasks_Update(&tasks, &map, &monolithPreparedPlayer, 0.5f);
    litTowerOxygen = monolithPreparedPlayer.oxygen;
    Require(litTowerOxygen > partialTowerOxygen,
            "finishing the full ring should further reduce oxygen pressure beyond the first monolith payoff");
    Require(litTowerOxygen > unlitTowerOxygen,
            "lighting the monoliths should reduce oxygen pressure on the final tower climb");

    ResetEndgameState(&map, &peacefulRoutePlayer, &tasks);
    peacefulRoutePlayer.hasSignalAmplifier = true;
    peacefulRoutePlayer.gridX = EXTERIOR_X(84);
    peacefulRoutePlayer.gridY = EXTERIOR_Y(16);
    Tasks_Update(&tasks, &map, &peacefulRoutePlayer, 0.5f);
    peacefulTowerOxygen = peacefulRoutePlayer.oxygen;
    Require(peacefulTowerOxygen > unlitTowerOxygen,
            "the peaceful route should reduce tower pressure compared with the forced climb");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = EXTERIOR_X(84);
    tasks.monsters[0].gridY = EXTERIOR_Y(20);
    tasks.monsters[0].area = MAP_AREA_RUINS;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 220.0f;
    tasks.monsters[0].maxHealth = 220.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_MELEE;
    player.gridX = EXTERIOR_X(83);
    player.gridY = EXTERIOR_Y(20);
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(player.health < INITIAL_HEALTH,
            "boss contact should still deal direct health damage");
    Require(player.poison > 0.0f,
            "boss contact should now apply poison pressure");
    Require(Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK),
            "boss contact should now apply oxygen leak pressure");
    unpreparedHealth = player.health;
    unpreparedOxygen = player.oxygen;
    unlitBossHealth = player.health;

    ResetEndgameState(&map, &preparedPlayer, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = EXTERIOR_X(84);
    tasks.monsters[0].gridY = EXTERIOR_Y(20);
    tasks.monsters[0].area = MAP_AREA_RUINS;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 220.0f;
    tasks.monsters[0].maxHealth = 220.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_MELEE;
    preparedPlayer.hasProtectionSuit = true;
    preparedPlayer.gridX = EXTERIOR_X(83);
    preparedPlayer.gridY = EXTERIOR_Y(20);
    Tasks_Update(&tasks, &map, &preparedPlayer, 0.0f);
    preparedHealth = preparedPlayer.health;
    preparedOxygen = preparedPlayer.oxygen;
    Require(preparedHealth > unpreparedHealth,
            "protection suit should noticeably improve boss survivability");
    Require(preparedOxygen > unpreparedOxygen,
            "protection suit should reduce the boss oxygen-collapse pressure");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monolithsLit = 3;
    tasks.monolithActivated[0] = true;
    tasks.monolithActivated[1] = true;
    tasks.monolithActivated[2] = true;
    tasks.monolithPuzzle.solved = true;
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = EXTERIOR_X(84);
    tasks.monsters[0].gridY = EXTERIOR_Y(20);
    tasks.monsters[0].area = MAP_AREA_RUINS;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 220.0f;
    tasks.monsters[0].maxHealth = 220.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_MELEE;
    player.gridX = EXTERIOR_X(83);
    player.gridY = EXTERIOR_Y(20);
    Tasks_Update(&tasks, &map, &player, 0.0f);
    litBossHealth = player.health;
    Require(litBossHealth > unlitBossHealth,
            "lit monoliths should weaken the guardian's direct combat pressure");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "peaceful route should be selectable once the archive is complete");
    player.hasSignalAmplifier = true;
    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "peaceful route should remain available under the new survival model");
    Require(tasks.ending == ENDING_PEACEFUL,
            "signal tower plus amplifier should still unlock the peaceful ending");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should be selectable once the archive is complete");
    tasks.bossDefeated = true;
    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "heroic route should remain available after the boss is defeated");
    Require(tasks.ending == ENDING_HEROIC,
            "boss-defeated tower interaction should still lead to heroic rescue");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    player.hasSignalAmplifier = true;
    player.gridX = LOXI_TERMINAL_X - 1;
    player.gridY = LOXI_TERMINAL_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "settlement route should still be selectable from the base terminal");
    Require(Tasks_CanChooseSettlement(&tasks),
            "settlement should remain exposed as an explicit route");
    Tasks_CommitSettlement(&tasks);
    Require(tasks.ending == ENDING_SETTLEMENT,
            "settlement should remain a valid third endgame route");

    puts("endgame_new_survival smoke ok");
    return 0;
}
