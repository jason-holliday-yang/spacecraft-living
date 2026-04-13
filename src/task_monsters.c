#include "task_runtime_internal.h"

#include "task_content.h"

#include <stdlib.h>

static float GetSuitFilterStrength(const Player *player) {
    return player->hasProtectionSuit ? 0.28f : 0.0f;
}

static float GetPlayerFilterStrength(const Player *player) {
    const PlayerStatusEffect *filteredEffect;
    float filterStrength;

    filterStrength = GetSuitFilterStrength(player);
    filteredEffect = Player_GetStatusEffect(player, PLAYER_STATUS_FILTERED);
    if (filteredEffect != NULL && filteredEffect->active && filteredEffect->magnitude > filterStrength) {
        filterStrength = filteredEffect->magnitude;
    }

    if (filterStrength > 0.65f) {
        filterStrength = 0.65f;
    }
    return filterStrength;
}

static float GetBossWeakeningFactor(const TaskSystem *tasks) {
    float factor;

    factor = 1.0f;
    if (tasks != NULL) {
        factor -= 0.08f * (float)tasks->monolithsLit;
    }
    if (factor < 0.76f) {
        factor = 0.76f;
    }
    return factor;
}

static void ApplyOxygenLeakStatus(Player *player, int level, float duration, float magnitude) {
    if (player == NULL || magnitude <= 0.0f || duration <= 0.0f) {
        return;
    }

    Player_SetStatus(player, PLAYER_STATUS_OXYGEN_LEAK, level, duration, magnitude);
}

static bool MonsterCanAct(const Monster *monster, const TaskSystem *tasks) {
    return monster->active && tasks->stage >= monster->unlockStage;
}

static int GetMonsterLeashRadius(const Monster *monster) {
    if (monster == NULL) {
        return 0;
    }

    switch (monster->type) {
        case MONSTER_FINAL_BOSS:
            return 10;
        case MONSTER_RELIC_GUARD:
            return 6;
        default:
            return 5;
    }
}

static bool IsTileOccupiedByMonster(const TaskSystem *tasks, const Monster *self, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *other;

        other = &tasks->monsters[index];
        if (!other->active || other == self) {
            continue;
        }

        if (TasksRuntime_IsMonsterOccupyingTile(other, gridX, gridY)) {
            return true;
        }
    }

    return false;
}

static bool IsMonsterFootprintValidForArea(const Monster *monster, const GameMap *map, int originX, int originY) {
    int offsetY;
    int offsetX;

    if (monster == NULL || map == NULL) {
        return false;
    }

    for (offsetY = 0; offsetY < MONSTER_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < MONSTER_FOOTPRINT_SIZE; offsetX++) {
            int gridX;
            int gridY;

            gridX = originX + offsetX;
            gridY = originY + offsetY;
            if (!Map_IsWithinBounds(gridX, gridY)) {
                return false;
            }
            if (Map_GetAreaAt(gridX, gridY) != monster->area) {
                return false;
            }
            if (!Map_IsWalkable(map, gridX, gridY)) {
                return false;
            }
        }
    }

    return true;
}

static bool CanMonsterOccupy(const TaskSystem *tasks, const Monster *self, const GameMap *map, int originX, int originY) {
    int offsetY;
    int offsetX;
    int leashRadius;
    int deltaX;
    int deltaY;

    if (!IsMonsterFootprintValidForArea(self, map, originX, originY)) {
        return false;
    }

    leashRadius = GetMonsterLeashRadius(self);
    deltaX = originX - self->spawnX;
    deltaY = originY - self->spawnY;
    if (deltaX < 0) {
        deltaX = -deltaX;
    }
    if (deltaY < 0) {
        deltaY = -deltaY;
    }
    if (deltaX + deltaY > leashRadius) {
        return false;
    }

    for (offsetY = 0; offsetY < MONSTER_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < MONSTER_FOOTPRINT_SIZE; offsetX++) {
            int gridX;
            int gridY;

            gridX = originX + offsetX;
            gridY = originY + offsetY;
            if (IsTileOccupiedByMonster(tasks, self, gridX, gridY)) {
                return false;
            }
        }
    }

    return true;
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
    if (stepX != 0 && CanMonsterOccupy(tasks, monster, map, tryX, tryY)) {
        monster->gridX = tryX;
        monster->gridY = tryY;
        return;
    }

    tryX = monster->gridX;
    tryY = monster->gridY + stepY;
    if (stepY != 0 && CanMonsterOccupy(tasks, monster, map, tryX, tryY)) {
        monster->gridX = tryX;
        monster->gridY = tryY;
    }
}

static void ApplyMonsterAttack(const TaskSystem *tasks, MonsterType type, Player *player) {
    float damage;
    float pressure;
    float filterStrength;
    float poisonAmount;
    float leakMagnitude;
    TaskMonsterSpec monsterSpec;

    if (TasksContent_GetMonsterSpec(type, &monsterSpec)) {
        damage = monsterSpec.attackDamage;
        pressure = monsterSpec.attackPressure;
    } else {
        damage = 4.0f;
        pressure = 6.0f;
    }

    filterStrength = GetPlayerFilterStrength(player);
    poisonAmount = 0.0f;
    leakMagnitude = 0.0f;

    if (player->hasProtectionSuit) {
        damage *= 0.70f;
        pressure *= 0.60f;
    }

    if (tasks->currentEvent == EVENT_MONSTER_FRENZY) {
        damage *= 1.25f;
        pressure *= 1.15f;
    }

    if (type == MONSTER_FINAL_BOSS) {
        const float bossWeakeningFactor = GetBossWeakeningFactor(tasks);
        damage *= bossWeakeningFactor;
        pressure *= bossWeakeningFactor;
    }

    Player_DamageHealth(player, damage);
    Player_AddPressure(player, pressure * 0.45f);

    switch (type) {
        case MONSTER_WING_BUG:
            leakMagnitude = 0.22f * (1.0f - filterStrength * 0.55f);
            ApplyOxygenLeakStatus(player, 1, 4.5f, leakMagnitude);
            break;
        case MONSTER_SWAMP_STALKER:
            poisonAmount = 6.5f * (1.0f - filterStrength * 0.80f);
            Player_AddPoison(player, poisonAmount);
            break;
        case MONSTER_SENTINEL_JELLY:
            leakMagnitude = 0.48f * (1.0f - filterStrength * 0.70f);
            ApplyOxygenLeakStatus(player, 1, 8.0f, leakMagnitude);
            break;
        case MONSTER_FOG_WORM:
            poisonAmount = 8.0f * (1.0f - filterStrength * 0.75f);
            Player_AddPoison(player, poisonAmount);
            leakMagnitude = 0.75f * (1.0f - filterStrength * 0.55f);
            ApplyOxygenLeakStatus(player, 2, 10.0f, leakMagnitude);
            break;
        case MONSTER_RELIC_GUARD:
            leakMagnitude = 0.84f * (1.0f - filterStrength * 0.55f);
            ApplyOxygenLeakStatus(player, 2, 10.0f, leakMagnitude);
            Player_DamageOxygen(player, 4.0f * (1.0f - filterStrength * 0.45f));
            break;
        case MONSTER_FINAL_BOSS:
            poisonAmount = 5.5f * (1.0f - filterStrength * 0.90f);
            poisonAmount *= GetBossWeakeningFactor(tasks);
            Player_AddPoison(player, poisonAmount);
            leakMagnitude = 0.82f * (1.0f - filterStrength * 0.75f);
            leakMagnitude *= GetBossWeakeningFactor(tasks);
            ApplyOxygenLeakStatus(player, 2, 11.0f, leakMagnitude);
            Player_DamageOxygen(player, 6.0f * GetBossWeakeningFactor(tasks) * (1.0f - filterStrength * 0.55f));
            break;
        case MONSTER_THORN_LARVA:
        case MONSTER_RAPTOR:
        default:
            break;
    }
}

static void TrySpawnBossGuards(TaskSystem *tasks, const GameMap *map) {
    (void)tasks;
    (void)map;
}

static void ExecuteBossAttack(TaskSystem *tasks, Monster *boss, Player *player, const GameMap *map) {
    (void)map;

    switch (boss->currentAttack) {
        case BOSS_ATTACK_MELEE: {
            if (TasksRuntime_GetDistanceToMonster(boss, player->gridX, player->gridY) == 1) {
                ApplyMonsterAttack(tasks, boss->type, player);
            }
            break;
        }
        case BOSS_ATTACK_CHARGE: {
            int steps;
            int x;
            int y;
            int dirX;
            int dirY;

            steps = 0;
            x = boss->gridX;
            y = boss->gridY;
            dirX = (player->gridX > x) ? 1 : (player->gridX < x) ? -1 : 0;
            dirY = (player->gridY > y) ? 1 : (player->gridY < y) ? -1 : 0;

            while (steps < 5 && CanMonsterOccupy(tasks, boss, map, x + dirX, y + dirY)) {
                x += dirX;
                y += dirY;
                steps++;

                if (player->gridX >= x && player->gridX < x + MONSTER_FOOTPRINT_SIZE
                    && player->gridY >= y && player->gridY < y + MONSTER_FOOTPRINT_SIZE) {
                    ApplyMonsterAttack(tasks, boss->type, player);
                    break;
                }
            }
            boss->gridX = x;
            boss->gridY = y;
            break;
        }
        case BOSS_ATTACK_AOE: {
            int dist;

            dist = TasksRuntime_GetDistanceToMonster(boss, player->gridX, player->gridY);
            if (dist <= 3) {
                float aoeDamage;
                float aoePressure;
                float bossWeakeningFactor;

                aoeDamage = 8.5f;
                aoePressure = 12.0f;
                bossWeakeningFactor = GetBossWeakeningFactor(tasks);

                if (player->hasProtectionSuit) {
                    aoeDamage *= 0.70f;
                    aoePressure *= 0.60f;
                }

                Player_DamageHealth(player, aoeDamage * bossWeakeningFactor);
                Player_AddPressure(player, aoePressure * bossWeakeningFactor * 0.45f);
            }
            break;
        }
        default:
            break;
    }

    boss->currentAttack = BOSS_ATTACK_NONE;
    boss->attackTimer = 1.5f;
}

static void ChooseBossAttack(TaskSystem *tasks, Monster *boss, Player *player, const GameMap *map) {
    float healthPercent;
    int attackChoice;

    healthPercent = boss->health / boss->maxHealth;
    boss->targetX = player->gridX;
    boss->targetY = player->gridY;

    if (healthPercent > 0.70f) {
        attackChoice = rand() % 3;
        switch (attackChoice) {
            case 0:
                boss->currentAttack = BOSS_ATTACK_MELEE;
                boss->attackTelegraph = 1.10f;
                break;
            case 1:
                boss->currentAttack = BOSS_ATTACK_CHARGE;
                boss->attackTelegraph = 1.70f;
                break;
            case 2:
                boss->currentAttack = BOSS_ATTACK_AOE;
                boss->attackTelegraph = 2.20f;
                break;
        }
    } else {
        attackChoice = rand() % 4;
        switch (attackChoice) {
            case 0:
                boss->currentAttack = BOSS_ATTACK_MELEE;
                boss->attackTelegraph = 1.00f;
                break;
            case 1:
                boss->currentAttack = BOSS_ATTACK_CHARGE;
                boss->attackTelegraph = 1.40f;
                break;
            case 2:
                boss->currentAttack = BOSS_ATTACK_SPAWN;
                boss->attackTelegraph = 1.60f;
                if (!boss->phaseTriggered) {
                    boss->phaseTriggered = true;
                    TrySpawnBossGuards(tasks, map);
                }
                break;
            case 3:
                boss->currentAttack = BOSS_ATTACK_AOE;
                boss->attackTelegraph = 1.70f;
                break;
        }
    }
}

void TasksRuntime_UpdateMonsters(TaskSystem *tasks, const GameMap *map, Player *player, float deltaTime) {
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
            TrySpawnBossGuards(tasks, map);
        }

        if (monster->type == MONSTER_FINAL_BOSS && monster->attackTelegraph > 0.0f) {
            monster->attackTelegraph -= deltaTime;
            if (monster->attackTelegraph <= 0.0f) {
                ExecuteBossAttack(tasks, monster, player, map);
            }
            continue;
        }

        sameArea = monster->area == playerArea;
        hiddenByStealth = player->crouching
            && playerArea == MAP_AREA_FOREST
            && TasksRuntime_GetDistanceToMonster(monster, player->gridX, player->gridY) > 1;
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

        if ((float)TasksRuntime_GetDistanceToMonster(monster, player->gridX, player->gridY) > aggroRange) {
            continue;
        }

        if (TasksRuntime_GetDistanceToMonster(monster, player->gridX, player->gridY) == 1) {
            if (monster->attackTimer <= 0.0f) {
                if (monster->type == MONSTER_FINAL_BOSS && monster->currentAttack == BOSS_ATTACK_NONE) {
                    ChooseBossAttack(tasks, monster, player, map);
                } else {
                    ApplyMonsterAttack(tasks, monster->type, player);
                    monster->attackTimer = monster->type == MONSTER_FINAL_BOSS ? 1.2f : 0.9f;
                }
            }
            continue;
        }

        if (monster->moveTimer <= 0.0f) {
            MoveMonsterToward(tasks, monster, map, player->gridX, player->gridY);
            monster->moveTimer = monster->type == MONSTER_FINAL_BOSS ? 0.45f : 0.70f;
        }
    }
}
