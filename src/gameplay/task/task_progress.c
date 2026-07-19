#include "task_progression_internal.h"
#include "task_archive_internal.h"
#include "task_ending_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

void TasksRuntime_EnsureEnergyCoreNode(TaskSystem *tasks, const GameMap *map, const Player *player) {
    int index;

    (void)map;
    (void)player;

    if (tasks == NULL) {
        return;
    }

    for (index = 0; index < tasks->nodeCount; index++) {
        if (tasks->nodes[index].type == RESOURCE_ENERGY_CORE) {
            tasks->nodes[index].active = false;
            tasks->nodes[index].respawnsRemaining = 0;
            tasks->nodes[index].initialRespawnsRemaining = 0;
            tasks->nodes[index].awayTimer = 0.0f;
        }
    }
}


static bool FindNearestActiveNodeOfType(const TaskSystem *tasks,
                                        ResourceType resourceType,
                                        const Player *player,
                                        int *gridX,
                                        int *gridY) {
    int bestIndex;
    int bestDistance;
    int index;

    if (tasks == NULL || gridX == NULL || gridY == NULL) {
        return false;
    }

    bestIndex = -1;
    bestDistance = INT_MAX;
    for (index = 0; index < tasks->nodeCount; index++) {
        int distance;

        if (!Tasks_IsEntityOnActiveMap(tasks, tasks->nodes[index].mapId)
            || !tasks->nodes[index].active || tasks->nodes[index].type != resourceType) {
            continue;
        }

        if (player == NULL) {
            bestIndex = index;
            break;
        }

        distance = abs(tasks->nodes[index].gridX - player->gridX)
            + abs(tasks->nodes[index].gridY - player->gridY);
        if (bestIndex < 0 || distance < bestDistance) {
            bestIndex = index;
            bestDistance = distance;
        }
    }

    if (bestIndex < 0) {
        return false;
    }

    *gridX = tasks->nodes[bestIndex].gridX;
    *gridY = tasks->nodes[bestIndex].gridY;
    return true;
}


static void SetRuntimeAnchorMarker(MapAnchor anchor, int fallbackX, int fallbackY, int *gridX, int *gridY) {
    if (Map_GetRuntimeAnchorPosition(anchor, gridX, gridY)) {
        return;
    }
    *gridX = fallbackX;
    *gridY = fallbackY;
}


static void SetRuinsApproachMarker(int *gridX, int *gridY) {
    SetRuntimeAnchorMarker(MAP_ANCHOR_RUINS_APPROACH, EXTERIOR_X(64), EXTERIOR_Y(27), gridX, gridY);
}


static void SetDeepSwampMarker(int *gridX, int *gridY) {
    SetRuntimeAnchorMarker(MAP_ANCHOR_ENERGY_CORE, ENERGY_CORE_NODE_X, ENERGY_CORE_NODE_Y, gridX, gridY);
}


static void SetNextRelicFragmentMarker(const TaskSystem *tasks,
                                       const Player *player,
                                       int *gridX,
                                       int *gridY) {
    if (FindNearestActiveNodeOfType(tasks, RESOURCE_RELIC_FRAGMENT, player, gridX, gridY)) {
        return;
    }

    SetRuinsApproachMarker(gridX, gridY);
}


static void SetNextShipIntroLogMarker(const TaskSystem *tasks, int *gridX, int *gridY) {
    static const int kShipIntroLogIndices[] = {
        LOG_INDEX_SHIP_IMPACT_PROTOCOL,
        LOG_INDEX_SHIP_SPLIT_ROSTER,
        LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS
    };
    int index;

    if (tasks == NULL) {
        SetRuntimeAnchorMarker(MAP_ANCHOR_OXYGEN_CONSOLE, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, gridX, gridY);
        return;
    }

    for (index = 0; index < (int)(sizeof(kShipIntroLogIndices) / sizeof(kShipIntroLogIndices[0])); index++) {
        const int logIndex = kShipIntroLogIndices[index];

        if (!TasksArchive_IsCollectedLogIndex(tasks, logIndex)) {
            *gridX = tasks->logs[logIndex].gridX;
            *gridY = tasks->logs[logIndex].gridY;
            return;
        }
    }

    SetRuntimeAnchorMarker(MAP_ANCHOR_OXYGEN_CONSOLE, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, gridX, gridY);
}


static bool HasEnergyCoreExtractionMaterials(const Player *player) {
    return player != NULL
        && player->resources[RESOURCE_JUNK_METAL] >= 1
        && player->resources[RESOURCE_PROTECTIVE_FIBER] >= 1
        && player->resources[RESOURCE_ENERGY_CRYSTAL] >= 1;
}


static void SetNextMonolithMarker(const TaskSystem *tasks, int *gridX, int *gridY) {
    if (!tasks->monolithActivated[1]) {
        SetRuntimeAnchorMarker(MAP_ANCHOR_MONOLITH_B, MONOLITH_B_X, MONOLITH_B_Y, gridX, gridY);
        return;
    }
    if (!tasks->monolithActivated[0]) {
        SetRuntimeAnchorMarker(MAP_ANCHOR_MONOLITH_A, MONOLITH_A_X, MONOLITH_A_Y, gridX, gridY);
        return;
    }
    if (!tasks->monolithActivated[2]) {
        SetRuntimeAnchorMarker(MAP_ANCHOR_MONOLITH_C, MONOLITH_C_X, MONOLITH_C_Y, gridX, gridY);
        return;
    }

    SetRuntimeAnchorMarker(MAP_ANCHOR_SIGNAL_TOWER, SIGNAL_TOWER_X, SIGNAL_TOWER_Y, gridX, gridY);
}


static void SetBossArenaMarker(const TaskSystem *tasks, int *gridX, int *gridY) {
    int index;

    if (tasks != NULL) {
        for (index = 0; index < tasks->monsterCount; index++) {
            const Monster *monster;

            monster = &tasks->monsters[index];
            if (monster->active && monster->type == MONSTER_FINAL_BOSS) {
                *gridX = monster->gridX;
                *gridY = monster->gridY;
                return;
            }
        }
    }

    SetRuntimeAnchorMarker(MAP_ANCHOR_BOSS_SPAWN, BOSS_ARENA_BOSS_X, BOSS_ARENA_BOSS_Y, gridX, gridY);
}


static void SetStage7UnlockMarker(const TaskSystem *tasks,
                                  const Player *player,
                                  int *gridX,
                                  int *gridY) {
    if (tasks == NULL) {
        SetRuntimeAnchorMarker(MAP_ANCHOR_LOXI_TERMINAL, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, gridX, gridY);
        return;
    }

    if (!tasks->endingArchiveReviewed || Tasks_GetAvailableEndingCount(tasks) > 0) {
        SetRuntimeAnchorMarker(MAP_ANCHOR_LOXI_TERMINAL, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, gridX, gridY);
        return;
    }

    if (TasksEnding_IsPeacefulEvidenceReady(tasks) && !TasksEnding_IsPeacefulAmplifierReady(tasks)) {
        if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
            SetRuntimeAnchorMarker(MAP_ANCHOR_WORKBENCH, WORKBENCH_X, WORKBENCH_Y, gridX, gridY);
        } else {
            SetNextRelicFragmentMarker(tasks, player, gridX, gridY);
        }
        return;
    }

    if (!TasksEnding_IsHeroicEvidenceReady(tasks)) {
        *gridX = tasks->logs[LOG_INDEX_CANOPY_HANDOFF_RECORD].gridX;
        *gridY = tasks->logs[LOG_INDEX_CANOPY_HANDOFF_RECORD].gridY;
        return;
    }

    if (!TasksEnding_IsPeacefulEvidenceReady(tasks)) {
        const int logIndex = !TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_VENT_CALIBRATION_HANDOVER)
            ? LOG_INDEX_VENT_CALIBRATION_HANDOVER
            : LOG_INDEX_PURIFIER_RING_CONTROL_BRIEF;

        *gridX = tasks->logs[logIndex].gridX;
        *gridY = tasks->logs[logIndex].gridY;
        return;
    }

    if (!TasksEnding_IsSettlementEvidenceReady(tasks)) {
        *gridX = tasks->logs[LOG_INDEX_LAST_CAMP_TESTAMENT].gridX;
        *gridY = tasks->logs[LOG_INDEX_LAST_CAMP_TESTAMENT].gridY;
        return;
    }

    if (!TasksEnding_IsSettlementBossGateReady(tasks)) {
        SetBossArenaMarker(tasks, gridX, gridY);
        return;
    }

    SetRuntimeAnchorMarker(MAP_ANCHOR_LOXI_TERMINAL, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, gridX, gridY);
}


void TasksRuntime_UnlockStageIfNeeded(TaskSystem *tasks, GameMap *map, int newStage) {
    if (newStage <= tasks->stage) {
        return;
    }

    tasks->stage = newStage;
    if (tasks->stage >= 5) {
        Map_UnlockSwampDeep(map);
        TasksRuntime_EnsureEnergyCoreNode(tasks, map, NULL);
    }
    if (tasks->stage >= 6) {
        Map_UnlockRuins(map);
    }
}


static bool Tasks_GetObjectiveMarkerRaw(const TaskSystem *tasks, const Player *player, int *gridX, int *gridY) {
    switch (tasks->stage) {
        case 1:
            if (!TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_IMPACT_PROTOCOL)
                || !TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_SPLIT_ROSTER)
                || !TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS)) {
                SetNextShipIntroLogMarker(tasks, gridX, gridY);
            } else {
                SetRuntimeAnchorMarker(MAP_ANCHOR_OXYGEN_CONSOLE, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, gridX, gridY);
            }
            return true;
        case 2:
            if (!TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_IMPACT_PROTOCOL)
                || !TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_SPLIT_ROSTER)
                || !TasksArchive_IsCollectedLogIndex(tasks, LOG_INDEX_SHIP_PATTERN_NOT_WILDERNESS)) {
                SetNextShipIntroLogMarker(tasks, gridX, gridY);
            } else if (player != NULL && !player->hasGlowStick) {
                SetRuntimeAnchorMarker(MAP_ANCHOR_WORKBENCH, WORKBENCH_X, WORKBENCH_Y, gridX, gridY);
            } else {
                SetRuntimeAnchorMarker(MAP_ANCHOR_OXYGEN_CONSOLE, OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y, gridX, gridY);
            }
            return true;
        case 6:
            if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                SetRuntimeAnchorMarker(MAP_ANCHOR_LOXI_TERMINAL, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, gridX, gridY);
            } else {
                SetNextRelicFragmentMarker(tasks, player, gridX, gridY);
            }
            return true;
        case 3:
            if (tasks->activeMapKind == MAP_KIND_INTERIOR) {
                SetRuntimeAnchorMarker(MAP_ANCHOR_AIRLOCK_CONSOLE, AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y, gridX, gridY);
            } else {
                SetRuntimeAnchorMarker(MAP_ANCHOR_COMM_RELAY, COMM_RELAY_X, COMM_RELAY_Y, gridX, gridY);
            }
            return true;
        case 4:
            if (player != NULL && (!player->hasLaserGun || !player->hasProtectionSuit)) {
                SetRuntimeAnchorMarker(MAP_ANCHOR_WORKBENCH, WORKBENCH_X, WORKBENCH_Y, gridX, gridY);
            } else {
                SetRuntimeAnchorMarker(MAP_ANCHOR_CRASH_CLUE, CRASH_CLUE_X, CRASH_CLUE_Y, gridX, gridY);
            }
            return true;
        case 5:
            if (player != NULL && player->resources[RESOURCE_ENERGY_CORE] > 0) {
                SetRuntimeAnchorMarker(MAP_ANCHOR_ENERGY_CONSOLE, ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y, gridX, gridY);
            } else if (HasEnergyCoreExtractionMaterials(player)) {
                SetRuntimeAnchorMarker(MAP_ANCHOR_CRASH_CLUE, CRASH_CLUE_X, CRASH_CLUE_Y, gridX, gridY);
            } else {
                SetDeepSwampMarker(gridX, gridY);
            }
            return true;
        case 7:
            if (Tasks_IsEndingBranchReady(tasks) && tasks->selectedEndingRoute == ENDING_NONE) {
                SetStage7UnlockMarker(tasks, player, gridX, gridY);
            } else if (tasks->selectedEndingRoute == ENDING_PEACEFUL) {
                if (player != NULL && player->hasSignalAmplifier) {
                    SetRuntimeAnchorMarker(MAP_ANCHOR_SIGNAL_TOWER, SIGNAL_TOWER_X, SIGNAL_TOWER_Y, gridX, gridY);
                } else if (player != NULL && player->resources[RESOURCE_RELIC_FRAGMENT] >= 3) {
                    SetRuntimeAnchorMarker(MAP_ANCHOR_WORKBENCH, WORKBENCH_X, WORKBENCH_Y, gridX, gridY);
                } else {
                    SetRuinsApproachMarker(gridX, gridY);
                }
            } else if (tasks->selectedEndingRoute == ENDING_HEROIC) {
                if (tasks->bossDefeated) {
                    SetRuntimeAnchorMarker(MAP_ANCHOR_SIGNAL_TOWER, SIGNAL_TOWER_X, SIGNAL_TOWER_Y, gridX, gridY);
                } else {
                    SetBossArenaMarker(tasks, gridX, gridY);
                }
            } else if (tasks->selectedEndingRoute == ENDING_SETTLEMENT) {
                if (tasks->bossDefeated) {
                    SetRuntimeAnchorMarker(MAP_ANCHOR_LOXI_TERMINAL, LOXI_TERMINAL_X, LOXI_TERMINAL_Y, gridX, gridY);
                } else {
                    SetBossArenaMarker(tasks, gridX, gridY);
                }
            } else {
                SetNextMonolithMarker(tasks, gridX, gridY);
            }
            return true;
        default:
            return false;
    }
}


static const char *InferObjectiveMapId(const TaskSystem *tasks, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->logCount; index++) {
        if (tasks->logs[index].gridX == gridX && tasks->logs[index].gridY == gridY) {
            return tasks->logs[index].mapId;
        }
    }
    for (index = 0; index < tasks->nodeCount; index++) {
        if (tasks->nodes[index].gridX == gridX && tasks->nodes[index].gridY == gridY) {
            return tasks->nodes[index].mapId;
        }
    }
    if ((gridX == LOXI_TERMINAL_X && gridY == LOXI_TERMINAL_Y)
        || (gridX == OXYGEN_CONSOLE_X && gridY == OXYGEN_CONSOLE_Y)
        || (gridX == WORKBENCH_X && gridY == WORKBENCH_Y)
        || (gridX == AIRLOCK_CONSOLE_X && gridY == AIRLOCK_CONSOLE_Y)
        || (gridX == ENERGY_CONSOLE_X && gridY == ENERGY_CONSOLE_Y)) {
        return "ship_interior";
    }
    return "planet_surface_01";
}


bool Tasks_GetObjectiveMarker(const TaskSystem *tasks, const Player *player, int *gridX, int *gridY) {
    return Tasks_GetObjectiveMarkerRaw(tasks, player, gridX, gridY);
}


bool Tasks_GetObjectiveMarkerForMap(const TaskSystem *tasks,
                                    const GameMap *map,
                                    const Player *player,
                                    int *gridX,
                                    int *gridY) {
    const char *targetMapId;
    int index;

    if (!Tasks_GetObjectiveMarkerRaw(tasks, player, gridX, gridY) || map == NULL) {
        return false;
    }
    if (strcmp(map->mapId, "legacy_world") == 0) {
        return true;
    }
    targetMapId = InferObjectiveMapId(tasks, *gridX, *gridY);
    if (strcmp(targetMapId, map->mapId) == 0) {
        return true;
    }
    for (index = 0; index < map->portalCount; index++) {
        const MapPortal *portal = &map->portals[index];
        if (strcmp(portal->targetMapId, targetMapId) == 0) {
            *gridX = portal->gridX + portal->width / 2;
            *gridY = portal->gridY + portal->height / 2;
            return true;
        }
    }
    return false;
}


bool Tasks_IsCommunicatorUnlocked(const TaskSystem *tasks) {
    return tasks->communicatorUnlocked;
}
