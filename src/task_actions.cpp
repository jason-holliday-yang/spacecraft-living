#include "task_runtime_internal.h"

bool Tasks_HandleAttack(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    Monster *target;
    float damage;
    float oxygenCost;
    float attackCooldown;
    bool bossWeakPointHit;

    (void)map;

    target = TasksRuntime_FindAttackTarget(tasks, player);
    if (target == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, "No target was hit.");
        return false;
    }

    if (player->health <= 10.0f || Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)) {
        TasksRuntime_WriteMessage(message, messageSize, "You are too injured to keep the fight stable right now.");
        return false;
    }
    if (player->attackCooldown > 0.0f) {
        TasksRuntime_WriteMessage(message, messageSize, "Your weapon is still recovering from the last strike.");
        return false;
    }

    oxygenCost = player->hasLaserGun ? 3.2f : 2.2f;
    if (player->oxygen <= oxygenCost + 3.0f || Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)) {
        TasksRuntime_WriteMessage(message, messageSize, "Your oxygen margin is too thin to fight safely right now.");
        return false;
    }

    Player_DamageOxygen(player, oxygenCost);
    attackCooldown = player->hasLaserGun ? 0.42f : 0.28f;
    player->attackCooldown = attackCooldown;
    damage = Player_GetAttackPower(player);
    bossWeakPointHit = target->type == MONSTER_FINAL_BOSS && target->weakPointTimer > 0.0f;
    if (bossWeakPointHit) {
        damage *= 1.55f + 0.05f * (float)tasks->monolithsLit;
        target->weakPointTimer = 0.0f;
    }

    target->health -= damage;
    if (target->health <= 0.0f) {
        target->active = false;
        TasksRuntime_DropMonsterRewards(tasks, player, target);
        if (target->type == MONSTER_FINAL_BOSS) {
            Tasks_UpdateObjective(tasks, player);
            TasksRuntime_WriteMessage(message,
                                      messageSize,
                                      "The guardian collapses in the northwest ruins. The path to the Signal Tower is finally open, and Loxi can now confirm any settlement plan back at the ship.");
        } else {
            TasksRuntime_WriteMessage(message, messageSize, "Target eliminated. Loot collected.");
        }
        if (tasks->stage == 5 && player->resources[RESOURCE_ENERGY_CORE] > 0) {
            Tasks_UpdateObjective(tasks, player);
        }
        return true;
    }

    if (target->type == MONSTER_THORN_LARVA && !player->hasLaserGun) {
        Player_DamageHealth(player, 1.0f);
    }

    if (bossWeakPointHit) {
        TasksRuntime_WriteMessage(message, messageSize, "Weak point hit. The guardian staggers under the strike.");
    } else {
        TasksRuntime_WriteMessage(message, messageSize, "Attack connected.");
    }
    return true;
}
