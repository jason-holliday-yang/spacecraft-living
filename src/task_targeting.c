#include "task_runtime_internal.h"

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static int DistanceManhattan(int ax, int ay, int bx, int by) {
    return AbsInt(ax - bx) + AbsInt(ay - by);
}

static int DistancePointToRectManhattan(int pointX, int pointY, int rectX, int rectY, int width, int height) {
    int dx;
    int dy;

    dx = 0;
    dy = 0;
    if (pointX < rectX) {
        dx = rectX - pointX;
    } else if (pointX >= rectX + width) {
        dx = pointX - (rectX + width - 1);
    }

    if (pointY < rectY) {
        dy = rectY - pointY;
    } else if (pointY >= rectY + height) {
        dy = pointY - (rectY + height - 1);
    }

    return dx + dy;
}

static int ScorePickupTarget(const Player *player, int gridX, int gridY) {
    int facingX;
    int facingY;
    int distance;

    if (player == NULL) {
        return 999;
    }

    distance = DistanceManhattan(player->gridX, player->gridY, gridX, gridY);
    if (distance > 1) {
        return 999;
    }

    if (gridX == player->gridX && gridY == player->gridY) {
        return 0;
    }

    facingX = player->gridX + player->facingX;
    facingY = player->gridY + player->facingY;
    if (gridX == facingX && gridY == facingY) {
        return 1;
    }

    return 2 + distance;
}

static int ScoreAdjacentAttackTarget(const Player *player, int gridX, int gridY) {
    const int width = MONSTER_FOOTPRINT_SIZE;
    const int height = MONSTER_FOOTPRINT_SIZE;

    if (player == NULL) {
        return 999;
    }

    if (player->gridX + player->facingX >= gridX
        && player->gridX + player->facingX < gridX + width
        && player->gridY + player->facingY >= gridY
        && player->gridY + player->facingY < gridY + height) {
        return 0;
    }

    if (DistancePointToRectManhattan(player->gridX, player->gridY, gridX, gridY, width, height) == 1) {
        return 1;
    }

    return 999;
}

static ResourceNode *FindNearbyNode(TaskSystem *tasks, const Player *player, bool includeInactive) {
    int index;
    ResourceNode *bestNode;
    int bestScore;

    bestNode = NULL;
    bestScore = 999;

    for (index = 0; index < tasks->nodeCount; index++) {
        ResourceNode *node;
        int score;

        node = &tasks->nodes[index];
        if (!includeInactive && !node->active) {
            continue;
        }

        score = ScorePickupTarget(player, node->gridX, node->gridY);
        if (score < bestScore) {
            bestScore = score;
            bestNode = node;
        }
    }

    return bestNode;
}

ResourceNode *TasksRuntime_FindNearbyNode(TaskSystem *tasks, const Player *player) {
    return FindNearbyNode(tasks, player, false);
}

ResourceNode *TasksRuntime_FindNearbyNodeAnyState(TaskSystem *tasks, const Player *player) {
    return FindNearbyNode(tasks, player, true);
}

ShipLog *TasksRuntime_FindNearbyLog(TaskSystem *tasks, const Player *player) {
    int index;
    ShipLog *bestLog;
    int bestScore;

    bestLog = NULL;
    bestScore = 999;

    for (index = 0; index < tasks->logCount; index++) {
        ShipLog *log;
        int score;

        log = &tasks->logs[index];
        if (!log->active || log->collected) {
            continue;
        }

        score = ScorePickupTarget(player, log->gridX, log->gridY);
        if (score < bestScore) {
            bestScore = score;
            bestLog = log;
        }
    }

    return bestLog;
}

bool TasksRuntime_HasNearbyPickupPriority(const TaskSystem *tasks, const Player *player) {
    TaskSystem *mutableTasks;

    if (tasks == NULL || player == NULL) {
        return false;
    }

    mutableTasks = (TaskSystem *)tasks;
    return TasksRuntime_FindNearbyNode(mutableTasks, player) != NULL
        || TasksRuntime_FindNearbyLog(mutableTasks, player) != NULL;
}

void TasksRuntime_GetMonsterFootprint(const Monster *monster,
                                      int *originX,
                                      int *originY,
                                      int *width,
                                      int *height) {
    if (originX != NULL) {
        *originX = monster != NULL ? monster->gridX : 0;
    }
    if (originY != NULL) {
        *originY = monster != NULL ? monster->gridY : 0;
    }
    if (width != NULL) {
        *width = MONSTER_FOOTPRINT_SIZE;
    }
    if (height != NULL) {
        *height = MONSTER_FOOTPRINT_SIZE;
    }
}

bool TasksRuntime_IsMonsterOccupyingTile(const Monster *monster, int gridX, int gridY) {
    int originX;
    int originY;
    int width;
    int height;

    if (monster == NULL || !monster->active) {
        return false;
    }

    TasksRuntime_GetMonsterFootprint(monster, &originX, &originY, &width, &height);
    return gridX >= originX
        && gridX < originX + width
        && gridY >= originY
        && gridY < originY + height;
}

int TasksRuntime_GetDistanceToMonster(const Monster *monster, int gridX, int gridY) {
    int originX;
    int originY;
    int width;
    int height;

    if (monster == NULL || !monster->active) {
        return 999;
    }

    TasksRuntime_GetMonsterFootprint(monster, &originX, &originY, &width, &height);
    return DistancePointToRectManhattan(gridX, gridY, originX, originY, width, height);
}

Monster *TasksRuntime_FindMonsterAt(TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && TasksRuntime_IsMonsterOccupyingTile(monster, gridX, gridY)) {
            return monster;
        }
    }

    return NULL;
}

Monster *TasksRuntime_FindAttackTarget(TaskSystem *tasks, const Player *player) {
    int range;
    int step;
    int index;
    Monster *bestMonster;
    int bestScore;

    if (player->hasLaserGun) {
        range = 5;
        for (step = 1; step <= range; step++) {
            int targetX;
            int targetY;
            Monster *monster;

            targetX = player->gridX + player->facingX * step;
            targetY = player->gridY + player->facingY * step;
            monster = TasksRuntime_FindMonsterAt(tasks, targetX, targetY);
            if (monster != NULL) {
                return monster;
            }
        }
    }

    bestMonster = NULL;
    bestScore = 999;
    for (index = 0; index < tasks->monsterCount; index++) {
        Monster *monster;
        int score;

        monster = &tasks->monsters[index];
        if (!monster->active) {
            continue;
        }

        score = ScoreAdjacentAttackTarget(player, monster->gridX, monster->gridY);
        if (score < bestScore) {
            bestScore = score;
            bestMonster = monster;
        }
    }

    return bestMonster;
}
