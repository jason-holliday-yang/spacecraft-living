#include "task_runtime_internal.h"

#include "task_content.h"

#include <string.h>

typedef bool (*SpawnTileValidator)(const TaskSystem *tasks, const GameMap *map, int gridX, int gridY);
typedef int (*SpawnTileScorer)(const TaskSystem *tasks,
                               const GameMap *map,
                               int preferredX,
                               int preferredY,
                               int gridX,
                               int gridY);

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static int DistanceManhattan(int ax, int ay, int bx, int by) {
    return AbsInt(ax - bx) + AbsInt(ay - by);
}

static bool HasWalkableNeighbor(const GameMap *map, int gridX, int gridY) {
    static const int kDirections[4][2] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };
    int directionIndex;

    for (directionIndex = 0; directionIndex < 4; directionIndex++) {
        if (Map_IsWalkable(map, gridX + kDirections[directionIndex][0], gridY + kDirections[directionIndex][1])) {
            return true;
        }
    }

    return false;
}

static int CountWalkableNeighbors(const GameMap *map, int gridX, int gridY) {
    static const int kDirections[4][2] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };
    int walkableCount;
    int directionIndex;

    walkableCount = 0;
    for (directionIndex = 0; directionIndex < 4; directionIndex++) {
        if (Map_IsWalkable(map, gridX + kDirections[directionIndex][0], gridY + kDirections[directionIndex][1])) {
            walkableCount += 1;
        }
    }

    return walkableCount;
}

static bool IsNodeOccupyingTile(const TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->gridX == gridX && node->gridY == gridY) {
            return true;
        }
    }

    return false;
}

static bool IsMonsterOccupyingTile(const TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        if (monster->active && TasksRuntime_IsMonsterOccupyingTile(monster, gridX, gridY)) {
            return true;
        }
    }

    return false;
}

static bool IsMonsterRectWalkable(const GameMap *map, int originX, int originY) {
    int offsetY;
    int offsetX;

    for (offsetY = 0; offsetY < MONSTER_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < MONSTER_FOOTPRINT_SIZE; offsetX++) {
            if (!Map_IsWalkable(map, originX + offsetX, originY + offsetY)) {
                return false;
            }
        }
    }

    return true;
}

static bool HasWalkableNeighborForMonsterRect(const GameMap *map, int originX, int originY) {
    int edgeIndex;

    for (edgeIndex = 0; edgeIndex < MONSTER_FOOTPRINT_SIZE; edgeIndex++) {
        if (Map_IsWalkable(map, originX - 1, originY + edgeIndex)
            || Map_IsWalkable(map, originX + MONSTER_FOOTPRINT_SIZE, originY + edgeIndex)
            || Map_IsWalkable(map, originX + edgeIndex, originY - 1)
            || Map_IsWalkable(map, originX + edgeIndex, originY + MONSTER_FOOTPRINT_SIZE)) {
            return true;
        }
    }

    return false;
}

static bool IsLogOccupyingTile(const TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;

        log = &tasks->logs[index];
        if (!log->collected && log->gridX == gridX && log->gridY == gridY) {
            return true;
        }
    }

    return false;
}

static int DistanceToNearestNode(const TaskSystem *tasks, int gridX, int gridY) {
    int index;
    int bestDistance;

    bestDistance = MAP_WIDTH + MAP_HEIGHT;
    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;
        int distance;

        node = &tasks->nodes[index];
        distance = DistanceManhattan(gridX, gridY, node->gridX, node->gridY);
        if (distance < bestDistance) {
            bestDistance = distance;
        }
    }

    return bestDistance;
}

static int DistanceToNearestMonster(const TaskSystem *tasks, int gridX, int gridY) {
    int index;
    int bestDistance;

    bestDistance = MAP_WIDTH + MAP_HEIGHT;
    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;
        int distance;

        monster = &tasks->monsters[index];
        if (!monster->active) {
            continue;
        }

        distance = DistanceManhattan(gridX, gridY, monster->gridX, monster->gridY);
        if (distance < bestDistance) {
            bestDistance = distance;
        }
    }

    return bestDistance;
}

static int DistanceToNearestLog(const TaskSystem *tasks, int gridX, int gridY) {
    int index;
    int bestDistance;

    bestDistance = MAP_WIDTH + MAP_HEIGHT;
    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;
        int distance;

        log = &tasks->logs[index];
        if (!log->active || log->collected) {
            continue;
        }

        distance = DistanceManhattan(gridX, gridY, log->gridX, log->gridY);
        if (distance < bestDistance) {
            bestDistance = distance;
        }
    }

    return bestDistance;
}

static int LimitDistanceBonus(int distance, int cap) {
    if (distance > cap) {
        return cap;
    }
    return distance;
}

static bool IsSameLocationName(const char *lhs, const char *rhs) {
    if (lhs == NULL || rhs == NULL) {
        return false;
    }

    return strcmp(lhs, rhs) == 0;
}

static bool IsSharedSpawnTileValid(const GameMap *map, int gridX, int gridY) {
    return map != NULL && Map_IsWalkable(map, gridX, gridY) && HasWalkableNeighbor(map, gridX, gridY);
}

static bool IsNodeSpawnTileValid(const TaskSystem *tasks, const GameMap *map, int gridX, int gridY) {
    return IsSharedSpawnTileValid(map, gridX, gridY)
        && !IsNodeOccupyingTile(tasks, gridX, gridY)
        && !IsLogOccupyingTile(tasks, gridX, gridY)
        && !IsMonsterOccupyingTile(tasks, gridX, gridY);
}

static bool IsMonsterSpawnTileValid(const TaskSystem *tasks, const GameMap *map, int gridX, int gridY) {
    int offsetY;
    int offsetX;

    if (!IsMonsterRectWalkable(map, gridX, gridY) || !HasWalkableNeighborForMonsterRect(map, gridX, gridY)) {
        return false;
    }

    for (offsetY = 0; offsetY < MONSTER_FOOTPRINT_SIZE; offsetY++) {
        for (offsetX = 0; offsetX < MONSTER_FOOTPRINT_SIZE; offsetX++) {
            if (IsMonsterOccupyingTile(tasks, gridX + offsetX, gridY + offsetY)
                || IsNodeOccupyingTile(tasks, gridX + offsetX, gridY + offsetY)
                || IsLogOccupyingTile(tasks, gridX + offsetX, gridY + offsetY)) {
                return false;
            }
        }
    }

    return true;
}

static int ScoreNodeSpawnTile(const TaskSystem *tasks,
                              const GameMap *map,
                              int preferredX,
                              int preferredY,
                              int gridX,
                              int gridY) {
    int score;

    score = DistanceManhattan(preferredX, preferredY, gridX, gridY) * 160;
    score -= LimitDistanceBonus(DistanceToNearestNode(tasks, gridX, gridY), 6) * 12;
    score -= LimitDistanceBonus(DistanceToNearestMonster(tasks, gridX, gridY), 5) * 4;
    score -= LimitDistanceBonus(DistanceToNearestLog(tasks, gridX, gridY), 5) * 6;
    score -= CountWalkableNeighbors(map, gridX, gridY) * 10;
    return score;
}

static int ScoreMonsterSpawnTile(const TaskSystem *tasks,
                                 const GameMap *map,
                                 int preferredX,
                                 int preferredY,
                                 int gridX,
                                 int gridY) {
    int score;

    score = DistanceManhattan(preferredX, preferredY, gridX, gridY) * 160;
    score -= LimitDistanceBonus(DistanceToNearestMonster(tasks, gridX, gridY), 6) * 16;
    score -= LimitDistanceBonus(DistanceToNearestNode(tasks, gridX, gridY), 5) * 6;
    score -= LimitDistanceBonus(DistanceToNearestLog(tasks, gridX, gridY), 5) * 8;
    score -= CountWalkableNeighbors(map, gridX, gridY) * 12;
    return score;
}

static bool FindNearestSpawnTile(const TaskSystem *tasks,
                                 const GameMap *map,
                                 int preferredX,
                                 int preferredY,
                                 SpawnTileValidator validator,
                                 SpawnTileScorer scorer,
                                 int maxRadius,
                                 int *resolvedX,
                                 int *resolvedY) {
    int radius;
    MapArea preferredArea;
    const char *preferredLocationName;
    bool foundCandidate;
    int bestScore;
    int bestDistance;
    int bestX;
    int bestY;
    int locationPass;

    if (tasks == NULL || map == NULL || validator == NULL || resolvedX == NULL || resolvedY == NULL) {
        return false;
    }

    preferredArea = Map_GetAreaAt(preferredX, preferredY);
    preferredLocationName = Map_GetLocationNameAt(preferredX, preferredY);
    foundCandidate = false;
    bestScore = 0;
    bestDistance = MAP_WIDTH + MAP_HEIGHT + 1;
    bestX = preferredX;
    bestY = preferredY;

    for (locationPass = 0; locationPass < 2; locationPass++) {
        const bool requireLocationMatch = preferredLocationName != NULL && locationPass == 0;

        for (radius = 0; radius <= maxRadius; radius++) {
            int gridY;

            for (gridY = preferredY - radius; gridY <= preferredY + radius; gridY++) {
                int gridX;

                for (gridX = preferredX - radius; gridX <= preferredX + radius; gridX++) {
                    int distance;

                    if (!Map_IsWithinBounds(gridX, gridY)) {
                        continue;
                    }
                    if (AbsInt(gridX - preferredX) != radius && AbsInt(gridY - preferredY) != radius) {
                        continue;
                    }
                    if (preferredArea != MAP_AREA_BOSS_ARENA
                        && Map_GetAreaAt(gridX, gridY) == MAP_AREA_BOSS_ARENA) {
                        continue;
                    }
                    if (preferredArea != MAP_AREA_UNKNOWN && Map_GetAreaAt(gridX, gridY) != preferredArea) {
                        continue;
                    }
                    if (requireLocationMatch
                        && !IsSameLocationName(Map_GetLocationNameAt(gridX, gridY), preferredLocationName)) {
                        continue;
                    }
                    if (!validator(tasks, map, gridX, gridY)) {
                        continue;
                    }

                    distance = DistanceManhattan(preferredX, preferredY, gridX, gridY);
                    if (!foundCandidate) {
                        bestDistance = distance;
                        bestScore = scorer != NULL ? scorer(tasks, map, preferredX, preferredY, gridX, gridY) : distance;
                        bestX = gridX;
                        bestY = gridY;
                        foundCandidate = true;
                        continue;
                    }

                    if (distance <= bestDistance + 1) {
                        int candidateScore;

                        candidateScore = scorer != NULL ? scorer(tasks, map, preferredX, preferredY, gridX, gridY) : distance;
                        if (distance < bestDistance || candidateScore < bestScore) {
                            bestDistance = distance;
                            bestScore = candidateScore;
                            bestX = gridX;
                            bestY = gridY;
                        }
                    }
                }
            }
        }

        if (foundCandidate) {
            break;
        }
    }

    if (!foundCandidate) {
        return false;
    }

    *resolvedX = bestX;
    *resolvedY = bestY;
    return true;
}

bool TasksRuntime_FindNodeSpawnTile(const TaskSystem *tasks,
                                    const GameMap *map,
                                    int preferredX,
                                    int preferredY,
                                    int *resolvedX,
                                    int *resolvedY) {
    return FindNearestSpawnTile(tasks, map, preferredX, preferredY, IsNodeSpawnTileValid, ScoreNodeSpawnTile, 10, resolvedX, resolvedY);
}

bool TasksRuntime_FindLogSpawnTile(const TaskSystem *tasks,
                                   const GameMap *map,
                                   int preferredX,
                                   int preferredY,
                                   int *resolvedX,
                                   int *resolvedY) {
    if (tasks == NULL || map == NULL || resolvedX == NULL || resolvedY == NULL) {
        return false;
    }

    if (IsNodeSpawnTileValid(tasks, map, preferredX, preferredY)) {
        *resolvedX = preferredX;
        *resolvedY = preferredY;
        return true;
    }

    return FindNearestSpawnTile(tasks, map, preferredX, preferredY, IsNodeSpawnTileValid, NULL, 3, resolvedX, resolvedY);
}

bool TasksRuntime_FindMonsterSpawnTile(const TaskSystem *tasks,
                                       const GameMap *map,
                                       int preferredX,
                                       int preferredY,
                                       int *resolvedX,
                                       int *resolvedY) {
    return FindNearestSpawnTile(tasks, map, preferredX, preferredY, IsMonsterSpawnTileValid, ScoreMonsterSpawnTile, 12, resolvedX, resolvedY);
}

static CombatEncounterId ResolveCombatEncounterId(MonsterType type, MapArea area, int gridX, int gridY) {
    const char *locationName;

    if (type == MONSTER_FINAL_BOSS) {
        return COMBAT_ENCOUNTER_FINAL_BOSS;
    }
    if (type == MONSTER_RELIC_GUARD) {
        return area == MAP_AREA_BOSS_ARENA ? COMBAT_ENCOUNTER_NONE : COMBAT_ENCOUNTER_RELIC_GUARD;
    }

    locationName = Map_GetLocationNameAt(gridX, gridY);
    if (locationName == NULL) {
        return COMBAT_ENCOUNTER_NONE;
    }
    if (strcmp(locationName, "West Frontier") == 0) {
        return COMBAT_ENCOUNTER_WEST_FRONTIER;
    }
    if (strcmp(locationName, "Canopy Hollow") == 0) {
        return COMBAT_ENCOUNTER_CANOPY_HOLLOW;
    }
    if (strcmp(locationName, "Echo Basin") == 0) {
        return COMBAT_ENCOUNTER_ECHO_BASIN;
    }
    if (strcmp(locationName, "Deep Basin") == 0) {
        return COMBAT_ENCOUNTER_DEEP_BASIN;
    }
    if (strcmp(locationName, "South Collapse") == 0) {
        return COMBAT_ENCOUNTER_SOUTH_COLLAPSE;
    }
    if (strcmp(locationName, "Root Vault") == 0) {
        return COMBAT_ENCOUNTER_ROOT_VAULT;
    }

    return COMBAT_ENCOUNTER_NONE;
}

bool TasksRuntime_AddMonsterSpawn(TaskSystem *tasks,
                                  const GameMap *map,
                                  MonsterType type,
                                  int gridX,
                                  int gridY,
                                  int unlockStage) {
    Monster *monster;
    TaskMonsterSpec monsterSpec;
    int resolvedX;
    int resolvedY;

    if (tasks == NULL || map == NULL || tasks->monsterCount >= MAX_MONSTERS
        || !TasksRuntime_FindMonsterSpawnTile(tasks, map, gridX, gridY, &resolvedX, &resolvedY)) {
        return false;
    }

    monster = &tasks->monsters[tasks->monsterCount++];
    memset(monster, 0, sizeof(*monster));
    monster->active = true;
    monster->type = type;
    monster->gridX = resolvedX;
    monster->gridY = resolvedY;
    monster->spawnX = resolvedX;
    monster->spawnY = resolvedY;
    monster->unlockStage = unlockStage;
    monster->area = Map_GetAreaAt(resolvedX, resolvedY);
    monster->encounterId = ResolveCombatEncounterId(type, monster->area, resolvedX, resolvedY);
    if (!TasksContent_GetMonsterSpec(type, &monsterSpec)) {
        memset(&monsterSpec, 0, sizeof(monsterSpec));
        monsterSpec.maxHealth = 20.0f;
    }

    monster->maxHealth = monsterSpec.maxHealth;
    monster->health = monster->maxHealth;
    return true;
}
