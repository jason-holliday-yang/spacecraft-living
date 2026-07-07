#include "task_runtime_internal.h"
#include "localization.h"

static float GetMonsterHitRecoverDuration(const Monster *monster) {
    if (monster == nullptr) {
        return 0.0f;
    }

    switch (monster->type) {
        case MONSTER_WING_BUG:
            return 0.22f;
        case MONSTER_THORN_LARVA:
            return 0.26f;
        case MONSTER_SWAMP_STALKER:
            return 0.34f;
        case MONSTER_SENTINEL_JELLY:
            return 0.42f;
        case MONSTER_FOG_WORM:
            return 0.48f;
        case MONSTER_RELIC_GUARD:
            return 0.54f;
        case MONSTER_RAPTOR:
            return 0.30f;
        case MONSTER_FINAL_BOSS:
        default:
            return 0.0f;
    }
}

static void ApplyMonsterHitRecoveryWindow(Monster *monster) {
    const float recoverDuration = GetMonsterHitRecoverDuration(monster);

    if (monster == nullptr || recoverDuration <= 0.0f) {
        return;
    }

    monster->attackTelegraph = 0.0f;
    monster->currentAttack = BOSS_ATTACK_NONE;
    if (monster->recoverTimer < recoverDuration) {
        monster->recoverTimer = recoverDuration;
    }
    if (monster->attackTimer < recoverDuration * 0.85f) {
        monster->attackTimer = recoverDuration * 0.85f;
    }
}

bool Tasks_HandleAttack(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    Monster *target;
    float damage;
    float oxygenCost;
    float attackCooldown;

    (void)map;

    if (!player->hasLaserGun) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("You need the Laser Gun before you can fight. For now, avoid the creature, route around it, or come back with proper gear.",
                                                  "你必须先拿到激光枪才能战斗。现在先避开这些怪物，绕路收集信息，或者等装备齐了再回来。"));
        return false;
    }

    target = TasksRuntime_FindAttackTarget(tasks, player);
    if (target == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("No target was hit.", "这次攻击没有命中目标。"));
        return false;
    }

    if (player->health <= 10.0f || Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("You are too injured to keep the fight stable right now.",
                                                  "你的伤势太重了，现在无法稳定地继续战斗。"));
        return false;
    }
    if (player->attackCooldown > 0.0f) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Your weapon is still recovering from the last strike.",
                                                  "你的武器还没从上一次攻击中恢复过来。"));
        return false;
    }

    oxygenCost = 3.2f;
    if (player->oxygen <= oxygenCost + 3.0f || Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("Your oxygen margin is too thin to fight safely right now.",
                                                  "你的氧气余量太低了，现在无法安全战斗。"));
        return false;
    }

    Player_DamageOxygen(player, oxygenCost);
    attackCooldown = 1.0f;
    player->attackCooldown = attackCooldown;
    damage = Player_GetAttackPower(player);

    target->health -= damage;
    {
        const bool wasWeakPoint = target->weakPointTimer > 0.0f;
        if (wasWeakPoint) {
            const float weakPointBonus = damage * 0.60f + 10.0f;
            target->health -= weakPointBonus;
            target->weakPointTimer = 0.0f;
        }
        if (target->health <= 0.0f) {
            target->active = false;
            TasksRuntime_DropMonsterRewards(tasks, player, target);
            if (target->type == MONSTER_FINAL_BOSS) {
                Tasks_UpdateObjective(tasks, player);
                TasksRuntime_WriteMessage(message,
                                          messageSize,
                                          Loc_PickLiteral("The guardian collapses in the northwest ruins. The path to the Signal Tower is finally open, and Loxi can now confirm any settlement plan back at the ship.",
                                                          "西北遗迹中的守卫终于倒下了。通往信号塔的道路彻底打开，现在也可以回到飞船让洛希确认任何定居方案。"));
            } else {
                TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Target eliminated. Loot collected.", "目标已清除，战利品已收集。"));
            }
            if (tasks->stage == 5 && player->resources[RESOURCE_ENERGY_CORE] > 0) {
                Tasks_UpdateObjective(tasks, player);
            }
            return true;
        }

        ApplyMonsterHitRecoveryWindow(target);

        if (target->type == MONSTER_THORN_LARVA && !player->hasLaserGun) {
            Player_DamageHealth(player, 1.0f);
        }

        if (target->type == MONSTER_FINAL_BOSS) {
            if (wasWeakPoint) {
                TasksRuntime_WriteMessage(message,
                                          messageSize,
                                          Loc_PickLiteral("Core hit — the guardian staggers from the deep blow.",
                                                          "核心命中——守卫被这次深层打击打得踉跄后退。"));
            } else {
                TasksRuntime_WriteMessage(message,
                                          messageSize,
                                          Loc_PickLiteral("The guardian absorbs the hit, but its momentum breaks for a moment.",
                                                          "守卫吃下了这次打击，但它的推进节奏还是被短暂打断了。"));
            }
        } else {
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Attack connected.", "攻击命中。"));
        }
    }
    return true;
}
