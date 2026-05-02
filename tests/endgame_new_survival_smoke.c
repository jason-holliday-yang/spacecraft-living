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

static Monster *FindMonsterByEncounter(TaskSystem *tasks, CombatEncounterId encounterId) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        if (tasks->monsters[index].encounterId == encounterId) {
            return &tasks->monsters[index];
        }
    }

    return NULL;
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

    tasks->endingArchiveReviewed = true;
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
    float unlitWeakPointTimer;
    float litWeakPointTimer;
    float normalBossHealthAfterHit;
    float weakPointBossHealthAfterHit;
    int heroicCompletionScore;
    int peacefulCompletionScore;
    int settlementCompletionScore;

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
    Require(Tasks_GetCombatScoreMax() == 120,
            "combat scoring should now expose the full fixed-encounter budget");
    Require(Tasks_GetCombatEncounterCount() == 8,
            "combat scoring should track the six key field encounters plus relic guard and final boss");
    Require(Tasks_GetCombatScore(&tasks) == 0,
            "a fresh run should start with zero combat score");
    tasks.ending = ENDING_HEROIC;
    heroicCompletionScore = Tasks_GetEndingCompletionScore(&tasks, &player);
    tasks.ending = ENDING_PEACEFUL;
    peacefulCompletionScore = Tasks_GetEndingCompletionScore(&tasks, &player);
    tasks.ending = ENDING_SETTLEMENT;
    settlementCompletionScore = Tasks_GetEndingCompletionScore(&tasks, &player);
    Require(heroicCompletionScore == peacefulCompletionScore
                && peacefulCompletionScore == settlementCompletionScore,
            "successful endings should now award the same completion score regardless of route");
    tasks.ending = ENDING_NONE;
    {
        Monster *westFrontierEncounter;

        westFrontierEncounter = FindMonsterByEncounter(&tasks, COMBAT_ENCOUNTER_WEST_FRONTIER);
        Require(westFrontierEncounter != NULL,
                "the west frontier encounter should stay tagged for combat scoring");
        westFrontierEncounter->active = false;
        westFrontierEncounter->health = 0.0f;
        Require(Tasks_IsCombatEncounterCompleted(&tasks, COMBAT_ENCOUNTER_WEST_FRONTIER),
                "defeated fixed encounters should report as completed");
        Require(Tasks_GetCombatScore(&tasks) == 10,
                "defeating the first field encounter should now contribute its fixed combat score");
    }

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 480.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_MELEE;
    player.gridX = BOSS_ARENA_BOSS_X - 1;
    player.gridY = BOSS_ARENA_BOSS_Y;
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
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 480.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_MELEE;
    preparedPlayer.hasProtectionSuit = true;
    preparedPlayer.gridX = BOSS_ARENA_BOSS_X - 1;
    preparedPlayer.gridY = BOSS_ARENA_BOSS_Y;
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
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 480.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_MELEE;
    player.gridX = BOSS_ARENA_BOSS_X - 1;
    player.gridY = BOSS_ARENA_BOSS_Y;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    litBossHealth = player.health;
    Require(litBossHealth > unlitBossHealth,
            "lit monoliths should weaken the guardian's direct combat pressure");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 300.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_CHARGE;
    tasks.monsters[0].attackTelegraph = 0.1f;
    tasks.monsters[0].targetX = BOSS_ARENA_BOSS_X + 5;
    tasks.monsters[0].targetY = BOSS_ARENA_BOSS_Y;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE + 3;
    player.gridY = BOSS_ARENA_BOSS_Y + 2;
    Tasks_Update(&tasks, &map, &player, 0.2f);
    unlitWeakPointTimer = tasks.monsters[0].weakPointTimer;
    Require(unlitWeakPointTimer > 0.0f,
            "dodging the guardian's line lock should expose a real core window");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monolithsLit = 3;
    tasks.monolithActivated[0] = true;
    tasks.monolithActivated[1] = true;
    tasks.monolithActivated[2] = true;
    tasks.monolithPuzzle.solved = true;
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 300.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_CHARGE;
    tasks.monsters[0].attackTelegraph = 0.1f;
    tasks.monsters[0].targetX = BOSS_ARENA_BOSS_X + 5;
    tasks.monsters[0].targetY = BOSS_ARENA_BOSS_Y;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE + 3;
    player.gridY = BOSS_ARENA_BOSS_Y + 2;
    Tasks_Update(&tasks, &map, &player, 0.2f);
    litWeakPointTimer = tasks.monsters[0].weakPointTimer;
    Require(litWeakPointTimer > unlitWeakPointTimer,
            "full monolith prep should extend the guardian core window after a dodged line lock");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 120.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_CHARGE;
    tasks.monsters[0].attackTelegraph = 0.1f;
    tasks.monsters[0].targetX = BOSS_ARENA_BOSS_X + 5;
    tasks.monsters[0].targetY = BOSS_ARENA_BOSS_Y;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE + 3;
    player.gridY = BOSS_ARENA_BOSS_Y;
    Tasks_Update(&tasks, &map, &player, 0.2f);
    Require(player.health < INITIAL_HEALTH,
            "the guardian final phase line lock should punish players who stay on the locked lane");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 480.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    player.hasLaserGun = true;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE;
    player.gridY = BOSS_ARENA_BOSS_Y;
    player.facingX = -1;
    player.facingY = 0;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleAttack(&tasks, &map, &player, message, sizeof(message)),
            "player should still be able to land a normal hit against the guardian");
    normalBossHealthAfterHit = tasks.monsters[0].health;

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 480.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].weakPointTimer = 1.4f;
    player.hasLaserGun = true;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE;
    player.gridY = BOSS_ARENA_BOSS_Y;
    player.facingX = -1;
    player.facingY = 0;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleAttack(&tasks, &map, &player, message, sizeof(message)),
            "player should be able to punish an exposed guardian weak point");
    weakPointBossHealthAfterHit = tasks.monsters[0].health;
    Require(weakPointBossHealthAfterHit < normalBossHealthAfterHit - 10.0f,
            "weak-point punishment should deal meaningfully more damage than a normal guardian hit");
    Require(tasks.monsters[0].weakPointTimer <= 0.0f,
            "landing the weak-point punish should consume the current guardian opening");
    Require(strstr(message, "Core hit") != NULL || strstr(message, "guardian staggers") != NULL,
            "core punishment should tell the player they exploited the opening");

    ResetEndgameState(&map, &player, &tasks);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 300.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_NONE;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE + 3;
    player.gridY = BOSS_ARENA_BOSS_Y;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(tasks.monsters[0].currentAttack == BOSS_ATTACK_CHARGE
                && tasks.monsters[0].attackTelegraph > 0.0f,
            "phase two guardian pressure should start a readable line-lock attack at range");
    Require(tasks.monsterCount == 1,
            "the redesigned guardian fight should stay focused on the boss instead of summoning reinforcements");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should be selectable for boss-fight combat verification");
    Map_LockSwampOuter(&map);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].unlockStage = 7;
    tasks.monsters[0].health = 20.0f;
    tasks.monsters[0].maxHealth = 480.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_NONE;
    player.hasLaserGun = true;
    player.gridX = BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE;
    player.gridY = BOSS_ARENA_BOSS_Y;
    player.facingX = -1;
    player.facingY = 0;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleAttack(&tasks, &map, &player, message, sizeof(message)),
            "player attack should be able to finish the guardian in Northwest Ruins");
    Require(tasks.bossDefeated,
            "killing the guardian through combat should mark the boss as defeated");
    Require(player.gridX == BOSS_ARENA_BOSS_X + MONSTER_FOOTPRINT_SIZE && player.gridY == BOSS_ARENA_BOSS_Y,
            "guardian defeat should leave the player in the in-world northwest ruins fight space");
    Require(player.resources[RESOURCE_BOSS_SCALE] >= 1,
            "guardian defeat should still award the boss-scale trophy resource");
    Require(strstr(message, "northwest ruins") != NULL
                || strstr(message, "Signal Tower") != NULL
                || strstr(message, "西北遗迹") != NULL,
            "guardian defeat should explain that the fight ended in-world and the route now continues to the tower or settlement confirmation");
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Heroic route chosen. Reach the Signal Tower and launch the rescue beacon.") == 0,
            "guardian defeat should hand the heroic route back to the tower finale");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "peaceful route should be selectable once the archive is complete");
    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "peaceful route should remain available under the new survival model");
    Require(tasks.ending == ENDING_PEACEFUL,
            "signal tower plus amplifier should still unlock the peaceful ending");
    Require(Tasks_CalculateEndingScore(&tasks, &player)
                == Tasks_GetArchiveScore(&tasks)
                    + Tasks_GetInvestigationScore(&tasks)
                    + Tasks_GetCombatScore(&tasks)
                    + Tasks_GetSurvivalScore(&tasks, &player)
                    + Tasks_GetEndingCompletionScore(&tasks, &player),
            "ending score breakdown helpers should stay aligned with the final score");
    Require(Tasks_CalculateEndingScore(&tasks, &player) >= 700,
            "a completed peaceful route should now award a strong endgame score");
    Require(strcmp(Tasks_GetEndingScoreRank(Tasks_CalculateEndingScore(&tasks, &player)), "A") == 0,
            "a completed peaceful route should land in the top A-to-E ending-score rank");

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
    Require(Tasks_CalculateEndingScore(&tasks, &player) > 0,
            "the ending score helper should produce a positive score once a successful ending is locked");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    player.hasSignalAmplifier = true;
    player.gridX = LOXI_TERMINAL_X - 1;
    player.gridY = LOXI_TERMINAL_Y;
    tasks.bossDefeated = true;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "settlement route should still be selectable from the base terminal");
    Require(Tasks_CanChooseSettlement(&tasks),
            "settlement should remain exposed as an explicit route");
    Tasks_CommitSettlement(&tasks);
    Require(tasks.ending == ENDING_SETTLEMENT,
            "settlement should remain a valid third endgame route");

    ResetEndgameState(&map, &player, &tasks);
    PrepareEndingBranch(&tasks);
    tasks.ending = ENDING_PEACEFUL;
    {
        const int scoreWithoutStoryArchive = Tasks_CalculateEndingScore(&tasks, &player);

        tasks.shownMainStorySceneCount = STORY_MAIN_SCENE_COUNT;
        Require(Tasks_CalculateEndingScore(&tasks, &player) > scoreWithoutStoryArchive,
                "ending score should now reward completed main-story archive progress even when images are placeholders");
    }

    puts("endgame_new_survival smoke ok");
    return 0;
}
