#include "task_system.h"

#include "localization.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static int DistanceManhattan(int ax, int ay, int bx, int by) {
    return AbsInt(ax - bx) + AbsInt(ay - by);
}

static bool IsNearTile(const Player *player, int gridX, int gridY) {
    return DistanceManhattan(player->gridX, player->gridY, gridX, gridY) <= 1;
}

static void WriteMessage(char *message, size_t messageSize, const char *text) {
    if (message != NULL && messageSize > 0) {
        snprintf(message, messageSize, "%s", text);
    }
}

static void AddNode(TaskSystem *tasks, ResourceType type, int gridX, int gridY, int yieldCount, int respawns, bool special) {
    ResourceNode *node;

    if (tasks->nodeCount >= MAX_RESOURCE_NODES) {
        return;
    }

    node = &tasks->nodes[tasks->nodeCount++];
    node->active = true;
    node->type = type;
    node->gridX = gridX;
    node->gridY = gridY;
    node->baseYield = yieldCount;
    node->respawnsRemaining = respawns;
    node->special = special;
    node->area = Map_GetAreaAt(gridX, gridY);
    node->awayTimer = 0.0f;
}

static void AddMonster(TaskSystem *tasks, MonsterType type, int gridX, int gridY, int unlockStage) {
    Monster *monster;

    if (tasks->monsterCount >= MAX_MONSTERS) {
        return;
    }

    monster = &tasks->monsters[tasks->monsterCount++];
    memset(monster, 0, sizeof(*monster));
    monster->active = true;
    monster->type = type;
    monster->gridX = gridX;
    monster->gridY = gridY;
    monster->spawnX = gridX;
    monster->spawnY = gridY;
    monster->unlockStage = unlockStage;
    monster->area = Map_GetAreaAt(gridX, gridY);

    switch (type) {
        case MONSTER_THORN_LARVA:
            monster->maxHealth = 28.0f;
            break;
        case MONSTER_WING_BUG:
            monster->maxHealth = 18.0f;
            break;
        case MONSTER_RAPTOR:
            monster->maxHealth = 52.0f;
            break;
        case MONSTER_SWAMP_STALKER:
            monster->maxHealth = 48.0f;
            break;
        case MONSTER_SENTINEL_JELLY:
            monster->maxHealth = 72.0f;
            break;
        case MONSTER_FOG_WORM:
            monster->maxHealth = 64.0f;
            break;
        case MONSTER_RELIC_GUARD:
            monster->maxHealth = 92.0f;
            break;
        case MONSTER_FINAL_BOSS:
            monster->maxHealth = 220.0f;
            break;
        default:
            monster->maxHealth = 20.0f;
            break;
    }

    monster->health = monster->maxHealth;
}

static void AddLog(TaskSystem *tasks, int gridX, int gridY, int rewardKind) {
    ShipLog *log;

    if (tasks->logCount >= MAX_LOGS) {
        return;
    }

    log = &tasks->logs[tasks->logCount++];
    log->active = true;
    log->collected = false;
    log->gridX = gridX;
    log->gridY = gridY;
    log->rewardKind = rewardKind;
}

static ResourceNode *FindNearbyNode(TaskSystem *tasks, const Player *player) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->active && DistanceManhattan(player->gridX, player->gridY, node->gridX, node->gridY) <= 1) {
            return node;
        }
    }

    return NULL;
}

static ShipLog *FindNearbyLog(TaskSystem *tasks, const Player *player) {
    int index;

    for (index = 0; index < tasks->logCount; index++) {
        ShipLog *log;

        log = &tasks->logs[index];
        if (log->active && !log->collected && DistanceManhattan(player->gridX, player->gridY, log->gridX, log->gridY) <= 1) {
            return log;
        }
    }

    return NULL;
}

static Monster *FindMonsterAt(TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && monster->gridX == gridX && monster->gridY == gridY) {
            return monster;
        }
    }

    return NULL;
}

static Monster *FindAttackTarget(TaskSystem *tasks, const Player *player) {
    int range;
    int step;

    if (player->hasLaserGun) {
        range = 5;
        for (step = 1; step <= range; step++) {
            int targetX;
            int targetY;
            Monster *monster;

            targetX = player->gridX + player->facingX * step;
            targetY = player->gridY + player->facingY * step;
            monster = FindMonsterAt(tasks, targetX, targetY);
            if (monster != NULL) {
                return monster;
            }
        }
    }

    for (step = 0; step < 4; step++) {
        static const int dirs[4][2] = {
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1}
        };
        Monster *monster;

        monster = FindMonsterAt(tasks, player->gridX + dirs[step][0], player->gridY + dirs[step][1]);
        if (monster != NULL) {
            return monster;
        }
    }

    return NULL;
}

static bool CollectNode(TaskSystem *tasks, Player *player, ResourceNode *node, char *message, size_t messageSize) {
    float gatherCost;
    int yieldCount;
    const char *resourceLabel;

    gatherCost = (node->special ? STAMINA_SPECIAL_GATHER_COST : STAMINA_GATHER_COST) * Player_GetGatherMultiplier(player);
    if (player->stamina < gatherCost) {
        WriteMessage(message, messageSize, "Not enough stamina to gather.");
        return false;
    }

    yieldCount = node->baseYield;
    if (tasks->currentEvent == EVENT_HARVEST) {
        yieldCount *= 2;
    }

    Player_ConsumeStamina(player, gatherCost);
    Player_AddResource(player, node->type, yieldCount);
    resourceLabel = Player_GetResourceLabel(node->type);

    if (node->respawnsRemaining > 0) {
        node->respawnsRemaining -= 1;
        node->active = false;
        node->awayTimer = 0.0f;
    } else {
        node->active = false;
    }

    snprintf(message, messageSize, "Collected %s x%d", resourceLabel, yieldCount);
    return true;
}

static void UnlockStageIfNeeded(TaskSystem *tasks, GameMap *map, int newStage) {
    if (newStage <= tasks->stage) {
        return;
    }

    tasks->stage = newStage;
    if (tasks->stage >= 5) {
        Map_UnlockSwampDeep(map);
    }
    if (tasks->stage >= 6) {
        Map_UnlockRuins(map);
    }
}

static void GrantLogReward(TaskSystem *tasks, Player *player, ShipLog *log, char *message, size_t messageSize) {
    log->collected = true;

    switch (log->rewardKind) {
        case 0:
            player->maxStaminaBonus += 6.0f;
            WriteMessage(message, messageSize, "Ship log recovered: permanent max stamina increased.");
            break;
        case 1:
            player->attackBonus += 4.0f;
            WriteMessage(message, messageSize, "Ship log recovered: weapon calibration improved your attacks.");
            break;
        case 2:
            Player_AddOxygen(player, 18.0f);
            WriteMessage(message, messageSize, "Ship log recovered: Loxi restored part of the oxygen reserve.");
            break;
        case 3:
            Player_RelievePressure(player, 22.0f);
            WriteMessage(message, messageSize, "Ship log recovered: the crash truth helped you regain control.");
            break;
        case 4:
            Player_AddResource(player, RESOURCE_ENERGY_CORE, 1);
            WriteMessage(message, messageSize, "Hidden log reward: gained one Energy Core.");
            break;
        default:
            WriteMessage(message, messageSize, "Ship log recovered: new clues were added.");
            break;
    }

    Tasks_UpdateObjective(tasks, player);
}

static void RespawnNodes(TaskSystem *tasks, const Player *player, float deltaTime) {
    int index;
    MapArea playerArea;

    playerArea = Map_GetAreaAt(player->gridX, player->gridY);

    for (index = 0; index < tasks->nodeCount; index++) {
        ResourceNode *node;
        float threshold;

        node = &tasks->nodes[index];
        if (node->active || node->respawnsRemaining <= 0) {
            continue;
        }

        if (playerArea != node->area) {
            node->awayTimer += deltaTime;
        } else {
            node->awayTimer = 0.0f;
        }

        threshold = node->special ? 300.0f : 120.0f;
        if (node->awayTimer >= threshold) {
            node->active = true;
            node->awayTimer = 0.0f;
        }
    }
}

static void RotateEventAtDawn(TaskSystem *tasks) {
    static const EventType eventCycle[EVENT_COUNT] = {
        EVENT_HARVEST,
        EVENT_CALM_BEASTS,
        EVENT_CLEAR_SKY,
        EVENT_SPORE_STORM,
        EVENT_MONSTER_FRENZY,
        EVENT_DEVICE_FAULT
    };

    tasks->currentEvent = eventCycle[tasks->dayCount % EVENT_COUNT];
}

static void UpdateDayCycle(TaskSystem *tasks, Player *player, float deltaTime) {
    float previousTimer;
    int previousDay;
    float cycleTime;

    previousTimer = tasks->cycleTimer;
    previousDay = tasks->dayCount;
    tasks->cycleTimer += deltaTime;
    tasks->elapsedSeconds += deltaTime;

    while (tasks->cycleTimer >= FULL_CYCLE_SECONDS) {
        tasks->cycleTimer -= FULL_CYCLE_SECONDS;
        tasks->dayCount += 1;
    }

    cycleTime = tasks->cycleTimer;
    if (cycleTime < DAY_DURATION_SECONDS) {
        tasks->phase = DAY_PHASE_DAY;
    } else if (cycleTime < DAY_DURATION_SECONDS + DUSK_DURATION_SECONDS) {
        tasks->phase = DAY_PHASE_DUSK;
    } else {
        tasks->phase = DAY_PHASE_NIGHT;
    }

    if (tasks->dayCount != previousDay || (previousTimer > tasks->cycleTimer && tasks->cycleTimer < 1.0f)) {
        RotateEventAtDawn(tasks);
    }

    if (tasks->phase == DAY_PHASE_NIGHT && player->noPressureTimer <= 0.0f) {
        Player_AddPressure(player, deltaTime * 1.3f);
    } else if (tasks->phase == DAY_PHASE_DUSK && player->noPressureTimer <= 0.0f) {
        Player_AddPressure(player, deltaTime * 0.45f);
    }
}

static float GetOxygenLeakRate(const TaskSystem *tasks) {
    float rate;
    int pressureCycles;

    rate = 15.0f / FULL_CYCLE_SECONDS;
    if (tasks->oxygenRepairLevel >= 1) {
        rate *= 0.70f;
    }
    if (tasks->oxygenRepairLevel >= 2) {
        rate = 0.0f;
    }

    pressureCycles = tasks->dayCount / 3;
    rate *= 1.0f + 0.08f * pressureCycles;

    if (tasks->currentEvent == EVENT_DEVICE_FAULT) {
        rate *= 1.35f;
    }

    return rate;
}

static bool MonsterCanAct(const Monster *monster, const TaskSystem *tasks) {
    return monster->active && tasks->stage >= monster->unlockStage;
}

static bool IsTileOccupiedByMonster(const TaskSystem *tasks, const Monster *self, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *other;

        other = &tasks->monsters[index];
        if (!other->active || other == self) {
            continue;
        }

        if (other->gridX == gridX && other->gridY == gridY) {
            return true;
        }
    }

    return false;
}

static void MoveMonsterToward(TaskSystem *tasks, Monster *monster, const GameMap *map, int targetX, int targetY) {
    int stepX;
    int stepY;
    int tryX;
    int tryY;

    stepX = 0;
    stepY = 0;
    if (targetX > monster->gridX) {
        stepX = 1;
    } else if (targetX < monster->gridX) {
        stepX = -1;
    }

    if (targetY > monster->gridY) {
        stepY = 1;
    } else if (targetY < monster->gridY) {
        stepY = -1;
    }

    tryX = monster->gridX + stepX;
    tryY = monster->gridY;
    if (stepX != 0 && Map_IsWalkable(map, tryX, tryY) && !IsTileOccupiedByMonster(tasks, monster, tryX, tryY)) {
        monster->gridX = tryX;
        monster->gridY = tryY;
        return;
    }

    tryX = monster->gridX;
    tryY = monster->gridY + stepY;
    if (stepY != 0 && Map_IsWalkable(map, tryX, tryY) && !IsTileOccupiedByMonster(tasks, monster, tryX, tryY)) {
        monster->gridX = tryX;
        monster->gridY = tryY;
    }
}

static void ApplyMonsterAttack(const TaskSystem *tasks, MonsterType type, Player *player) {
    float damage;
    float pressure;

    switch (type) {
        case MONSTER_THORN_LARVA:
            damage = 3.0f;
            pressure = 8.0f;
            break;
        case MONSTER_WING_BUG:
            damage = 1.0f;
            pressure = 5.0f;
            break;
        case MONSTER_RAPTOR:
            damage = 6.0f;
            pressure = 12.0f;
            break;
        case MONSTER_SWAMP_STALKER:
            damage = 5.0f;
            pressure = 10.0f;
            break;
        case MONSTER_SENTINEL_JELLY:
            damage = 5.0f;
            pressure = 10.0f;
            break;
        case MONSTER_FOG_WORM:
            damage = 5.0f;
            pressure = 11.0f;
            break;
        case MONSTER_RELIC_GUARD:
            damage = 10.0f;
            pressure = 15.0f;
            break;
        case MONSTER_FINAL_BOSS:
            damage = 15.0f;
            pressure = 20.0f;
            break;
        default:
            damage = 4.0f;
            pressure = 6.0f;
            break;
    }

    if (player->hasProtectionSuit) {
        damage *= 0.70f;
        pressure *= 0.60f;
    }

    if (tasks->currentEvent == EVENT_MONSTER_FRENZY) {
        damage *= 1.25f;
        pressure *= 1.15f;
    }

    Player_ConsumeStamina(player, damage);
    Player_AddPressure(player, pressure);
}

static void TrySpawnBossGuards(TaskSystem *tasks) {
    int guardCount;
    int index;

    guardCount = 0;
    for (index = 0; index < tasks->monsterCount; index++) {
        if (tasks->monsters[index].active && tasks->monsters[index].type == MONSTER_RELIC_GUARD) {
            guardCount += 1;
        }
    }

    if (guardCount >= 3) {
        return;
    }

    AddMonster(tasks, MONSTER_RELIC_GUARD, 90, 44, 7);
    AddMonster(tasks, MONSTER_RELIC_GUARD, 94, 49, 7);
}

static void UpdateMonsters(TaskSystem *tasks, const GameMap *map, Player *player, float deltaTime) {
    int index;
    MapArea playerArea;

    playerArea = Map_GetAreaAt(player->gridX, player->gridY);

    for (index = 0; index < tasks->monsterCount; index++) {
        Monster *monster;
        float aggroRange;
        bool sameArea;
        bool hiddenByStealth;

        monster = &tasks->monsters[index];
        if (!MonsterCanAct(monster, tasks)) {
            continue;
        }

        monster->moveTimer -= deltaTime;
        monster->attackTimer -= deltaTime;

        if (monster->type == MONSTER_FINAL_BOSS && monster->health <= monster->maxHealth * 0.70f && !monster->phaseTriggered) {
            monster->phaseTriggered = true;
            TrySpawnBossGuards(tasks);
        }

        sameArea = monster->area == playerArea;
        hiddenByStealth = player->crouching && playerArea == MAP_AREA_FOREST && DistanceManhattan(player->gridX, player->gridY, monster->gridX, monster->gridY) > 1;
        if (!sameArea || hiddenByStealth) {
            continue;
        }

        aggroRange = 4.0f;
        if (monster->type == MONSTER_FINAL_BOSS) {
            aggroRange = 8.0f;
        } else if (monster->type == MONSTER_WING_BUG && tasks->phase == DAY_PHASE_DAY) {
            aggroRange = 2.0f;
        } else if (monster->type == MONSTER_SWAMP_STALKER && tasks->phase != DAY_PHASE_DAY) {
            aggroRange = 5.0f;
        }

        aggroRange *= Player_GetAggroMultiplier(player);
        if (tasks->currentEvent == EVENT_CALM_BEASTS) {
            aggroRange *= 0.5f;
        }
        if (tasks->currentEvent == EVENT_MONSTER_FRENZY) {
            aggroRange *= 1.2f;
        }

        if ((float)DistanceManhattan(player->gridX, player->gridY, monster->gridX, monster->gridY) > aggroRange) {
            continue;
        }

        if (DistanceManhattan(player->gridX, player->gridY, monster->gridX, monster->gridY) == 1) {
            if (monster->attackTimer <= 0.0f) {
                ApplyMonsterAttack(tasks, monster->type, player);
                monster->attackTimer = monster->type == MONSTER_FINAL_BOSS ? 1.2f : 0.9f;
            }
            continue;
        }

        if (monster->moveTimer <= 0.0f) {
            MoveMonsterToward(tasks, monster, map, player->gridX, player->gridY);
            monster->moveTimer = monster->type == MONSTER_FINAL_BOSS ? 0.45f : 0.70f;
        }
    }
}

static void HandlePlayerCollapse(TaskSystem *tasks, GameMap *map, Player *player) {
    float restoredStamina;
    float pressureFloor;

    player->deathCount += 1;
    player->gridX = PLAYER_START_X;
    player->gridY = PLAYER_START_Y;
    player->facingX = 0;
    player->facingY = 1;
    Player_UpdateWorldPosition(player);
    restoredStamina = Player_GetCurrentStaminaCap(player) * 0.55f;
    player->stamina = restoredStamina;
    pressureFloor = player->pressure < 100.0f ? 100.0f : player->pressure;
    player->pressure = ClampFloat(pressureFloor, 0.0f, MAX_PRESSURE);
    player->poison = 0.0f;
    player->moveTimer = 0.0f;
    player->safeRecoveryTimer = 0.0f;
    player->pressureDamageTimer = 0.0f;
    player->glowStickTimer = 0.0f;
    player->speedBoostTimer = 0.0f;
    player->noPressureTimer = 0.0f;
    map->campPlaced = false;

    if (player->deathCount >= 3) {
        tasks->ending = ENDING_FAILURE;
    }
}

static void UpdatePlayerStatus(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    HazardType hazard;
    float safeRecoveryAmount;

    player->glowStickTimer = fmaxf(0.0f, player->glowStickTimer - deltaTime);
    player->speedBoostTimer = fmaxf(0.0f, player->speedBoostTimer - deltaTime);
    player->noPressureTimer = fmaxf(0.0f, player->noPressureTimer - deltaTime);
    player->blurPulse += deltaTime;

    Player_DamageOxygen(player, GetOxygenLeakRate(tasks) * deltaTime);
    if (player->oxygen < 10.0f) {
        Player_ConsumeStamina(player, deltaTime);
    }

    if (player->oxygen <= 0.0f) {
        tasks->ending = ENDING_FAILURE;
        return;
    }

    if (player->oxygen < 30.0f) {
        Player_AddPressure(player, deltaTime * 0.60f);
    }

    hazard = Map_GetHazardAt(map, player->gridX, player->gridY);
    if (hazard == HAZARD_TRIP) {
        Player_ConsumeStamina(player, deltaTime * 0.8f);
        Player_AddPressure(player, deltaTime * 0.8f);
    } else if (hazard == HAZARD_SWAMP) {
        Player_ConsumeStamina(player, deltaTime * 1.4f);
        Player_AddPressure(player, deltaTime * 0.8f);
    } else if (hazard == HAZARD_POISON) {
        Player_AddPoison(player, deltaTime * 6.5f);
        Player_AddPressure(player, deltaTime * 1.0f);
    }

    if (player->poison >= MAX_POISON) {
        Player_ConsumeStamina(player, deltaTime * 1.2f);
        Player_AddPressure(player, deltaTime * 0.8f);
    }

    if (player->pressure >= 100.0f) {
        player->pressureDamageTimer += deltaTime;
        while (player->pressureDamageTimer >= PRESSURE_DAMAGE_INTERVAL) {
            Player_ConsumeStamina(player, 1.0f);
            player->pressureDamageTimer -= PRESSURE_DAMAGE_INTERVAL;
        }
    } else {
        player->pressureDamageTimer = 0.0f;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE || (map->campPlaced && DistanceManhattan(player->gridX, player->gridY, map->campX, map->campY) <= 1)) {
        player->safeRecoveryTimer += deltaTime;
        safeRecoveryAmount = tasks->oxygenRepairLevel >= 2 ? 3.0f : SAFE_RECOVERY_AMOUNT;
        while (player->safeRecoveryTimer >= SAFE_RECOVERY_INTERVAL) {
            Player_RecoverStamina(player, safeRecoveryAmount);
            player->safeRecoveryTimer -= SAFE_RECOVERY_INTERVAL;
        }
    } else {
        player->safeRecoveryTimer = 0.0f;
    }

    if (player->stamina <= 0.0f) {
        HandlePlayerCollapse(tasks, map, player);
    }
}

void Tasks_Init(TaskSystem *tasks, GameMap *map) {
    (void)map;

    memset(tasks, 0, sizeof(*tasks));
    tasks->stage = 1;
    tasks->phase = DAY_PHASE_DAY;
    tasks->currentEvent = EVENT_HARVEST;
    tasks->communicatorUnlocked = false;
    
    Puzzle_Init(&tasks->monolithPuzzle);

    AddNode(tasks, RESOURCE_WOOD, 34, 55, 1, 3, false);
    AddNode(tasks, RESOURCE_WOOD, 37, 56, 1, 3, false);
    AddNode(tasks, RESOURCE_WOOD, 56, 55, 1, 3, false);
    AddNode(tasks, RESOURCE_WOOD, 72, 31, 1, 3, false);
    AddNode(tasks, RESOURCE_WOOD, 74, 38, 1, 3, false);
    AddNode(tasks, RESOURCE_WOOD, 77, 57, 1, 3, false);
    AddNode(tasks, RESOURCE_METAL_SCRAP, 34, 42, 1, 3, false);
    AddNode(tasks, RESOURCE_METAL_SCRAP, 46, 42, 1, 3, false);
    AddNode(tasks, RESOURCE_METAL_SCRAP, 56, 58, 1, 3, false);
    AddNode(tasks, RESOURCE_FRUIT, 73, 51, 1, 3, false);
    AddNode(tasks, RESOURCE_FRUIT, 80, 61, 1, 3, false);
    AddNode(tasks, RESOURCE_GLOW_MOSS, 33, 56, 1, 3, false);
    AddNode(tasks, RESOURCE_GLOW_MOSS, 46, 57, 1, 3, false);
    AddNode(tasks, RESOURCE_ORE, 45, 30, 1, 3, false);
    AddNode(tasks, RESOURCE_ORE, 53, 57, 1, 3, false);
    AddNode(tasks, RESOURCE_SPECIAL_FUNGUS, 76, 70, 1, 3, false);
    AddNode(tasks, RESOURCE_SPECIAL_FUNGUS, 84, 77, 1, 3, false);
    AddNode(tasks, RESOURCE_ALIEN_VINE, 76, 69, 1, 3, false);
    AddNode(tasks, RESOURCE_ALIEN_VINE, 82, 74, 1, 3, false);
    AddNode(tasks, RESOURCE_SHELL_FRUIT, 79, 72, 1, 3, false);
    AddNode(tasks, RESOURCE_SHELL_FRUIT, 86, 79, 1, 3, false);
    AddNode(tasks, RESOURCE_JUNK_METAL, 72, 53, 1, 3, false);
    AddNode(tasks, RESOURCE_JUNK_METAL, 89, 38, 1, 3, false);
    AddNode(tasks, RESOURCE_ENERGY_CRYSTAL, 84, 58, 1, 3, false);
    AddNode(tasks, RESOURCE_ENERGY_CRYSTAL, 92, 53, 1, 3, false);
    AddNode(tasks, RESOURCE_CALM_MUSHROOM, 83, 61, 1, 3, false);
    AddNode(tasks, RESOURCE_CALM_MUSHROOM, 88, 63, 1, 3, false);
    AddNode(tasks, RESOURCE_PROTECTIVE_FIBER, 85, 57, 1, 3, false);
    AddNode(tasks, RESOURCE_PROTECTIVE_FIBER, 91, 60, 1, 3, false);
    AddNode(tasks, RESOURCE_RELIC_FRAGMENT, 90, 40, 1, 2, true);
    AddNode(tasks, RESOURCE_RELIC_FRAGMENT, 94, 49, 1, 2, true);
    AddNode(tasks, RESOURCE_RELIC_FRAGMENT, 96, 67, 1, 2, true);
    AddNode(tasks, RESOURCE_ALIEN_SLIME, 89, 56, 1, 2, true);
    AddNode(tasks, RESOURCE_ALIEN_SLIME, 93, 35, 1, 2, true);

    AddMonster(tasks, MONSTER_THORN_LARVA, 73, 44, 2);
    AddMonster(tasks, MONSTER_THORN_LARVA, 78, 56, 2);
    AddMonster(tasks, MONSTER_WING_BUG, 81, 33, 2);
    AddMonster(tasks, MONSTER_RAPTOR, 79, 73, 3);
    AddMonster(tasks, MONSTER_SWAMP_STALKER, 84, 76, 3);
    AddMonster(tasks, MONSTER_SENTINEL_JELLY, 86, 58, 5);
    AddMonster(tasks, MONSTER_FOG_WORM, 91, 61, 5);
    AddMonster(tasks, MONSTER_RELIC_GUARD, 89, 41, 7);
    AddMonster(tasks, MONSTER_RELIC_GUARD, 95, 47, 7);
    AddMonster(tasks, MONSTER_FINAL_BOSS, 92, 45, 7);

    AddLog(tasks, 33, 33, 0);
    AddLog(tasks, 58, 57, 1);
    AddLog(tasks, 52, 73, 2);
    AddLog(tasks, 90, 37, 3);
    AddLog(tasks, 96, 67, 4);

    tasks->stage = 1;
    tasks->objective[0] = '\0';
    tasks->communicator[0] = '\0';
}

void Tasks_UpdateObjective(TaskSystem *tasks, const Player *player) {
    switch (tasks->stage) {
        case 1:
            snprintf(tasks->objective, sizeof(tasks->objective), "Gather 3 Wood and 2 Metal Scrap, then repair the first oxygen module at the base.");
            break;
        case 2:
            snprintf(tasks->objective, sizeof(tasks->objective), "Gather Glow Moss and Ore, craft a Glow Stick, and finish the oxygen repair.");
            break;
        case 3:
            snprintf(tasks->objective, sizeof(tasks->objective), "Use the airlock console, open the outer door, then gather vines, fruit, and fungi to repair the comm relay.");
            break;
        case 4:
            snprintf(tasks->objective, sizeof(tasks->objective), "Use the base workbench to craft the Laser Gun and Suit, then inspect the crash clue.");
            break;
        case 5:
            snprintf(tasks->objective, sizeof(tasks->objective), "Secure an Energy Core and restore the power module at the base.");
            break;
        case 6:
            snprintf(tasks->objective, sizeof(tasks->objective), "Collect 3 Relic Fragments and return to base to unlock the endgame path.");
            break;
        case 7:
            if (player->hasSignalAmplifier) {
                snprintf(tasks->objective, sizeof(tasks->objective), "Reach the Signal Tower and use the Signal Amplifier for the peaceful ending.");
            } else if (!tasks->bossDefeated) {
                snprintf(tasks->objective, sizeof(tasks->objective), "Light monoliths or defeat the final boss, then activate the Signal Tower.");
            } else {
                snprintf(tasks->objective, sizeof(tasks->objective), "Activate rescue at the tower, or return to base to choose settlement.");
            }
            break;
        default:
            snprintf(tasks->objective, sizeof(tasks->objective), "Follow Loxi and keep exploring.");
            break;
    }

    snprintf(
        tasks->communicator,
        sizeof(tasks->communicator),
        "Loxi Link: %s\nCurrent Stage: %s\nCurrent Area: %s\nCurrent Cycle: Day %d - %s\nCurrent Task: %s",
        tasks->communicatorUnlocked ? "Online" : "Standby",
        Tasks_GetStageName(tasks->stage),
        Map_GetAreaName(Map_GetAreaAt(player->gridX, player->gridY)),
        tasks->dayCount + 1,
        Tasks_GetPhaseName(tasks->phase),
        tasks->objective
    );
}

void Tasks_Update(TaskSystem *tasks, GameMap *map, Player *player, float deltaTime) {
    UpdateDayCycle(tasks, player, deltaTime);
    RespawnNodes(tasks, player, deltaTime);
    UpdateMonsters(tasks, map, player, deltaTime);
    UpdatePlayerStatus(tasks, map, player, deltaTime);
    Tasks_UpdateObjective(tasks, player);
}

static void DropMonsterRewards(TaskSystem *tasks, Player *player, Monster *monster) {
    switch (monster->type) {
        case MONSTER_THORN_LARVA:
            Player_AddResource(player, RESOURCE_FRUIT, 1);
            break;
        case MONSTER_WING_BUG:
            Player_AddResource(player, RESOURCE_GLOW_MOSS, 1);
            break;
        case MONSTER_RAPTOR:
            Player_AddResource(player, RESOURCE_SPECIAL_FUNGUS, 1);
            break;
        case MONSTER_SWAMP_STALKER:
            Player_AddResource(player, RESOURCE_SHELL_FRUIT, 1);
            Player_AddResource(player, RESOURCE_ALIEN_VINE, 1);
            break;
        case MONSTER_SENTINEL_JELLY:
            Player_AddResource(player, RESOURCE_ENERGY_CORE, 1);
            break;
        case MONSTER_FOG_WORM:
            Player_AddResource(player, RESOURCE_CALM_MUSHROOM, 1);
            Player_AddResource(player, RESOURCE_PROTECTIVE_FIBER, 1);
            break;
        case MONSTER_RELIC_GUARD:
            Player_AddResource(player, RESOURCE_RELIC_FRAGMENT, 1);
            break;
        case MONSTER_FINAL_BOSS:
            Player_AddResource(player, RESOURCE_BOSS_SCALE, 3);
            Player_AddResource(player, RESOURCE_ENERGY_CORE, 1);
            tasks->bossDefeated = true;
            break;
        default:
            break;
    }
}

bool Tasks_HandleAttack(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    Monster *target;
    float damage;

    (void)map;

    target = FindAttackTarget(tasks, player);
    if (target == NULL) {
        WriteMessage(message, messageSize, "No target was hit.");
        return false;
    }

    if (player->stamina < STAMINA_ATTACK_COST) {
        WriteMessage(message, messageSize, "Not enough stamina to attack.");
        return false;
    }

    Player_ConsumeStamina(player, STAMINA_ATTACK_COST);
    damage = Player_GetAttackPower(player);
    if (target->type == MONSTER_FINAL_BOSS && player->gridX > target->gridX) {
        damage *= 2.0f;
    }
    if (tasks->monolithsLit > 0 && target->type == MONSTER_FINAL_BOSS) {
        damage *= 1.0f + 0.10f * tasks->monolithsLit;
    }

    target->health -= damage;
    if (target->health <= 0.0f) {
        target->active = false;
        DropMonsterRewards(tasks, player, target);
        if (target->type == MONSTER_FINAL_BOSS) {
            WriteMessage(message, messageSize, "The final boss is down. The Signal Tower is temporarily safe.");
        } else {
            WriteMessage(message, messageSize, "Target eliminated. Loot collected.");
        }
        if (tasks->stage == 5 && player->resources[RESOURCE_ENERGY_CORE] > 0) {
            Tasks_UpdateObjective(tasks, player);
        }
        return true;
    }

    if (target->type == MONSTER_THORN_LARVA && !player->hasLaserGun) {
        Player_ConsumeStamina(player, 1.0f);
    }

    WriteMessage(message, messageSize, "Attack connected.");
    return true;
}

static bool TryRepairAtShipCore(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    if (!tasks->communicatorUnlocked) {
        tasks->communicatorUnlocked = true;
    }

    if (tasks->stage == 1) {
        if (Player_HasResources(player, RESOURCE_WOOD, 3) && Player_HasResources(player, RESOURCE_METAL_SCRAP, 2)) {
            Player_SpendResource(player, RESOURCE_WOOD, 3);
            Player_SpendResource(player, RESOURCE_METAL_SCRAP, 2);
            tasks->oxygenRepairLevel = 1;
            UnlockStageIfNeeded(tasks, map, 2);
            Player_AddOxygen(player, 18.0f);
            WriteMessage(message, messageSize, "First oxygen module repaired. Loxi suggests crafting light gear before going deeper.");
            return true;
        }
        WriteMessage(message, messageSize, "Missing materials for the first oxygen repair: 3 Wood and 2 Metal Scrap.");
        return true;
    }

    if (tasks->stage == 2) {
        if (player->hasGlowStick && Player_HasResources(player, RESOURCE_GLOW_MOSS, 1) && Player_HasResources(player, RESOURCE_ORE, 1)) {
            Player_SpendResource(player, RESOURCE_GLOW_MOSS, 1);
            Player_SpendResource(player, RESOURCE_ORE, 1);
            tasks->oxygenRepairLevel = 2;
            UnlockStageIfNeeded(tasks, map, 3);
            Player_AddOxygen(player, 35.0f);
            WriteMessage(message, messageSize, "Oxygen system fully repaired. The outer swamp is now open.");
            return true;
        }
        WriteMessage(message, messageSize, "Final oxygen repair requires a Glow Stick, 1 Glow Moss, and 1 Ore.");
        return true;
    }

    if (tasks->stage == 6 && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
        Player_SpendResource(player, RESOURCE_RELIC_FRAGMENT, 3);
        tasks->amplifierUnlocked = true;
        UnlockStageIfNeeded(tasks, map, 7);
        WriteMessage(message, messageSize, "Loxi finished the endgame analysis: challenge the boss or craft the amplifier.");
        return true;
    }

    if (tasks->stage >= 7 && tasks->oxygenRepairLevel >= 2 && tasks->energyRepairLevel >= 1 && !tasks->bossDefeated && !player->hasSignalAmplifier) {
        tasks->ending = ENDING_SETTLEMENT;
        WriteMessage(message, messageSize, "You chose not to wait for rescue and turned the ship base into a new home.");
        return true;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE) {
        player->pressure = player->pressure > 30.0f ? 30.0f : player->pressure;
        Player_RecoverStamina(player, 20.0f);
        WriteMessage(message, messageSize, "A short rest at the ship base helped you recover.");
        return true;
    }

    return false;
}

static bool TryRoomInteraction(TaskSystem *tasks, Player *player, char *message, size_t messageSize) {
    const char *roomName;

    (void)tasks;

    roomName = Map_GetRoomNameAt(player->gridX, player->gridY);

    if (strcmp(roomName, "Left Cabin") == 0) {
        Player_RecoverStamina(player, 18.0f);
        Player_RelievePressure(player, 10.0f);
        WriteMessage(message, messageSize, "You took a short break in the left cabin and recovered.");
        return true;
    }

    if (strcmp(roomName, "Upper Cabin") == 0) {
        Player_ClearPoison(player);
        Player_RelievePressure(player, 16.0f);
        WriteMessage(message, messageSize, "The upper cabin support station cleared your poison and lowered stress.");
        return true;
    }

    if (strcmp(roomName, "Lower Cabin") == 0) {
        if (!player->hasLaserGun && tasks->stage >= 4) {
            WriteMessage(message, messageSize, "The lower cabin lockers are powered. Use the workbench in the hub to assemble a Laser Gun.");
        } else if (player->hasLaserGun) {
            WriteMessage(message, messageSize, "The lower cabin racks hold spare cells and training notes for field combat.");
        } else {
            WriteMessage(message, messageSize, "The lower cabin is locked down. Loxi suggests restoring more ship systems first.");
        }
        return true;
    }

    return false;
}

bool Tasks_HandleInteraction(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    ResourceNode *node;
    ShipLog *log;

    node = FindNearbyNode(tasks, player);
    if (node != NULL) {
        return CollectNode(tasks, player, node, message, messageSize);
    }

    log = FindNearbyLog(tasks, player);
    if (log != NULL) {
        GrantLogReward(tasks, player, log, message, messageSize);
        return true;
    }

    if (IsNearTile(player, SHIP_CORE_X, SHIP_CORE_Y)) {
        return TryRepairAtShipCore(tasks, map, player, message, messageSize);
    }

    if (IsNearTile(player, WORKBENCH_X, WORKBENCH_Y)) {
        WriteMessage(message, messageSize, "This is the workbench. Press Q to open crafting.");
        return true;
    }

    if (IsNearTile(player, AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y)) {
        if (tasks->stage < 3) {
            WriteMessage(message, messageSize, "The airlock is still sealed. Restore the oxygen system first.");
            return true;
        }

        if (Map_GetTileAt(map, AIRLOCK_DOOR_X, AIRLOCK_DOOR_Y) == TILE_AIRLOCK_DOOR) {
            Map_UnlockSwampOuter(map);
            WriteMessage(message, messageSize, "Airlock cycling complete. The outer blast door is now open.");
            return true;
        }

        WriteMessage(message, messageSize, "The airlock door is already open.");
        return true;
    }

    if (IsNearTile(player, COMM_RELAY_X, COMM_RELAY_Y)) {
        if (tasks->stage == 3) {
            if (Player_HasResources(player, RESOURCE_ALIEN_VINE, 2)
                && Player_HasResources(player, RESOURCE_SHELL_FRUIT, 2)
                && Player_HasResources(player, RESOURCE_SPECIAL_FUNGUS, 1)) {
                Player_SpendResource(player, RESOURCE_ALIEN_VINE, 2);
                Player_SpendResource(player, RESOURCE_SHELL_FRUIT, 2);
                Player_SpendResource(player, RESOURCE_SPECIAL_FUNGUS, 1);
                tasks->commRepairLevel = 1;
                UnlockStageIfNeeded(tasks, map, 4);
                WriteMessage(message, messageSize, "The comm relay is restored. Loxi can now guide you deeper into the world.");
                return true;
            }
            WriteMessage(message, messageSize, "Comm relay repair still needs 2 Vines, 2 Shell Fruit, and 1 Special Fungus.");
            return true;
        }

        WriteMessage(message, messageSize, "The comm relay is stable.");
        return true;
    }

    if (IsNearTile(player, CRASH_CLUE_X, CRASH_CLUE_Y)) {
        if (tasks->stage == 4 && player->hasLaserGun && player->hasProtectionSuit) {
            tasks->crashClueFound = true;
            UnlockStageIfNeeded(tasks, map, 5);
            WriteMessage(message, messageSize, "You found the crash clue. The deep swamp entrance is now unlocked.");
            return true;
        }

        WriteMessage(message, messageSize, "The wreck is leaking dangerous residue. Better prepare a weapon and suit first.");
        return true;
    }

    if (IsNearTile(player, ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y)) {
        if (tasks->stage == 5) {
            if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1)
                && Player_HasResources(player, RESOURCE_ORE, 2)
                && Player_HasResources(player, RESOURCE_ENERGY_CRYSTAL, 1)) {
                Player_SpendResource(player, RESOURCE_ENERGY_CORE, 1);
                Player_SpendResource(player, RESOURCE_ORE, 2);
                Player_SpendResource(player, RESOURCE_ENERGY_CRYSTAL, 1);
                tasks->energyRepairLevel = 1;
                UnlockStageIfNeeded(tasks, map, 6);
                WriteMessage(message, messageSize, "The power module is fully restored. The ruins entrance is now open.");
                return true;
            }
            WriteMessage(message, messageSize, "Power repair still needs 1 Energy Core, 2 Ore, and 1 Energy Crystal.");
            return true;
        }

        WriteMessage(message, messageSize, "The power console is operating normally.");
        return true;
    }

    if (IsNearTile(player, MONOLITH_A_X, MONOLITH_A_Y) || IsNearTile(player, MONOLITH_B_X, MONOLITH_B_Y) || IsNearTile(player, MONOLITH_C_X, MONOLITH_C_Y)) {
        int monolithIndex;

        monolithIndex = -1;
        if (IsNearTile(player, MONOLITH_A_X, MONOLITH_A_Y)) {
            monolithIndex = 0;
        } else if (IsNearTile(player, MONOLITH_B_X, MONOLITH_B_Y)) {
            monolithIndex = 1;
        } else if (IsNearTile(player, MONOLITH_C_X, MONOLITH_C_Y)) {
            monolithIndex = 2;
        }

        if (tasks->stage < 7) {
            WriteMessage(message, messageSize, "The monolith is still dormant. Loxi suggests finishing the main prep first.");
            return true;
        }
        
        if (!tasks->monolithPuzzle.active) {
            tasks->monolithPuzzle.active = true;
            tasks->monolithPuzzle.correctOrder[0] = 1;
            tasks->monolithPuzzle.correctOrder[1] = 0;
            tasks->monolithPuzzle.correctOrder[2] = 2;
            WriteMessage(message, messageSize, "The monolith hums to life! A puzzle mechanism has been activated. Try activating them in the correct sequence.");
            return true;
        }
        
        if (tasks->monolithPuzzle.solved) {
            WriteMessage(message, messageSize, "This monolith is already active and resonating with power.");
            return true;
        }
        
        if (Puzzle_TryActivate(&tasks->monolithPuzzle, monolithIndex)) {
            if (Puzzle_CheckSolved(&tasks->monolithPuzzle)) {
                tasks->monolithPuzzle.solved = true;
                tasks->monolithsLit = 3;
                tasks->monolithActivated[0] = true;
                tasks->monolithActivated[1] = true;
                tasks->monolithActivated[2] = true;
                WriteMessage(message, messageSize, "All three monoliths resonate in harmony! The final boss has been significantly weakened.");
            } else if (tasks->monolithPuzzle.currentStep < MAX_PUZZLE_STEPS) {
                WriteMessage(message, messageSize, "The monolith responds. The sequence continues...");
            } else {
                Puzzle_Reset(&tasks->monolithPuzzle);
                WriteMessage(message, messageSize, "The monoliths fall silent. The sequence was incorrect. Try again.");
            }
            return true;
        }

        WriteMessage(message, messageSize, "This monolith is already active.");
        return true;
    }

    if (IsNearTile(player, SIGNAL_TOWER_X, SIGNAL_TOWER_Y)) {
        if (tasks->stage < 7) {
            WriteMessage(message, messageSize, "The Signal Tower is still offline. Loxi suggests finishing the endgame prep first.");
            return true;
        }

        if (player->hasSignalAmplifier) {
            tasks->signalTowerActivated = true;
            tasks->ending = ENDING_PEACEFUL;
            WriteMessage(message, messageSize, "The Signal Amplifier took over the tower. The peaceful rescue route is open.");
            return true;
        }

        if (tasks->bossDefeated) {
            tasks->signalTowerActivated = true;
            tasks->ending = ENDING_HEROIC;
            WriteMessage(message, messageSize, "You manually activated the Signal Tower. The rescue beacon is on its way to Earth.");
            return true;
        }

        WriteMessage(message, messageSize, "A final threat remains near the tower unless you defeat the boss or bring the amplifier.");
        return true;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE && TryRoomInteraction(tasks, player, message, messageSize)) {
        return true;
    }

    if (map->campPlaced && DistanceManhattan(player->gridX, player->gridY, map->campX, map->campY) <= 1) {
        player->pressure = player->pressure > 45.0f ? 45.0f : player->pressure;
        Player_RecoverStamina(player, 14.0f);
        WriteMessage(message, messageSize, "You rested at the field camp and recovered a bit.");
        return true;
    }
    
    if (player->hasRope) {
        int targetX = player->gridX + player->facingX;
        int targetY = player->gridY + player->facingY;
        
        if (Map_CanCrossWithRope(map, player->gridX, player->gridY, targetX, targetY)) {
            Map_CreateRopeBridge(map, targetX, targetY);
            Player_SpendResource(player, RESOURCE_ALIEN_VINE, 0);
            WriteMessage(message, messageSize, "You used the rope to cross the hazardous terrain.");
            return true;
        }
    }

    WriteMessage(message, messageSize, "There is nothing useful to interact with here right now.");
    return false;
}

bool Tasks_TryCraft(TaskSystem *tasks, GameMap *map, Player *player, RecipeType recipe, char *message, size_t messageSize) {
    bool atWorkbench;

    atWorkbench = IsNearTile(player, WORKBENCH_X, WORKBENCH_Y);

    switch (recipe) {
        case RECIPE_GLOW_STICK:
            if (tasks->stage < 2) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (Player_HasResources(player, RESOURCE_WOOD, 1) && Player_HasResources(player, RESOURCE_GLOW_MOSS, 1)) {
                Player_SpendResource(player, RESOURCE_WOOD, 1);
                Player_SpendResource(player, RESOURCE_GLOW_MOSS, 1);
                player->hasGlowStick = true;
                player->glowStickTimer = 60.0f;
                WriteMessage(message, messageSize, "Crafted Glow Stick. Night visibility increased for a while.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 1 Wood + 1 Glow Moss.");
            return false;
        case RECIPE_ROPE:
            if (tasks->stage < 3) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (Player_HasResources(player, RESOURCE_WOOD, 1) && Player_HasResources(player, RESOURCE_ALIEN_VINE, 2)) {
                Player_SpendResource(player, RESOURCE_WOOD, 1);
                Player_SpendResource(player, RESOURCE_ALIEN_VINE, 2);
                player->hasRope = true;
                WriteMessage(message, messageSize, "Crafted Simple Rope. It can cross swamp tiles and unlock shortcuts.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 1 Wood + 2 Alien Vines.");
            return false;
        case RECIPE_REINFORCED_METAL:
            if (tasks->stage < 4) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (!atWorkbench || !Player_CanCraftAdvanced(player)) {
                WriteMessage(message, messageSize, "Advanced crafting requires the workbench and stress below 60.");
                return false;
            }
            if (Player_HasResources(player, RESOURCE_JUNK_METAL, 1) && Player_HasResources(player, RESOURCE_METAL_SCRAP, 2)) {
                Player_SpendResource(player, RESOURCE_JUNK_METAL, 1);
                Player_SpendResource(player, RESOURCE_METAL_SCRAP, 2);
                player->resources[RESOURCE_METAL_SCRAP] += 1;
                WriteMessage(message, messageSize, "Crafted Reinforced Metal for the Laser Gun.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 1 Junk Metal + 2 Metal Scrap.");
            return false;
        case RECIPE_LASER_GUN:
            if (tasks->stage < 4) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (!atWorkbench || !Player_CanCraftAdvanced(player)) {
                WriteMessage(message, messageSize, "The Laser Gun requires the workbench and manageable stress.");
                return false;
            }
            if (Player_HasResources(player, RESOURCE_METAL_SCRAP, 1) && Player_HasResources(player, RESOURCE_ORE, 2)) {
                Player_SpendResource(player, RESOURCE_METAL_SCRAP, 1);
                Player_SpendResource(player, RESOURCE_ORE, 2);
                player->hasLaserGun = true;
                WriteMessage(message, messageSize, "Laser Gun assembled. Ranged combat is now available.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 1 Reinforced Metal + 2 Ore.");
            return false;
        case RECIPE_PROTECTION_SUIT:
            if (tasks->stage < 5) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (!atWorkbench || !Player_CanCraftAdvanced(player)) {
                WriteMessage(message, messageSize, "The Protection Suit requires the workbench and manageable stress.");
                return false;
            }
            if (Player_HasResources(player, RESOURCE_WOOD, 1)
                && Player_HasResources(player, RESOURCE_ALIEN_VINE, 2)
                && Player_HasResources(player, RESOURCE_PROTECTIVE_FIBER, 1)) {
                Player_SpendResource(player, RESOURCE_WOOD, 1);
                Player_SpendResource(player, RESOURCE_ALIEN_VINE, 2);
                Player_SpendResource(player, RESOURCE_PROTECTIVE_FIBER, 1);
                player->hasProtectionSuit = true;
                WriteMessage(message, messageSize, "Protection Suit crafted. Damage and poison buildup are reduced.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 1 Wood + 2 Vines + 1 Protective Fiber.");
            return false;
        case RECIPE_SIGNAL_AMPLIFIER:
            if (!tasks->amplifierUnlocked) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (!atWorkbench || !Player_CanCraftAdvanced(player)) {
                WriteMessage(message, messageSize, "The Signal Amplifier must be crafted at the workbench with low stress.");
                return false;
            }
            if (Player_HasResources(player, RESOURCE_ENERGY_CORE, 1) && Player_HasResources(player, RESOURCE_RELIC_FRAGMENT, 3)) {
                Player_SpendResource(player, RESOURCE_ENERGY_CORE, 1);
                Player_SpendResource(player, RESOURCE_RELIC_FRAGMENT, 3);
                player->hasSignalAmplifier = true;
                WriteMessage(message, messageSize, "Signal Amplifier completed. You can now bypass the final boss.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 1 Energy Core + 3 Relic Fragments.");
            return false;
        case RECIPE_FIELD_CAMP:
            if (tasks->stage < BASE_CAMP_UNLOCK_STAGE) {
                WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
                return false;
            }
            if (Player_HasResources(player, RESOURCE_WOOD, 5)) {
                Player_SpendResource(player, RESOURCE_WOOD, 5);
                player->hasFieldCamp = true;
                Map_SetFieldCamp(map, player->gridX, player->gridY);
                WriteMessage(message, messageSize, "Field Camp built. You can rest outdoors now.");
                return true;
            }
            WriteMessage(message, messageSize, "Missing materials: 5 Wood.");
            return false;
        case RECIPE_COUNT:
        default:
            WriteMessage(message, messageSize, "Unknown recipe.");
            return false;
    }
}

bool Tasks_IsBlockingActorTile(const TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && tasks->stage >= monster->unlockStage && monster->gridX == gridX && monster->gridY == gridY) {
            return true;
        }
    }

    return false;
}

bool Tasks_GetObjectiveMarker(const TaskSystem *tasks, const Player *player, int *gridX, int *gridY) {
    switch (tasks->stage) {
        case 1:
        case 2:
        case 6:
            *gridX = SHIP_CORE_X;
            *gridY = SHIP_CORE_Y;
            return true;
        case 3:
            if (Map_GetAreaAt(player->gridX, player->gridY) == MAP_AREA_BASE) {
                *gridX = AIRLOCK_CONSOLE_X;
                *gridY = AIRLOCK_CONSOLE_Y;
            } else {
                *gridX = COMM_RELAY_X;
                *gridY = COMM_RELAY_Y;
            }
            return true;
        case 4:
            *gridX = CRASH_CLUE_X;
            *gridY = CRASH_CLUE_Y;
            return true;
        case 5:
            *gridX = ENERGY_CONSOLE_X;
            *gridY = ENERGY_CONSOLE_Y;
            return true;
        case 7:
            *gridX = SIGNAL_TOWER_X;
            *gridY = SIGNAL_TOWER_Y;
            return true;
        default:
            return false;
    }
}

static void DrawResourceNode(const ResourceNode *node, float elapsedSeconds) {
    Rectangle rect;
    Color glow;

    rect = Map_GridToRect(node->gridX, node->gridY);
    glow = (Color){190, 234, 255, (unsigned char)(90 + 40.0f * (sinf(elapsedSeconds * 4.0f) * 0.5f + 0.5f))};

    switch (node->type) {
        case RESOURCE_WOOD:
            DrawRectangle((int)rect.x + 24, (int)rect.y + 18, 16, 32, (Color){113, 78, 49, 255});
            break;
        case RESOURCE_METAL_SCRAP:
        case RESOURCE_JUNK_METAL:
            DrawRectangle((int)rect.x + 18, (int)rect.y + 20, 28, 20, (Color){145, 152, 164, 255});
            break;
        case RESOURCE_FRUIT:
        case RESOURCE_SHELL_FRUIT:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 30), 10.0f, (Color){221, 173, 89, 255});
            break;
        case RESOURCE_GLOW_MOSS:
        case RESOURCE_CALM_MUSHROOM:
        case RESOURCE_SPECIAL_FUNGUS:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 32), 12.0f, (Color){126, 255, 208, 170});
            break;
        case RESOURCE_ENERGY_CORE:
        case RESOURCE_ENERGY_CRYSTAL:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 28), 12.0f, (Color){110, 214, 255, 190});
            DrawCircleLines((int)(rect.x + 32), (int)(rect.y + 28), 15.0f, glow);
            break;
        case RESOURCE_RELIC_FRAGMENT:
        case RESOURCE_ALIEN_SLIME:
            DrawTriangle(
                (Vector2){rect.x + 22.0f, rect.y + 42.0f},
                (Vector2){rect.x + 32.0f, rect.y + 18.0f},
                (Vector2){rect.x + 44.0f, rect.y + 42.0f},
                (Color){184, 194, 211, 255}
            );
            break;
        default:
            DrawCircle((int)(rect.x + 32), (int)(rect.y + 32), 10.0f, glow);
            break;
    }
}

static void DrawMonster(const Monster *monster, const AssetBundle *assets, float elapsedSeconds, int bossDebuffStacks) {
    Vector2 pos;
    Color body;
    float radius;

    pos = Map_GridToWorld(monster->gridX, monster->gridY);
    body = (Color){255, 138, 112, 255};
    radius = 12.0f;

    switch (monster->type) {
        case MONSTER_THORN_LARVA:
            body = (Color){197, 110, 81, 255};
            radius = 10.0f;
            break;
        case MONSTER_WING_BUG:
            body = (Color){160, 217, 132, 255};
            radius = 8.0f;
            break;
        case MONSTER_RAPTOR:
            body = (Color){211, 121, 90, 255};
            radius = 13.0f;
            break;
        case MONSTER_SWAMP_STALKER:
            body = (Color){96, 150, 103, 255};
            radius = 12.0f;
            break;
        case MONSTER_SENTINEL_JELLY:
            body = (Color){101, 207, 242, 255};
            radius = 13.0f;
            break;
        case MONSTER_FOG_WORM:
            body = (Color){173, 214, 86, 255};
            radius = 12.0f;
            break;
        case MONSTER_RELIC_GUARD:
            body = (Color){173, 176, 194, 255};
            radius = 15.0f;
            break;
        case MONSTER_FINAL_BOSS:
            if (assets->boss.loaded) {
                Rectangle source;
                Rectangle dest;
                float drawSize;

                source = (Rectangle){0.0f, 0.0f, (float)assets->boss.texture.width, (float)assets->boss.texture.height};
                drawSize = 54.0f;
                dest = (Rectangle){pos.x - drawSize * 0.5f, pos.y - drawSize * 0.5f, drawSize, drawSize};
                DrawTexturePro(assets->boss.texture, source, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            } else {
                body = (Color){164, 94, 93, 255};
                radius = 20.0f;
                DrawCircleV(pos, radius + sinf(elapsedSeconds * 3.0f), (Color){255, 163, 131, 45});
            }
            if (bossDebuffStacks > 0) {
                DrawRing(pos, 24.0f, 30.0f, 0.0f, 360.0f, 32, (Color){107, 242, 255, 50 + bossDebuffStacks * 20});
            }
            if (!assets->boss.loaded) {
                DrawCircleV(pos, radius, body);
            }
            DrawRectangle((int)(pos.x - 18.0f), (int)(pos.y - 28.0f), 36, 5, (Color){44, 22, 22, 255});
            DrawRectangle((int)(pos.x - 18.0f), (int)(pos.y - 28.0f), (int)(36.0f * (monster->health / monster->maxHealth)), 5, (Color){245, 94, 81, 255});
            return;
        default:
            break;
    }

    DrawCircleV(pos, radius, body);
    DrawCircle((int)pos.x, (int)(pos.y - radius * 0.6f), radius * 0.55f + sinf(elapsedSeconds * 4.0f) * 1.2f, (Color){255, 255, 255, 40});
}

void Tasks_DrawWorld(const TaskSystem *tasks, const AssetBundle *assets, float elapsedSeconds) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->active) {
            DrawResourceNode(node, elapsedSeconds);
        }
    }

    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;
        Vector2 pos;

        log = &tasks->logs[index];
        if (!log->active || log->collected) {
            continue;
        }

        pos = Map_GridToWorld(log->gridX, log->gridY);
        DrawRing(pos, 10.0f, 18.0f, 0.0f, 360.0f, 24, (Color){120, 185, 255, 90});
        DrawCircleV(pos, 9.0f, (Color){92, 157, 255, 180});
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && tasks->stage >= monster->unlockStage) {
            DrawMonster(monster, assets, elapsedSeconds, tasks->monolithsLit);
        }
    }
}

int Tasks_GetVisibleRecipeCount(const TaskSystem *tasks) {
    static const RecipeType allRecipes[] = {
        RECIPE_GLOW_STICK,
        RECIPE_ROPE,
        RECIPE_FIELD_CAMP,
        RECIPE_REINFORCED_METAL,
        RECIPE_LASER_GUN,
        RECIPE_PROTECTION_SUIT,
        RECIPE_SIGNAL_AMPLIFIER
    };
    int count;
    int itemIndex;

    count = 0;
    for (itemIndex = 0; itemIndex < (int)(sizeof(allRecipes) / sizeof(allRecipes[0])); itemIndex++) {
        RecipeType recipe;

        recipe = allRecipes[itemIndex];
        if (recipe == RECIPE_GLOW_STICK && tasks->stage < 2) {
            continue;
        }
        if ((recipe == RECIPE_ROPE || recipe == RECIPE_FIELD_CAMP) && tasks->stage < 3) {
            continue;
        }
        if ((recipe == RECIPE_REINFORCED_METAL || recipe == RECIPE_LASER_GUN) && tasks->stage < 4) {
            continue;
        }
        if (recipe == RECIPE_PROTECTION_SUIT && tasks->stage < 5) {
            continue;
        }
        if (recipe == RECIPE_SIGNAL_AMPLIFIER && !tasks->amplifierUnlocked) {
            continue;
        }

        count += 1;
    }

    return count;
}

RecipeType Tasks_GetVisibleRecipeAt(const TaskSystem *tasks, int index) {
    static const RecipeType earlyRecipes[] = {
        RECIPE_GLOW_STICK,
        RECIPE_ROPE,
        RECIPE_FIELD_CAMP,
        RECIPE_REINFORCED_METAL,
        RECIPE_LASER_GUN,
        RECIPE_PROTECTION_SUIT,
        RECIPE_SIGNAL_AMPLIFIER
    };
    int visibleIndex;
    int itemIndex;

    visibleIndex = 0;
    for (itemIndex = 0; itemIndex < (int)(sizeof(earlyRecipes) / sizeof(earlyRecipes[0])); itemIndex++) {
        RecipeType recipe;

        recipe = earlyRecipes[itemIndex];
        if (recipe == RECIPE_ROPE || recipe == RECIPE_FIELD_CAMP) {
            if (tasks->stage < 3) {
                continue;
            }
        } else if (recipe == RECIPE_REINFORCED_METAL || recipe == RECIPE_LASER_GUN) {
            if (tasks->stage < 4) {
                continue;
            }
        } else if (recipe == RECIPE_PROTECTION_SUIT) {
            if (tasks->stage < 5) {
                continue;
            }
        } else if (recipe == RECIPE_SIGNAL_AMPLIFIER) {
            if (!tasks->amplifierUnlocked) {
                continue;
            }
        } else if (recipe == RECIPE_GLOW_STICK && tasks->stage < 2) {
            continue;
        }

        if (visibleIndex == index) {
            return recipe;
        }
        visibleIndex += 1;
    }

    return RECIPE_GLOW_STICK;
}

const char *Tasks_GetStageName(int stage) {
    switch (stage) {
        case 1:
            return "Stage 1: Wake Up";
        case 2:
            return "Stage 2: First Steps";
        case 3:
            return "Stage 3: Into the Wild";
        case 4:
            return "Stage 4: Rising Risk";
        case 5:
            return "Stage 5: Power Breakthrough";
        case 6:
            return "Stage 6: Final Preparation";
        case 7:
            return "Stage 7: Final Choice";
        default:
            return "Unknown Stage";
    }
}

const char *Tasks_GetPhaseName(DayPhase phase) {
    switch (phase) {
        case DAY_PHASE_DAY:
            return "Day";
        case DAY_PHASE_DUSK:
            return "Dusk";
        case DAY_PHASE_NIGHT:
            return "Night";
        default:
            return "Unknown";
    }
}

const char *Tasks_GetEventName(EventType eventType) {
    switch (eventType) {
        case EVENT_HARVEST:
            return "Resource Surge";
        case EVENT_CALM_BEASTS:
            return "Quiet Creatures";
        case EVENT_CLEAR_SKY:
            return "Clear Skies";
        case EVENT_SPORE_STORM:
            return "Spore Storm";
        case EVENT_MONSTER_FRENZY:
            return "Monster Frenzy";
        case EVENT_DEVICE_FAULT:
            return "Device Fault";
        default:
            return "None";
    }
}

bool Tasks_IsCommunicatorUnlocked(const TaskSystem *tasks) {
    return tasks->communicatorUnlocked || tasks->oxygenRepairLevel > 0 || tasks->stage > 1;
}

const char *Tasks_GetCommunicatorHint(const TaskSystem *tasks) {
    return tasks->communicator;
}

GameEnding Tasks_GetEnding(const TaskSystem *tasks) {
    return tasks->ending;
}

const char *Tasks_GetEndingTitle(GameEnding ending) {
    switch (ending) {
        case ENDING_HEROIC:
            return "Heroic Rescue";
        case ENDING_PEACEFUL:
            return "Peaceful Rescue";
        case ENDING_SETTLEMENT:
            return "Alien Settlement";
        case ENDING_FAILURE:
            return "Failed Survival";
        case ENDING_NONE:
        default:
            return "";
    }
}

const char *Tasks_GetEndingBody(GameEnding ending) {
    switch (ending) {
        case ENDING_HEROIC:
            return "You defeated the final threat and manually activated the Signal Tower. Loxi confirmed the rescue beacon was sent.";
        case ENDING_PEACEFUL:
            return "You avoided the final battle and stabilized the tower with the Signal Amplifier, leaving the alien ecosystem undisturbed.";
        case ENDING_SETTLEMENT:
            return "You gave up on uncertain rescue and transformed the ship base into a long-term home.";
        case ENDING_FAILURE:
            return "Repeated collapses and total resource loss ended the survival attempt. Even Loxi could not pull you back this time.";
        case ENDING_NONE:
        default:
            return "";
    }
}
