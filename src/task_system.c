#include "task_runtime_internal.h"

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static int DistanceManhattan(int ax, int ay, int bx, int by) {
    return AbsInt(ax - bx) + AbsInt(ay - by);
}

bool TasksRuntime_IsNearRect(const Player *player, int gridX, int gridY, int width, int height) {
    int row;
    int column;

    for (row = gridY; row < gridY + height; row++) {
        for (column = gridX; column < gridX + width; column++) {
            if (DistanceManhattan(player->gridX, player->gridY, column, row) <= 1) {
                return true;
            }
        }
    }

    return false;
}

int TasksRuntime_GetRectInteractionScore(const Player *player, int gridX, int gridY, int width, int height) {
    int row;
    int column;
    int facingX;
    int facingY;
    int bestScore;

    if (player == NULL) {
        return 999;
    }

    facingX = player->gridX + player->facingX;
    facingY = player->gridY + player->facingY;
    bestScore = 999;

    for (row = gridY; row < gridY + height; row++) {
        for (column = gridX; column < gridX + width; column++) {
            int distance;

            distance = DistanceManhattan(player->gridX, player->gridY, column, row);
            if (distance > 1) {
                continue;
            }

            if (column == player->gridX && row == player->gridY) {
                return 0;
            }
            if (column == facingX && row == facingY) {
                if (bestScore > 1) {
                    bestScore = 1;
                }
                continue;
            }
            if (bestScore > 2) {
                bestScore = 2;
            }
        }
    }

    return bestScore;
}

bool TasksRuntime_IsNearTile(const Player *player, int gridX, int gridY) {
    return DistanceManhattan(player->gridX, player->gridY, gridX, gridY) <= 1;
}

bool Tasks_IsBlockingActorTile(const TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active
            && tasks->stage >= monster->unlockStage
            && TasksRuntime_IsMonsterOccupyingTile(monster, gridX, gridY)) {
            return true;
        }
    }

    return false;
}
