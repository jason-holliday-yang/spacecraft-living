#include "task_runtime_internal.h"

#include "task_content.h"

#include <stdlib.h>

static const float kMonsterDisengageRecoveryDelay = 6.0f;

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
        factor -= 0.05f * (float)tasks->monolithsLit;
    }
    if (factor < 0.84f) {
        factor = 0.84f;
    }
    return factor;
}

static int GetBossPhase(const Monster *boss) {
    const float healthPercent = boss != NULL && boss->maxHealth > 0.0f ? boss->health / boss->maxHealth : 1.0f;

    if (healthPercent > 0.70f) {
        return 1;
    }
    if (healthPercent > 0.35f) {
        return 2;
    }
    return 3;
}

static float GetBossMoveInterval(const Monster *boss) {
    const int phase = GetBossPhase(boss);

    switch (phase) {
        case 1:
            return 0.78f;
        case 2:
            return 0.62f;
        case 3:
        default:
            return 0.48f;
    }
}

static float GetBossTelegraphScale(const TaskSystem *tasks) {
    float scale = 1.0f;

    if (tasks != NULL) {
        scale += 0.08f * (float)tasks->monolithsLit;
    }
    if (scale > 1.24f) {
        scale = 1.24f;
    }
    return scale;
}

static bool IsMonsterDamaged(const Monster *monster) {
    return monster != NULL
        && monster->active
        && monster->maxHealth > 0.0f
        && monster->health < monster->maxHealth - 0.05f;
}

static void UpdateMonsterDisengageRecovery(Monster *monster, bool disengaged, float deltaTime) {
    if (!IsMonsterDamaged(monster)) {
        if (monster != NULL) {
            monster->disengageTimer = 0.0f;
        }
        return;
    }

    if (!disengaged || deltaTime <= 0.0f) {
        monster->disengageTimer = 0.0f;
        return;
    }

    monster->disengageTimer += deltaTime;
    if (monster->disengageTimer >= kMonsterDisengageRecoveryDelay) {
        monster->health = monster->maxHealth;
        monster->disengageTimer = 0.0f;
        monster->attackTelegraph = 0.0f;
        monster->currentAttack = BOSS_ATTACK_NONE;
        monster->recoverTimer = 0.0f;
    }
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

static float GetMonsterWindupDuration(MonsterType type) {
    switch (type) {
        case MONSTER_WING_BUG:
            return 0.18f;
        case MONSTER_THORN_LARVA:
            return 0.24f;
        case MONSTER_SWAMP_STALKER:
            return 0.30f;
        case MONSTER_SENTINEL_JELLY:
            return 0.42f;
        case MONSTER_FOG_WORM:
            return 0.48f;
        case MONSTER_RELIC_GUARD:
            return 0.54f;
        case MONSTER_RAPTOR:
            return 0.24f;
        case MONSTER_FINAL_BOSS:
            return 0.0f;
        default:
            return 0.28f;
    }
}

static float GetMonsterRecoverDuration(MonsterType type) {
    switch (type) {
        case MONSTER_WING_BUG:
            return 0.14f;
        case MONSTER_THORN_LARVA:
            return 0.18f;
        case MONSTER_SWAMP_STALKER:
            return 0.24f;
        case MONSTER_SENTINEL_JELLY:
            return 0.30f;
        case MONSTER_FOG_WORM:
            return 0.34f;
        case MONSTER_RELIC_GUARD:
            return 0.40f;
        case MONSTER_RAPTOR:
            return 0.20f;
        case MONSTER_FINAL_BOSS:
            return 0.0f;
        default:
            return 0.20f;
    }
}

static float GetMonsterAttackInterval(MonsterType type) {
    switch (type) {
        case MONSTER_WING_BUG:
            return 0.72f;
        case MONSTER_THORN_LARVA:
            return 0.76f;
        case MONSTER_RAPTOR:
            return 0.78f;
        case MONSTER_SWAMP_STALKER:
            return 0.84f;
        case MONSTER_RELIC_GUARD:
            return 1.00f;
        case MONSTER_SENTINEL_JELLY:
        case MONSTER_FOG_WORM:
            return 0.92f;
        case MONSTER_FINAL_BOSS:
            return 1.20f;
        default:
            return 0.82f;
    }
}

static float GetMonsterMoveInterval(MonsterType type) {
    switch (type) {
        case MONSTER_WING_BUG:
            return 0.50f;
        case MONSTER_THORN_LARVA:
            return 0.58f;
        case MONSTER_RAPTOR:
            return 0.56f;
        case MONSTER_SWAMP_STALKER:
            return 0.62f;
        case MONSTER_SENTINEL_JELLY:
            return 0.66f;
        case MONSTER_FOG_WORM:
            return 0.64f;
        case MONSTER_RELIC_GUARD:
            return 0.68f;
        case MONSTER_FINAL_BOSS:
            return 0.0f;
        default:
            return 0.60f;
    }
}

static float GetMonsterAggroRange(const Monster *monster, const TaskSystem *tasks, const Player *player) {
    float aggroRange;

    (void)player;

    if (monster == NULL) {
        return 0.0f;
    }

    aggroRange = 6.0f;
    switch (monster->type) {
        case MONSTER_FINAL_BOSS:
            aggroRange = 10.0f;
            break;
        case MONSTER_RELIC_GUARD:
            aggroRange = 7.5f;
            break;
        case MONSTER_SENTINEL_JELLY:
        case MONSTER_FOG_WORM:
            aggroRange = 6.8f;
            break;
        case MONSTER_SWAMP_STALKER:
            aggroRange = tasks != NULL && tasks->phase != DAY_PHASE_DAY ? 7.5f : 6.5f;
            break;
        case MONSTER_WING_BUG:
            aggroRange = tasks != NULL && tasks->phase == DAY_PHASE_DAY ? 5.5f : 6.2f;
            break;
        case MONSTER_RAPTOR:
            aggroRange = 6.8f;
            break;
        case MONSTER_THORN_LARVA:
        default:
            break;
    }

    return aggroRange;
}

static float GetBossAttackInterval(const Monster *boss) {
    const int phase = GetBossPhase(boss);

    switch (phase) {
        case 1:
            return 1.45f;
        case 2:
            return 1.20f;
        case 3:
        default:
            return 1.00f;
    }
}

static void StartNormalMonsterAttackTelegraph(Monster *monster) {
    if (monster == NULL || monster->type == MONSTER_FINAL_BOSS) {
        return;
    }

    monster->attackTelegraph = GetMonsterWindupDuration(monster->type);
}

static int GetMonsterLeashRadius(const Monster *monster) {
    if (monster == NULL) {
        return 0;
    }

    switch (monster->type) {
        case MONSTER_FINAL_BOSS:
            return 12;
        case MONSTER_RELIC_GUARD:
            return 8;
        default:
            return 7;
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
    float filterStrength;
    float poisonAmount;
    float leakMagnitude;
    TaskMonsterSpec monsterSpec;

    if (TasksContent_GetMonsterSpec(type, &monsterSpec)) {
        damage = monsterSpec.attackDamage;
    } else {
        damage = 4.0f;
    }

    filterStrength = GetPlayerFilterStrength(player);
    poisonAmount = 0.0f;
    leakMagnitude = 0.0f;

    if (player->hasProtectionSuit) {
        damage *= 0.70f;
    }

    if (tasks->currentEvent == EVENT_MONSTER_FRENZY) {
        damage *= 1.25f;
    }

    if (type == MONSTER_FINAL_BOSS) {
        const float bossWeakeningFactor = GetBossWeakeningFactor(tasks);
        damage *= bossWeakeningFactor;
    }

    Player_DamageHealth(player, damage);

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
            Player_DamageOxygen(player, 9.0f * GetBossWeakeningFactor(tasks) * (1.0f - filterStrength * 0.55f));
            break;
        case MONSTER_THORN_LARVA:
        case MONSTER_RAPTOR:
        default:
            break;
    }
}

static bool IsBossLineLockHorizontal(const Monster *boss) {
    int bossCenterX;
    int bossCenterY;
    int deltaX;
    int deltaY;

    if (boss == NULL) {
        return true;
    }

    bossCenterX = boss->gridX + MONSTER_FOOTPRINT_SIZE / 2;
    bossCenterY = boss->gridY + MONSTER_FOOTPRINT_SIZE / 2;
    deltaX = boss->targetX - bossCenterX;
    deltaY = boss->targetY - bossCenterY;
    if (deltaX < 0) {
        deltaX = -deltaX;
    }
    if (deltaY < 0) {
        deltaY = -deltaY;
    }
    return deltaX >= deltaY;
}

static bool IsPlayerInBossLineLock(const Monster *boss, const Player *player) {
    if (boss == NULL || player == NULL) {
        return false;
    }

    if (Map_GetAreaAt(player->gridX, player->gridY) != MAP_AREA_BOSS_ARENA) {
        return false;
    }

    if (IsBossLineLockHorizontal(boss)) {
        return player->gridY == boss->targetY;
    }
    return player->gridX == boss->targetX;
}

static void ExecuteBossAttack(TaskSystem *tasks, Monster *boss, Player *player) {
    const int phase = GetBossPhase(boss);
    float nextAttackTimer;

    nextAttackTimer = GetBossAttackInterval(boss);

    switch (boss->currentAttack) {
        case BOSS_ATTACK_MELEE: {
            if (TasksRuntime_GetDistanceToMonster(boss, player->gridX, player->gridY) == 1) {
                ApplyMonsterAttack(tasks, boss->type, player);
            }
            if (phase >= 3) {
                nextAttackTimer *= 0.82f;
            }
            break;
        }
        case BOSS_ATTACK_CHARGE: {
            if (IsPlayerInBossLineLock(boss, player)) {
                ApplyMonsterAttack(tasks, boss->type, player);
                if (phase >= 3) {
                    nextAttackTimer *= 0.78f;
                }
            } else {
                float weakPointDuration;
                int litMonoliths;

                litMonoliths = tasks != NULL ? tasks->monolithsLit : 0;
                weakPointDuration = (phase == 2 ? 0.92f : 1.14f);
                weakPointDuration *= 1.0f + 0.12f * (float)litMonoliths;
                boss->weakPointTimer = weakPointDuration;
            }
            break;
        }
        case BOSS_ATTACK_AOE:
        case BOSS_ATTACK_SPAWN:
            if (phase >= 3) {
                nextAttackTimer *= 0.78f;
            }
            break;
        default:
            break;
    }

    boss->currentAttack = BOSS_ATTACK_NONE;
    boss->attackTimer = nextAttackTimer;
}

static void ChooseBossAttack(TaskSystem *tasks, Monster *boss, Player *player) {
    int phase;
    int distance;
    float telegraphScale;

    phase = GetBossPhase(boss);
    distance = TasksRuntime_GetDistanceToMonster(boss, player->gridX, player->gridY);
    boss->targetX = player->gridX;
    boss->targetY = player->gridY;
    telegraphScale = GetBossTelegraphScale(tasks);

    if (distance == 1) {
        boss->currentAttack = BOSS_ATTACK_MELEE;
        boss->attackTelegraph = (phase == 1 ? 0.92f : (phase == 2 ? 0.80f : 0.68f)) * telegraphScale;
    } else if (phase >= 2) {
        boss->currentAttack = BOSS_ATTACK_CHARGE;
        boss->attackTelegraph = (phase == 2 ? 1.18f : 0.98f) * telegraphScale;
    } else {
        boss->currentAttack = BOSS_ATTACK_NONE;
        boss->attackTelegraph = 0.0f;
    }
}

void TasksRuntime_UpdateMonsters(TaskSystem *tasks, const GameMap *map, Player *player, float deltaTime) {
    int index;
    MapArea playerArea;

    playerArea = Map_GetAreaAt(player->gridX, player->gridY);

    for (index = 0; index < tasks->monsterCount; index++) {
        Monster *monster;
        float aggroRange;
        int distance;
        bool sameArea;
        bool disengaged;

        monster = &tasks->monsters[index];
        if (!MonsterCanAct(monster, tasks)) {
            continue;
        }

        monster->moveTimer -= deltaTime;
        monster->attackTimer -= deltaTime;
        monster->recoverTimer -= deltaTime;
        if (monster->weakPointTimer > 0.0f) {
            monster->weakPointTimer -= deltaTime;
            if (monster->weakPointTimer < 0.0f) {
                monster->weakPointTimer = 0.0f;
            }
        }
        if (monster->recoverTimer < 0.0f) {
            monster->recoverTimer = 0.0f;
        }

        sameArea = monster->area == playerArea;
        distance = TasksRuntime_GetDistanceToMonster(monster, player->gridX, player->gridY);
        aggroRange = GetMonsterAggroRange(monster, tasks, player);
        aggroRange *= Player_GetAggroMultiplier(player);
        if (tasks->currentEvent == EVENT_CALM_BEASTS) {
            aggroRange *= 0.5f;
        }
        if (tasks->currentEvent == EVENT_MONSTER_FRENZY) {
            aggroRange *= 1.2f;
        }

        disengaged = !sameArea || (float)distance > aggroRange;
        if (disengaged) {
            UpdateMonsterDisengageRecovery(monster, true, deltaTime);
            monster->attackTelegraph = 0.0f;
            monster->currentAttack = BOSS_ATTACK_NONE;
            monster->recoverTimer = 0.0f;
            continue;
        }
        UpdateMonsterDisengageRecovery(monster, false, deltaTime);

        if (monster->attackTelegraph > 0.0f) {
            monster->attackTelegraph -= deltaTime;
            if (monster->attackTelegraph <= 0.0f) {
                if (monster->type == MONSTER_FINAL_BOSS) {
                    ExecuteBossAttack(tasks, monster, player);
                } else {
                    if (distance == 1) {
                        ApplyMonsterAttack(tasks, monster->type, player);
                    }
                    monster->recoverTimer = GetMonsterRecoverDuration(monster->type);
                    monster->attackTimer = GetMonsterAttackInterval(monster->type);
                }
            }
            continue;
        }

        if (distance == 1) {
            if (monster->attackTimer <= 0.0f) {
                if (monster->type == MONSTER_FINAL_BOSS && monster->currentAttack == BOSS_ATTACK_NONE) {
                    ChooseBossAttack(tasks, monster, player);
                } else if (monster->type == MONSTER_FINAL_BOSS) {
                    ExecuteBossAttack(tasks, monster, player);
                } else if (monster->recoverTimer <= 0.0f) {
                    StartNormalMonsterAttackTelegraph(monster);
                }
            }
            continue;
        }

        if (monster->recoverTimer > 0.0f) {
            continue;
        }
        if (monster->type == MONSTER_FINAL_BOSS
            && GetBossPhase(monster) >= 2
            && monster->attackTimer <= 0.0f
            && monster->currentAttack == BOSS_ATTACK_NONE) {
            ChooseBossAttack(tasks, monster, player);
            if (monster->attackTelegraph > 0.0f) {
                continue;
            }
        }
        if (monster->moveTimer <= 0.0f) {
            MoveMonsterToward(tasks, monster, map, player->gridX, player->gridY);
            monster->moveTimer = monster->type == MONSTER_FINAL_BOSS
                ? GetBossMoveInterval(monster)
                : GetMonsterMoveInterval(monster->type);
        }
    }
}
