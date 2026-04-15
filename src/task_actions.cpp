#include "task_runtime_internal.h"

bool Tasks_HandleAttack(TaskSystem *tasks, GameMap *map, Player *player, char *message, size_t messageSize) {
    Monster *target;
    float damage;
    float oxygenCost;

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

    oxygenCost = player->hasLaserGun ? 3.2f : 2.2f;
    if (player->oxygen <= oxygenCost + 3.0f || Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)) {
        TasksRuntime_WriteMessage(message, messageSize, "Your oxygen margin is too thin to fight safely right now.");
        return false;
    }

    Player_DamageOxygen(player, oxygenCost);
    damage = Player_GetAttackPower(player);
    if (target->type == MONSTER_FINAL_BOSS && player->gridX > target->gridX + MONSTER_FOOTPRINT_SIZE - 1) {
        damage *= 2.0f;
    }
    if (tasks->monolithsLit > 0 && target->type == MONSTER_FINAL_BOSS) {
        damage *= 1.0f + 0.10f * tasks->monolithsLit;
    }

    target->health -= damage;
    if (target->health <= 0.0f) {
        target->active = false;
        TasksRuntime_DropMonsterRewards(tasks, player, target);
        if (target->type == MONSTER_FINAL_BOSS) {
            if (map != NULL) {
                Map_UnlockSwampOuter(map);
            }
            player->gridX = BOSS_ARENA_RETURN_X;
            player->gridY = BOSS_ARENA_RETURN_Y;
            player->facingX = 1;
            player->facingY = 0;
            Player_UpdateWorldPosition(player);
            Tasks_UpdateObjective(tasks, player);
            TasksRuntime_WriteMessage(message, messageSize, "The guardian collapses and the breach spits you back to the ship-side airlock. The Signal Tower is now the final step.");
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

    TasksRuntime_WriteMessage(message, messageSize, "Attack connected.");
    return true;
}
