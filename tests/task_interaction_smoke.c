#include "map.h"
#include "player.h"
#include "task_system.h"
#include "../src/task_runtime_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "task_interaction_smoke failed: %s\n", message);
    exit(1);
}

static void RequireValidSpawnPlacements(const GameMap *map, const TaskSystem *tasks) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        Require(Map_IsWalkable(map, node->gridX, node->gridY),
                "resource nodes should spawn on walkable world tiles");
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        int otherIndex;
        const Monster *monster;
        int originX;
        int originY;
        int width;
        int height;
        int offsetY;
        int offsetX;

        monster = &tasks->monsters[index];
        TasksRuntime_GetMonsterFootprint(monster, &originX, &originY, &width, &height);
        for (offsetY = 0; offsetY < height; offsetY++) {
            for (offsetX = 0; offsetX < width; offsetX++) {
                Require(Map_IsWalkable(map, originX + offsetX, originY + offsetY),
                        "monsters should keep their full footprint on walkable world tiles");
            }
        }

        for (otherIndex = index + 1; otherIndex < tasks->monsterCount; otherIndex++) {
            const Monster *other;
            int otherOriginX;
            int otherOriginY;
            int otherWidth;
            int otherHeight;

            other = &tasks->monsters[otherIndex];
            TasksRuntime_GetMonsterFootprint(other, &otherOriginX, &otherOriginY, &otherWidth, &otherHeight);
            Require(originX + width <= otherOriginX
                        || otherOriginX + otherWidth <= originX
                        || originY + height <= otherOriginY
                        || otherOriginY + otherHeight <= originY,
                    "monster spawns should not overlap each other");
        }
    }

    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;

        log = &tasks->logs[index];
        Require(Map_IsWalkable(map, log->gridX, log->gridY),
                "ship logs should spawn on walkable world tiles");
    }
}

static int CountMonstersOfType(const TaskSystem *tasks, MonsterType type) {
    int count;
    int index;

    count = 0;
    for (index = 0; index < tasks->monsterCount; index++) {
        if (tasks->monsters[index].active && tasks->monsters[index].type == type) {
            count += 1;
        }
    }

    return count;
}

static int SumActiveBaseYield(const TaskSystem *tasks, ResourceType type) {
    int totalYield;
    int index;

    totalYield = 0;
    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->active && node->type == type && node->area == MAP_AREA_BASE) {
            totalYield += node->baseYield;
        }
    }

    return totalYield;
}

static int CountActiveNodesInLocation(const TaskSystem *tasks, const char *locationName) {
    int count;
    int index;

    count = 0;
    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;
        const char *nodeLocation;

        node = &tasks->nodes[index];
        if (!node->active) {
            continue;
        }

        nodeLocation = Map_GetLocationNameAt(node->gridX, node->gridY);
        if (nodeLocation != NULL && strcmp(nodeLocation, locationName) == 0) {
            count += 1;
        }
    }

    return count;
}

static int CountActiveNodesOfType(const TaskSystem *tasks, ResourceType type) {
    int count;
    int index;

    count = 0;
    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->active && node->type == type) {
            count += 1;
        }
    }

    return count;
}

static int CountActiveMonstersInLocation(const TaskSystem *tasks, const char *locationName) {
    int count;
    int index;

    count = 0;
    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;
        const char *monsterLocation;

        monster = &tasks->monsters[index];
        if (!monster->active) {
            continue;
        }

        monsterLocation = Map_GetLocationNameAt(monster->gridX, monster->gridY);
        if (monsterLocation != NULL && strcmp(monsterLocation, locationName) == 0) {
            count += 1;
        }
    }

    return count;
}

static int CountLogsInArea(const TaskSystem *tasks, MapArea area) {
    int count;
    int index;

    count = 0;
    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;

        log = &tasks->logs[index];
        if (Map_GetAreaAt(log->gridX, log->gridY) == area) {
            count += 1;
        }
    }

    return count;
}

static int CountLogsInLocation(const TaskSystem *tasks, const char *locationName) {
    int count;
    int index;

    count = 0;
    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;
        const char *logLocation;

        log = &tasks->logs[index];
        logLocation = Map_GetLocationNameAt(log->gridX, log->gridY);
        if (logLocation != NULL && strcmp(logLocation, locationName) == 0) {
            count += 1;
        }
    }

    return count;
}

static const ShipLog *FindLogInLocation(const TaskSystem *tasks, const char *locationName) {
    int index;

    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;
        const char *logLocation;

        log = &tasks->logs[index];
        logLocation = Map_GetLocationNameAt(log->gridX, log->gridY);
        if (logLocation != NULL && strcmp(logLocation, locationName) == 0) {
            return log;
        }
    }

    return NULL;
}

static const Monster *FindActiveMonsterInLocation(const TaskSystem *tasks, const char *locationName) {
    int index;

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;
        const char *monsterLocation;

        monster = &tasks->monsters[index];
        if (!monster->active) {
            continue;
        }

        monsterLocation = Map_GetLocationNameAt(monster->gridX, monster->gridY);
        if (monsterLocation != NULL && strcmp(monsterLocation, locationName) == 0) {
            return monster;
        }
    }

    return NULL;
}

static int DistanceManhattan(int ax, int ay, int bx, int by) {
    int dx;
    int dy;

    dx = ax - bx;
    dy = ay - by;
    if (dx < 0) {
        dx = -dx;
    }
    if (dy < 0) {
        dy = -dy;
    }
    return dx + dy;
}

static void CollectAllLogs(TaskSystem *tasks) {
    int index;

    for (index = 0; index < tasks->logCount; index++) {
        tasks->logs[index].collected = true;
        tasks->logs[index].active = true;
    }
}

static void PrepareEndingBranch(TaskSystem *tasks) {
    tasks->stage = 7;
    tasks->ending = ENDING_NONE;
    tasks->selectedEndingRoute = ENDING_NONE;
    tasks->endingArchiveReviewed = true;
    tasks->signalTowerActivated = false;
    tasks->bossDefeated = false;
    tasks->oxygenRepairLevel = 2;
    tasks->commRepairLevel = 1;
    tasks->energyRepairLevel = 1;
    tasks->crashClueFound = true;
    tasks->amplifierUnlocked = true;
    tasks->westW5Completed = true;
    tasks->southS5Completed = true;
    CollectAllLogs(tasks);
}

static ResourceNode *FindActiveBaseNode(TaskSystem *tasks, ResourceType type) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        ResourceNode *node;

        node = &tasks->nodes[index];
        if (node->active && node->type == type && node->area == MAP_AREA_BASE) {
            return node;
        }
    }

    return NULL;
}

static bool IsReasonableNodeArea(const ResourceNode *node) {
    if (node == NULL) {
        return false;
    }

    switch (node->type) {
        case RESOURCE_RELIC_FRAGMENT:
        case RESOURCE_ALIEN_SLIME:
            return node->area == MAP_AREA_RUINS;
        case RESOURCE_ENERGY_CRYSTAL:
        case RESOURCE_CALM_MUSHROOM:
        case RESOURCE_PROTECTIVE_FIBER:
            return node->area == MAP_AREA_SWAMP_DEEP
                || node->area == MAP_AREA_SWAMP_OUTER
                || node->area == MAP_AREA_FOREST;
        case RESOURCE_ALIEN_VINE:
        case RESOURCE_SHELL_FRUIT:
        case RESOURCE_JUNK_METAL:
            return node->area == MAP_AREA_SWAMP_OUTER
                || node->area == MAP_AREA_SWAMP_DEEP
                || node->area == MAP_AREA_FOREST;
        case RESOURCE_WOOD:
        case RESOURCE_METAL_SCRAP:
        case RESOURCE_ORE:
        case RESOURCE_FRUIT:
        case RESOURCE_GLOW_MOSS:
        case RESOURCE_SPECIAL_FUNGUS:
            return node->area == MAP_AREA_BASE
                || node->area == MAP_AREA_FOREST
                || node->area == MAP_AREA_SWAMP_OUTER
                || node->area == MAP_AREA_SWAMP_DEEP;
        case RESOURCE_ENERGY_CORE:
        case RESOURCE_BOSS_SCALE:
        case RESOURCE_COUNT:
        default:
            return true;
    }
}

static bool IsReasonableMonsterArea(const Monster *monster) {
    if (monster == NULL) {
        return false;
    }

    switch (monster->type) {
        case MONSTER_RELIC_GUARD:
            return monster->area == MAP_AREA_RUINS
                || monster->area == MAP_AREA_BOSS_ARENA;
        case MONSTER_FINAL_BOSS:
            return monster->area == MAP_AREA_BOSS_ARENA;
        case MONSTER_SENTINEL_JELLY:
        case MONSTER_FOG_WORM:
            return monster->area == MAP_AREA_SWAMP_DEEP
                || monster->area == MAP_AREA_SWAMP_OUTER
                || monster->area == MAP_AREA_FOREST;
        case MONSTER_THORN_LARVA:
        case MONSTER_WING_BUG:
        case MONSTER_RAPTOR:
        case MONSTER_SWAMP_STALKER:
            return monster->area == MAP_AREA_SWAMP_OUTER
                || monster->area == MAP_AREA_SWAMP_DEEP
                || monster->area == MAP_AREA_FOREST;
        default:
            return false;
    }
}

static void RequireReasonableTaskPlacements(const GameMap *map, const TaskSystem *tasks) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        const ResourceNode *node;

        node = &tasks->nodes[index];
        Require(Map_IsWithinBounds(node->gridX, node->gridY),
                "resource nodes should stay inside the map bounds");
        Require(Map_GetGroundTileAt(map, node->gridX, node->gridY) != TILE_VOID,
                "resource nodes should not resolve onto void tiles");
        Require(IsReasonableNodeArea(node),
                "resource nodes should stay in reasonable biome buckets after spawn resolution");
    }

    for (index = 0; index < tasks->monsterCount; index++) {
        const Monster *monster;

        monster = &tasks->monsters[index];
        Require(Map_IsWithinBounds(monster->gridX, monster->gridY),
                "monster spawns should stay inside the map bounds");
        Require(Map_GetGroundTileAt(map, monster->gridX, monster->gridY) != TILE_VOID,
                "monster spawns should not resolve onto void tiles");
        Require(IsReasonableMonsterArea(monster),
                "monster spawns should stay in the intended route pressure bands");
    }

    for (index = 0; index < tasks->logCount; index++) {
        const ShipLog *log;

        log = &tasks->logs[index];
        Require(Map_IsWithinBounds(log->gridX, log->gridY),
                "ship logs should stay inside the map bounds");
        Require(Map_GetGroundTileAt(map, log->gridX, log->gridY) != TILE_VOID,
                "ship logs should not be placed on void tiles");
        Require(Map_GetAreaAt(log->gridX, log->gridY) == MAP_AREA_BASE
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "West Frontier") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Survey Break") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Canopy Hollow") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Echo Basin") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Last Camp") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Crash Forest") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "South Collapse") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Vent Galleries") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Service Shafts") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Purifier Ring") == 0
                    || strcmp(Map_GetLocationNameAt(log->gridX, log->gridY), "Root Vault") == 0,
                "collectible records should stay inside the ship or on the intended west/south archive anchors");
    }
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    PlayerStatusType activeStatuses[PLAYER_STATUS_COUNT];
    char message[256];
    char tooltip[512];
    int activeStatusCount;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);
    RequireValidSpawnPlacements(&map, &tasks);
    RequireReasonableTaskPlacements(&map, &tasks);
    Require(SumActiveBaseYield(&tasks, RESOURCE_WOOD) >= 4,
            "ship interior should guarantee enough one-time wood for the early mainline recipe floor");
    Require(SumActiveBaseYield(&tasks, RESOURCE_METAL_SCRAP) >= 2,
            "ship interior should guarantee enough one-time metal scrap for the stage 1 oxygen repair");
    Require(SumActiveBaseYield(&tasks, RESOURCE_GLOW_MOSS) >= 2,
            "ship interior should guarantee enough one-time glow moss for the stage 2 glow-stick craft plus oxygen repair");
    Require(SumActiveBaseYield(&tasks, RESOURCE_ORE) >= 1,
            "ship interior should guarantee at least one one-time ore so stage 2 cannot hard-lock on node distribution");
    Require(CountLogsInArea(&tasks, MAP_AREA_BASE) == 3,
            "early ship logs should remain onboard so the intro narrative does not leak into the world");
    Require(tasks.logCount == 14,
            "the full narrative arc should keep all fourteen logs available in task data");
    Require(CountLogsInLocation(&tasks, "West Frontier") == 1,
            "west frontier should expose a first field record anchor");
    Require(CountLogsInLocation(&tasks, "Survey Break") == 1,
            "survey break should expose a follow-up investigation record");
    Require(CountLogsInLocation(&tasks, "Canopy Hollow") == 1,
            "canopy hollow should expose a handoff record");
    Require(CountLogsInLocation(&tasks, "Echo Basin") == 1,
            "echo basin should expose a topology reconstruction record");
    Require(CountLogsInLocation(&tasks, "Last Camp") == 1,
            "last camp should expose a late west archive record");
    Require(CountLogsInLocation(&tasks, "Crash Forest") == 1,
            "crash forest should expose the wreck black-box log once the clue is resolved");
    Require(CountLogsInLocation(&tasks, "South Collapse") == 1,
            "south collapse should expose the opening facility outage memo");
    Require(CountLogsInLocation(&tasks, "Vent Galleries") == 1,
            "vent galleries should expose a calibration handover record");
    Require(CountLogsInLocation(&tasks, "Service Shafts") == 1,
            "service shafts should expose a synchronization record");
    Require(CountLogsInLocation(&tasks, "Purifier Ring") == 1,
            "purifier ring should expose the control brief record");
    Require(CountLogsInLocation(&tasks, "Root Vault") == 1,
            "root vault should expose a south facility dossier record");
    Require(CountActiveNodesInLocation(&tasks, "Terminal Bay") == 0,
            "Loxi's terminal room should not contain stray pickup resources");
    Require(CountActiveNodesInLocation(&tasks, "West Frontier") >= 1,
            "west frontier should keep at least one resource node so W1 is not just an empty border shelf");
    Require(CountActiveNodesInLocation(&tasks, "Survey Break") >= 1,
            "survey break should keep at least one resource node so W2 has tangible map content");
    Require(CountActiveNodesInLocation(&tasks, "Canopy Hollow") >= 1,
            "canopy hollow should keep at least one resource node so W3 is not visually empty");
    Require(CountActiveNodesInLocation(&tasks, "Echo Basin") >= 1,
            "echo basin should keep at least one resource node so W4 has grounded scene support");
    Require(CountActiveNodesInLocation(&tasks, "Last Camp") >= 1,
            "last camp should keep at least one resource node so W5 is not left without any physical pickup content");
    Require(CountActiveNodesInLocation(&tasks, "South Collapse") >= 1,
            "south collapse should keep at least one resource node so S1 is not just a named threshold");
    Require(CountActiveNodesInLocation(&tasks, "Vent Galleries") >= 1,
            "vent galleries should keep at least one resource node so S2 has local resource pressure");
    Require(CountActiveNodesInLocation(&tasks, "Service Shafts") >= 1,
            "service shafts should keep at least one resource node so S3 has recoverable route value");
    Require(CountActiveNodesInLocation(&tasks, "Purifier Ring") >= 1,
            "purifier ring should keep at least one resource node so S4 has facility-floor rewards");
    Require(CountActiveNodesInLocation(&tasks, "Root Vault") >= 1,
            "root vault should keep at least one resource node so S5 does not land as an empty chamber");
    Require(tasks.monsterCount == 12,
            "route cleanup should now place one fixed monster encounter in every west and south route area, plus the ruins guard and final boss");
    Require(CountMonstersOfType(&tasks, MONSTER_RELIC_GUARD) == 1,
            "ruins should now retain only one fixed relic guard");
    Require(CountMonstersOfType(&tasks, MONSTER_FINAL_BOSS) == 1,
            "final boss should remain a single encounter");
    Require(CountActiveMonstersInLocation(&tasks, "West Frontier") == 1,
            "west frontier should retain a single local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Survey Break") == 1,
            "survey break should now carry its own local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Canopy Hollow") == 1,
            "canopy hollow should retain a single local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Echo Basin") == 1,
            "echo basin should retain a single local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Last Camp") == 1,
            "last camp should now carry its own local monster encounter instead of landing empty");
    Require(CountActiveMonstersInLocation(&tasks, "South Collapse") == 1,
            "south collapse should retain a single local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Vent Galleries") == 1,
            "vent galleries should now carry its own local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Service Shafts") == 1,
            "service shafts should retain a single local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Purifier Ring") == 1,
            "purifier ring should now carry its own local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Root Vault") == 1,
            "root vault should retain a single local monster encounter");
    Require(CountActiveMonstersInLocation(&tasks, "Monolith Ring") == 1,
            "ruins should now hold only the relic guard after the boss is moved to the arena");
    Require(CountActiveMonstersInLocation(&tasks, "Guardian Arena") == 1,
            "guardian arena should now hold the isolated final boss encounter");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "West Frontier")->gridX,
                              FindLogInLocation(&tasks, "West Frontier")->gridY,
                              FindActiveMonsterInLocation(&tasks, "West Frontier")->gridX,
                              FindActiveMonsterInLocation(&tasks, "West Frontier")->gridY) <= 6,
            "west frontier log should stay inside the local monster pressure pocket");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Canopy Hollow")->gridX,
                              FindLogInLocation(&tasks, "Canopy Hollow")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Canopy Hollow")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Canopy Hollow")->gridY) <= 6,
            "canopy hollow log should stay inside the local monster pressure pocket");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Echo Basin")->gridX,
                              FindLogInLocation(&tasks, "Echo Basin")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Echo Basin")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Echo Basin")->gridY) <= 6,
            "echo basin log should stay inside the local monster pressure pocket");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "South Collapse")->gridX,
                              FindLogInLocation(&tasks, "South Collapse")->gridY,
                              FindActiveMonsterInLocation(&tasks, "South Collapse")->gridX,
                              FindActiveMonsterInLocation(&tasks, "South Collapse")->gridY) <= 6,
            "south collapse log should stay inside the local monster pressure pocket");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Service Shafts")->gridX,
                              FindLogInLocation(&tasks, "Service Shafts")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Service Shafts")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Service Shafts")->gridY) <= 6,
            "service shafts log should stay inside the local monster pressure pocket");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Root Vault")->gridX,
                              FindLogInLocation(&tasks, "Root Vault")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Root Vault")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Root Vault")->gridY) <= 6,
            "root vault log should stay inside the local monster pressure pocket");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Survey Break")->gridX,
                              FindLogInLocation(&tasks, "Survey Break")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Survey Break")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Survey Break")->gridY) >= 8,
            "survey break log should stay readable as a direct-pickup investigation rather than a monster-gated pickup");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Last Camp")->gridX,
                              FindLogInLocation(&tasks, "Last Camp")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Last Camp")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Last Camp")->gridY) >= 4,
            "last camp log should stay readable as a direct-pickup investigation rather than a monster-gated pickup");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Vent Galleries")->gridX,
                              FindLogInLocation(&tasks, "Vent Galleries")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Vent Galleries")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Vent Galleries")->gridY) >= 8,
            "vent galleries log should stay readable as a direct-pickup investigation rather than a monster-gated pickup");
    Require(DistanceManhattan(FindLogInLocation(&tasks, "Purifier Ring")->gridX,
                              FindLogInLocation(&tasks, "Purifier Ring")->gridY,
                              FindActiveMonsterInLocation(&tasks, "Purifier Ring")->gridX,
                              FindActiveMonsterInLocation(&tasks, "Purifier Ring")->gridY) >= 6,
            "purifier ring log should stay readable as a direct-pickup investigation rather than a monster-gated pickup");

    Player_SetStatus(&player, PLAYER_STATUS_LOW_OXYGEN, 1, 6.0f, 12.0f);
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 1, 10.0f, 8.0f);
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 3, 4.0f, 18.0f);
    Require(player.statusEffects[PLAYER_STATUS_POISONED].level == 3,
            "reapplying the same status should upgrade its level instead of duplicating it");
    Require(player.statusEffects[PLAYER_STATUS_POISONED].remainingTime >= 10.0f,
            "status upgrades should preserve the longest remaining duration");
    activeStatusCount = Player_CollectActiveStatuses(&player, activeStatuses, PLAYER_STATUS_COUNT);
    Require(activeStatusCount == 2,
            "active status collection should return each active status once");
    Require(activeStatuses[0] == PLAYER_STATUS_POISONED && activeStatuses[1] == PLAYER_STATUS_LOW_OXYGEN,
            "negative statuses should be ordered by display priority");
    Player_GetStatusTooltip(&player, PLAYER_STATUS_POISONED, tooltip, (int)sizeof(tooltip));
    Require(strstr(tooltip, "Poisoned") != NULL && strstr(tooltip, "Relief:") != NULL,
            "status tooltip should expose the name and relief guidance");
    Player_DowngradeStatus(&player, PLAYER_STATUS_POISONED, 1, 3.0f, 5.0f);
    Require(player.statusEffects[PLAYER_STATUS_POISONED].level == 1,
            "status downgrade should lower the stored level");
    Player_UpdateStatuses(&player, 6.5f);
    Require(!Player_HasStatus(&player, PLAYER_STATUS_LOW_OXYGEN),
            "timed statuses should expire when their duration elapses");
    Player_ClearAllStatuses(&player);

    tasks.stage = 7;
    for (int index = 0; index < tasks.monsterCount; index++) {
        if (tasks.monsters[index].type == MONSTER_FINAL_BOSS) {
            tasks.monsters[index].health = tasks.monsters[index].maxHealth * 0.69f;
            tasks.monsters[index].phaseTriggered = false;
            break;
        }
    }
    player.gridX = EXTERIOR_X(84);
    player.gridY = EXTERIOR_Y(20);
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(CountMonstersOfType(&tasks, MONSTER_RELIC_GUARD) >= 2,
            "boss phase changes should now be able to summon additional relic guards into the arena");
    Require(CountActiveMonstersInLocation(&tasks, "Guardian Arena") >= 2,
            "guardian arena should gain at least one reinforcement once the boss enters its later phase");
    RequireValidSpawnPlacements(&map, &tasks);
    tasks.stage = 1;

    player.health = 14.0f;
    player.oxygen = 5.0f;
    player.poison = 50.0f;
    player.hasProtectionSuit = true;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_POISONED),
            "poison buildup should activate the poisoned status");
    Require(Player_HasStatus(&player, PLAYER_STATUS_LOW_OXYGEN),
            "very low oxygen should activate the low oxygen status");
    Require(Player_HasStatus(&player, PLAYER_STATUS_CRITICAL_CONDITION),
            "low health should activate the critical condition status");
    Require(Player_HasStatus(&player, PLAYER_STATUS_FILTERED),
            "protection suit should expose the filtered status");
    player.oxygen = 0.0f;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_SUFFOCATING),
            "oxygen depletion should activate the suffocating status");
    player.gridX = EXTERIOR_X(84);
    player.gridY = EXTERIOR_Y(18);
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    player.hasProtectionSuit = false;
    Player_ClearAllStatuses(&player);
    Tasks_Update(&tasks, &map, &player, 0.5f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK),
            "ruins exposure should now be able to trigger oxygen leak as an area identity");
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    player.hasProtectionSuit = false;
    Player_ClearAllStatuses(&player);
    Player_SetStatus(&player, PLAYER_STATUS_FILTERED, 1, 12.0f, 0.22f);
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_FILTERED),
            "temporary filtered effects should persist without being overwritten by suit sync");
    Player_ClearAllStatuses(&player);

    memset(message, 0, sizeof(message));
    Require(!Tasks_HandleInteraction(NULL, &map, &player, message, sizeof(message)),
            "null task system should fail safely");
    Require(strstr(message, "Interaction unavailable.") != NULL,
            "null task system should report an unavailable interaction");

    player.gridX = AIRLOCK_CONSOLE_X - 1;
    player.gridY = AIRLOCK_CONSOLE_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "airlock interaction should produce feedback");
    Require(strstr(message, "still sealed") != NULL,
            "airlock should stay sealed before stage 3");

    tasks.stage = 3;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "airlock should be usable at stage 3");
    Require(Map_IsSwampOuterUnlocked(&map),
            "airlock should unlock the outer swamp");

    player.gridX = SHIP_TERMINAL_BAY_X + 2;
    player.gridY = SHIP_TERMINAL_BAY_Y + 2;
    memset(message, 0, sizeof(message));
    Require(TasksRuntime_HandleShipInteraction(&tasks, &map, &player, TASK_INTERACTION_NONE, message, sizeof(message)),
            "terminal bay room interaction should provide route-debrief guidance");
    Require(strstr(message, "first east relay sortie") != NULL && strstr(message, "Loxi") != NULL,
            "terminal bay should explain that Stage 3 field results are converted into route guidance there");

    player.gridX = tasks.logs[0].gridX;
    player.gridY = tasks.logs[0].gridY;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "collecting an onboard log should still succeed through the unified interaction flow");
    Require(strstr(message, "Press N") != NULL || strstr(message, "按 N") != NULL,
            "log recovery text should now point players to the unified N interface instead of the old L shortcut");
    Require(strstr(message, " with L ") == NULL && strstr(message, "按 L") == NULL,
            "log recovery text should no longer reference the retired log-only keybinding");

    player.gridX = CRASH_CLUE_X - 1;
    player.gridY = CRASH_CLUE_Y;
    tasks.stage = 4;
    player.hasLaserGun = false;
    player.hasProtectionSuit = false;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "crash clue should provide blocked feedback without gear");
    Require(message[0] != '\0',
            "crash clue should explain the missing gear and keep it framed as the next real lead");

    player.hasLaserGun = true;
    player.hasProtectionSuit = true;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "crash clue should advance once the player has gear");
    Require(tasks.crashClueFound,
            "crash clue should be marked as found after the successful interaction");
    Require(tasks.stage >= 5,
            "successful crash clue interaction should advance progression");
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(CountActiveNodesOfType(&tasks, RESOURCE_ENERGY_CORE) == 1,
            "unlocking stage 5 should spawn exactly one active Energy Core node");

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "repeat crash clue interaction should still return a closing message");
    Require(strstr(message, "fully explored") != NULL || strstr(message, "route data") != NULL,
            "repeat crash clue interaction should explain that the wreck investigation is already complete");

    player.gridX = SHIP_DIAGNOSTICS_X + 2;
    player.gridY = SHIP_DIAGNOSTICS_Y + 2;
    memset(message, 0, sizeof(message));
    Require(TasksRuntime_HandleShipInteraction(&tasks, &map, &player, TASK_INTERACTION_NONE, message, sizeof(message)),
            "diagnostics room interaction should provide deep-east planning guidance");
    Require(strstr(message, "Deep Gate") != NULL && strstr(message, "plan") != NULL,
            "diagnostics should frame Stage 5 hazard control as route planning rather than random punishment");

    player.gridX = SHIP_POWER_BAY_X + 2;
    player.gridY = SHIP_POWER_BAY_Y + 2;
    player.resources[RESOURCE_ENERGY_CORE] = 1;
    memset(message, 0, sizeof(message));
    Require(TasksRuntime_HandleShipInteraction(&tasks, &map, &player, TASK_INTERACTION_NONE, message, sizeof(message)),
            "power bay room interaction should explain the Energy Core handoff");
    Require(strstr(message, "Energy Core") != NULL && strstr(message, "opens the north route") != NULL,
            "power bay should explain that installing the core is what converts east proof into north progression");

    tasks.stage = 6;
    tasks.westW4Completed = true;
    tasks.southS4Completed = true;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    player.gridX = LOXI_TERMINAL_X - 1;
    player.gridY = LOXI_TERMINAL_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "stage 6 Loxi interaction should still accept a full fragment set");
    Require(player.resources[RESOURCE_RELIC_FRAGMENT] == 3,
            "stage 6 Loxi sync should read the fragment set without consuming it");
    Require(message[0] != '\0',
            "stage 6 fragment sync should explain the rewritten system-level route comparison once X2 is ready");
    Require(message[0] != '\0',
            "stage 6 fragment sync should explain all three route meanings once cross-route context is ready");

    {
        TaskSystem unlockTasks;
        GameMap unlockMap;
        Player unlockPlayer;

        Map_Init(&unlockMap);
        Tasks_Init(&unlockTasks, &unlockMap);
        Player_Init(&unlockPlayer);
        unlockPlayer.gridX = LOXI_TERMINAL_X - 1;
        unlockPlayer.gridY = LOXI_TERMINAL_Y;
        memset(message, 0, sizeof(message));
        Require(!unlockTasks.communicatorUnlocked,
                "fresh task state should begin with the communicator locked");
        Require(!Map_IsLoxiRoomUnlocked(&unlockMap),
                "fresh map state should begin with the Loxi room sealed");
        Require(TasksRuntime_HandleShipInteraction(&unlockTasks,
                                                   &unlockMap,
                                                   &unlockPlayer,
                                                   TASK_INTERACTION_LOXI_TERMINAL,
                                                   message,
                                                   sizeof(message)),
                "first Loxi interaction should succeed from the respawn room");
        Require(unlockTasks.communicatorUnlocked,
                "first Loxi interaction should unlock the communicator");
        Require(Map_IsLoxiRoomUnlocked(&unlockMap),
                "first Loxi interaction should permanently remove the Loxi room door");
        Require(message[0] != '\0',
                "first Loxi interaction should teach the unified N terminal flow");

        unlockPlayer.gridX = LOXI_TERMINAL_X - 2;
        unlockPlayer.gridY = LOXI_TERMINAL_Y;
        memset(message, 0, sizeof(message));
        Require(Tasks_HandleInteraction(&unlockTasks, &unlockMap, &unlockPlayer, message, sizeof(message)),
                "repeat Loxi interaction should still work slightly outside the terminal footprint");
        Require(message[0] != '\0',
                "repeat Loxi interaction should continue returning terminal guidance after unlock");
    }

    {
        ResourceNode *baseOre;

        baseOre = FindActiveBaseNode(&tasks, RESOURCE_ORE);
        Require(baseOre != NULL, "base ore should exist for interaction-priority checks");
        player.gridX = baseOre->gridX;
        player.gridY = baseOre->gridY - 1;
        player.facingX = 0;
        player.facingY = 1;
        player.oxygen = INITIAL_OXYGEN;
        memset(message, 0, sizeof(message));
        Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
                "base ore should still be collectable when adjacent ship interactions are also in range");
        Require(strstr(message, "Collected Ore") != NULL && strstr(message, "ship supply") != NULL,
                "pickup priority should prefer the ore node over overlapping ship interactions");
        Require(!baseOre->active,
                "collecting the one-time base ore should exhaust the ship supply node");
    }

    tasks.stage = 7;
    player.gridX = MONOLITH_B_X - 1;
    player.gridY = MONOLITH_B_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "first monolith interaction should activate the puzzle");
    Require(strstr(message, "strengthen your boss damage") != NULL,
            "first monolith interaction should explain the Stage 7 payoff");
    Require(strstr(message, "guardian and Signal Tower") != NULL,
            "first monolith interaction should now explain that the ring helps reveal the guardian-tower relationship");

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "second monolith interaction at the same stone should start the real sequence");
    Require(tasks.monolithsLit == 1 && tasks.monolithActivated[1],
            "first real monolith step should permanently light the first stone in the sequence");
    Require(strstr(message, "first monolith locks into resonance") != NULL,
            "first real monolith step should explain the new partial endgame payoff");

    player.gridX = MONOLITH_B_X - 2;
    player.gridY = MONOLITH_B_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "an already lit monolith should stay interactable from the nearby ring tiles");
    Require(strstr(message, "already resonating") != NULL || strstr(message, "already active") != NULL,
            "returning to a lit monolith should still explain its current resonance state");

    player.gridX = MONOLITH_A_X - 1;
    player.gridY = MONOLITH_A_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "third monolith interaction should advance the sequence");
    Require(tasks.monolithsLit == 2 && tasks.monolithActivated[0],
            "second real monolith step should permanently light the second stone in the sequence");
    Require(strstr(message, "second monolith joins the resonance") != NULL,
            "second real monolith step should explain the stronger partial payoff");

    player.gridX = MONOLITH_C_X - 1;
    player.gridY = MONOLITH_C_Y;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "final monolith interaction should complete the sequence");
    Require(tasks.monolithsLit == 3,
            "solving the monolith sequence should light all monoliths");
    Require(strstr(message, "30% more damage") != NULL,
            "solving the monolith sequence should explain the real combat bonus");

    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    tasks.stage = 7;
    player.hasSignalAmplifier = false;
    tasks.bossDefeated = false;
    tasks.signalTowerActivated = false;
    tasks.ending = ENDING_NONE;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "signal tower should explain the blocked heroic route");
    Require(strstr(message, "remaining mainline logs") != NULL && strstr(message, "Loxi") != NULL,
            "blocked tower text should now require archive completion and a return to Loxi before any ending route");

    tasks.monolithsLit = 3;
    tasks.monolithActivated[0] = true;
    tasks.monolithActivated[1] = true;
    tasks.monolithActivated[2] = true;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "signal tower should mention the fully lit ring when the guardian still blocks the route");
    Require(strstr(message, "Loxi") != NULL,
            "tower should continue redirecting the player to the ship branch point until a route is chosen");

    player.gridX = SHIP_CORRIDOR_X + 8;
    player.gridY = SHIP_CORRIDOR_Y + 1;
    player.hasSignalAmplifier = true;
    memset(message, 0, sizeof(message));
    Require(TasksRuntime_HandleShipInteraction(&tasks, &map, &player, TASK_INTERACTION_NONE, message, sizeof(message)),
            "central corridor room interaction should summarize the final base role");
    Require(strstr(message, "decision corridor") != NULL && strstr(message, "settlement") != NULL,
            "central corridor should explain that the full ship now supports the final rescue-versus-settlement decision");

    player.gridX = AIRLOCK_CONSOLE_X - 1;
    player.gridY = AIRLOCK_CONSOLE_Y;
    PrepareEndingBranch(&tasks);
    tasks.bossDefeated = false;
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should become selectable for the airlock-to-arena flow");
    Map_LockSwampOuter(&map);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "heroic-route airlock interaction should now transition into the isolated arena");
    Require(player.gridX == BOSS_ARENA_PLAYER_ENTRY_X && player.gridY == BOSS_ARENA_PLAYER_ENTRY_Y,
            "heroic-route airlock interaction should teleport the player to the arena entry");
    Require(Map_GetAreaAt(player.gridX, player.gridY) == MAP_AREA_BOSS_ARENA,
            "heroic-route airlock interaction should place the player inside the boss arena area");
    Require(strstr(message, "guardian arena") != NULL || strstr(message, "isolated breach mode") != NULL,
            "heroic-route airlock text should explain the forced transition into the isolated boss arena");
    Require(strstr(tasks.objective, "Defeat the guardian") != NULL || strstr(tasks.objective, "isolated arena") != NULL,
            "heroic-route arena transition should immediately update the objective to the guardian fight");

    player.gridX = SIGNAL_TOWER_X - 1;
    player.gridY = SIGNAL_TOWER_Y;
    PrepareEndingBranch(&tasks);
    player.hasSignalAmplifier = true;
    tasks.signalTowerActivated = false;
    tasks.ending = ENDING_NONE;
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "peaceful route should become selectable once every log and archive task is complete");
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "signal tower should accept the peaceful route interaction");
    Require(tasks.signalTowerActivated,
            "signal tower should activate after peaceful route interaction");
    Require(tasks.ending == ENDING_PEACEFUL,
            "signal tower with amplifier should lead to the peaceful ending");
    Require(strstr(message, "Signal Amplifier") != NULL || strstr(message, "peaceful") != NULL,
            "peaceful tower activation should clearly describe the non-combat tower stabilization route");
    Require(strstr(message, "chosen with Loxi") != NULL || strstr(message, "full context") != NULL,
            "peaceful tower activation should frame the outcome as a ship-side commitment");

    tasks.stage = 5;
    player.gridX = SHIP_CREW_QUARTERS_X + 2;
    player.gridY = SHIP_CREW_QUARTERS_Y + 2;
    player.health = 27.0f;
    player.oxygen = 22.0f;
    player.stamina = 11.0f;
    player.pressure = 58.0f;
    player.poison = 18.0f;
    memset(message, 0, sizeof(message));
    Require(TasksRuntime_HandleShipInteraction(&tasks, &map, &player, TASK_INTERACTION_NONE, message, sizeof(message)),
            "crew quarters interaction should still provide the ship rest action");
    Require(player.health == Player_GetMaxHealth(&player),
            "crew quarters rest should now fully restore health");
    Require(player.oxygen > 22.0f && player.oxygen < MAX_OXYGEN,
            "crew quarters rest should still stop short of a full oxygen refill");
    Require(player.stamina > 11.0f,
            "crew quarters rest should still restore stamina");
    Require(player.pressure == INITIAL_PRESSURE,
            "crew quarters rest should still clear pressure");
    Require(strstr(message, "fully recover your health") != NULL || strstr(message, "生命值完全恢复") != NULL,
            "crew quarters rest text should now explain the full-health recovery role");

    map.campPlaced = true;
    map.campX = ROPE_BARRIER_B_X - 1;
    map.campY = ROPE_BARRIER_B_Y;
    player.gridX = map.campX;
    player.gridY = map.campY;
    player.health = 52.0f;
    player.oxygen = 14.0f;
    player.poison = 36.0f;
    player.pressure = 72.0f;
    player.stamina = 24.0f;
    Player_SetStatus(&player, PLAYER_STATUS_OXYGEN_LEAK, 2, 30.0f, 1.4f);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "field camp interaction should provide recovery feedback");
    Require(strstr(message, "safe outdoor fallback") != NULL,
            "field camp interaction should explain the camp's limited recovery role");
    Require(player.health > 52.0f,
            "field camp interaction should restore health in the new survival model");
    Require(player.health < Player_GetMaxHealth(&player),
            "field camp interaction should remain a partial recovery point instead of a full reset");
    Require(player.oxygen > 14.0f && player.oxygen < MAX_OXYGEN,
            "field camp interaction should restore oxygen without fully refilling it");
    Require(player.poison < 36.0f,
            "field camp interaction should relieve some poison without acting like full base treatment");
    Require(player.pressure <= 40.0f,
            "field camp interaction should reduce pressure to the intended cap");
    Require(player.stamina > 24.0f,
            "field camp interaction should restore stamina");
    Require(Player_HasStatus(&player, PLAYER_STATUS_CAMP_RECOVERY),
            "field camp interaction should grant the camp recovery status");
    Require(!Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK)
                || Player_GetStatusEffect(&player, PLAYER_STATUS_OXYGEN_LEAK)->level == 1,
            "field camp interaction should downgrade or clear oxygen leaks instead of fully resetting every anomaly");

    tasks.stage = 5;
    player.gridX = OXYGEN_CONSOLE_X - 1;
    player.gridY = OXYGEN_CONSOLE_Y;
    player.health = 31.0f;
    player.oxygen = 8.0f;
    player.stamina = 12.0f;
    player.pressure = 67.0f;
    player.poison = 42.0f;
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 2, 16.0f, 42.0f);
    Player_SetStatus(&player, PLAYER_STATUS_OXYGEN_LEAK, 2, 12.0f, 1.3f);
    Player_SetStatus(&player, PLAYER_STATUS_LOW_OXYGEN, 2, 12.0f, 8.0f);
    Player_SetStatus(&player, PLAYER_STATUS_CRITICAL_CONDITION, 1, 12.0f, 31.0f);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "oxygen console should provide base recovery feedback after core repairs are complete");
    Require(player.oxygen == MAX_OXYGEN,
            "base oxygen console should fully restore oxygen");
    Require(player.pressure == 0.0f,
            "base oxygen console should stabilize hidden pressure");
    Require(player.health == 31.0f && player.stamina == 12.0f && player.poison == 42.0f,
            "base oxygen console should no longer act like a full-body recovery station");
    Require(Player_HasStatus(&player, PLAYER_STATUS_POISONED)
                && !Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK)
                && !Player_HasStatus(&player, PLAYER_STATUS_LOW_OXYGEN)
                && !Player_HasStatus(&player, PLAYER_STATUS_CRITICAL_CONDITION),
            "base oxygen console should clear breathing-related statuses while leaving unrelated recovery to resting");
    Require(strstr(message, "oxygen reserves are full") != NULL && strstr(message, "breathing alerts") != NULL,
            "base oxygen console text should explain the oxygen-focused recovery role");

    map.campPlaced = false;
    player.gridX = ROPE_BARRIER_A_X - 1;
    player.gridY = ROPE_BARRIER_A_Y;
    player.facingX = 1;
    player.facingY = 0;
    player.hasRope = true;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "rope interaction should work when facing a swamp barrier");
    Require(strstr(message, "shortcut") != NULL,
            "rope interaction should explain that it creates a shortcut");
    Require(Map_GetPropTileAt(&map, ROPE_BARRIER_A_X, ROPE_BARRIER_A_Y) == TILE_VOID,
            "rope interaction should clear the targeted swamp barrier");

    tasks.nodeCount = 2;
    tasks.logCount = 0;
    tasks.currentEvent = EVENT_CLEAR_SKY;
    tasks.nodes[0].active = true;
    tasks.nodes[0].type = RESOURCE_WOOD;
    tasks.nodes[0].gridX = PLAYER_START_X - 1;
    tasks.nodes[0].gridY = PLAYER_START_Y;
    tasks.nodes[0].baseYield = 1;
    tasks.nodes[0].respawnsRemaining = 0;
    tasks.nodes[1].active = true;
    tasks.nodes[1].type = RESOURCE_FRUIT;
    tasks.nodes[1].gridX = PLAYER_START_X + 1;
    tasks.nodes[1].gridY = PLAYER_START_Y;
    tasks.nodes[1].baseYield = 1;
    tasks.nodes[1].respawnsRemaining = 0;
    player.gridX = PLAYER_START_X;
    player.gridY = PLAYER_START_Y;
    player.facingX = 1;
    player.facingY = 0;
    player.stamina = 100.0f;
    player.resources[RESOURCE_WOOD] = 0;
    player.resources[RESOURCE_FRUIT] = 0;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "facing a nearby pickup should still allow collection when multiple pickups are adjacent");
    Require(tasks.nodes[0].active,
            "pickup selection should not consume a different adjacent node that is not being faced");
    Require(!tasks.nodes[1].active,
            "pickup selection should prioritize the adjacent node in the facing direction");
    Require(player.resources[RESOURCE_FRUIT] == 1 && player.resources[RESOURCE_WOOD] == 0,
            "pickup rewards should come from the faced adjacent node");
    tasks.nodes[1].active = true;
    player.oxygen = 4.0f;
    memset(message, 0, sizeof(message));
    Require(!TasksRuntime_CollectNode(&tasks, &player, &tasks.nodes[1], message, sizeof(message)),
            "gathering should fail when the oxygen margin is too low");
    Require(strstr(message, "oxygen margin") != NULL,
            "failed gathering should explain the oxygen-based exertion gate");
    player.oxygen = INITIAL_OXYGEN;
    player.gridX = 10;
    player.gridY = 10;
    player.facingX = 1;
    player.facingY = 0;
    Require(TasksRuntime_GetRectInteractionScore(&player, 11, 10, 1, 1)
                < TasksRuntime_GetRectInteractionScore(&player, 10, 11, 1, 1),
            "facing priority should score the target in front of the player above side-adjacent targets");

    tasks.nodeCount = 1;
    tasks.nodes[0].active = false;
    tasks.nodes[0].type = RESOURCE_ORE;
    tasks.nodes[0].gridX = player.gridX + 1;
    tasks.nodes[0].gridY = player.gridY;
    tasks.nodes[0].baseYield = 1;
    tasks.nodes[0].respawnsRemaining = 2;
    tasks.nodes[0].initialRespawnsRemaining = 3;
    tasks.nodes[0].special = false;
    tasks.nodes[0].area = MAP_AREA_FOREST;
    tasks.nodes[0].awayTimer = 0.0f;
    memset(message, 0, sizeof(message));
    Require(!Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "adjacent depleted exterior nodes should now stay silent instead of faking an interaction");
    Require(message[0] == '\0',
            "adjacent depleted exterior nodes should no longer show refresh-state text");

    memset(message, 0, sizeof(message));
    TasksRuntime_DescribeNodeStatus(&tasks.nodes[0], message, sizeof(message));
    Require(message[0] == '\0',
            "depleted node status text should now stay silent");

    tasks.nodes[0].active = true;
    memset(message, 0, sizeof(message));
    TasksRuntime_DescribeNodeStatus(&tasks.nodes[0], message, sizeof(message));
    Require(strstr(message, "restored") != NULL,
            "reactivated exterior nodes should use the restored abstract state once they have cycled");

    tasks.nodes[0].active = false;
    tasks.nodes[0].respawnsRemaining = 2;
    tasks.nodes[0].initialRespawnsRemaining = 2;
    tasks.nodes[0].area = MAP_AREA_BASE;
    tasks.nodes[0].awayTimer = 400.0f;
    player.gridX = EXTERIOR_X(90);
    player.gridY = EXTERIOR_Y(50);
    TasksRuntime_RespawnNodes(&tasks, &player, 10.0f);
    Require(!tasks.nodes[0].active,
            "ship interior one-time supply nodes should never re-enter the exterior respawn loop");

    tasks.nodes[0].active = true;
    player.gridX = tasks.nodes[0].gridX - 1;
    player.gridY = tasks.nodes[0].gridY;
    player.facingX = 1;
    player.facingY = 0;
    player.oxygen = INITIAL_OXYGEN;
    player.health = INITIAL_HEALTH;
    memset(message, 0, sizeof(message));
    Require(TasksRuntime_CollectNode(&tasks, &player, &tasks.nodes[0], message, sizeof(message)),
            "ship interior one-time supply nodes should still be collectible once");
    Require(strstr(message, "one-time ship supply") != NULL,
            "ship interior collection text should label the node as a one-time supply");
    Require(tasks.nodes[0].respawnsRemaining == 0 && !tasks.nodes[0].active,
            "ship interior one-time supply collection should exhaust the node permanently");

    tasks.monsterCount = 2;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_WING_BUG;
    tasks.monsters[0].gridX = PLAYER_START_X - 2;
    tasks.monsters[0].gridY = PLAYER_START_Y - 1;
    tasks.monsters[0].health = 40.0f;
    tasks.monsters[0].maxHealth = 40.0f;
    tasks.monsters[1].active = true;
    tasks.monsters[1].type = MONSTER_WING_BUG;
    tasks.monsters[1].gridX = PLAYER_START_X + 1;
    tasks.monsters[1].gridY = PLAYER_START_Y - 1;
    tasks.monsters[1].health = 40.0f;
    tasks.monsters[1].maxHealth = 40.0f;
    player.gridX = PLAYER_START_X;
    player.gridY = PLAYER_START_Y;
    player.facingX = 1;
    player.facingY = 0;
    player.hasLaserGun = false;
    player.attackBonus = 0.0f;
    player.pressure = 0.0f;
    player.oxygen = 100.0f;
    player.stamina = 100.0f;
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleAttack(&tasks, &map, &player, message, sizeof(message)),
            "attack should succeed when adjacent monsters are present");
    Require(tasks.monsters[0].health == 40.0f,
            "attack targeting should not hit a different adjacent monster to the side or rear");
    Require(tasks.monsters[1].health < 40.0f,
            "attack targeting should prioritize the adjacent monster in the facing direction");
    tasks.monsters[1].health = 40.0f;
    player.oxygen = 4.0f;
    memset(message, 0, sizeof(message));
    Require(!Tasks_HandleAttack(&tasks, &map, &player, message, sizeof(message)),
            "attack should fail when oxygen is too low for another committed fight");
    Require(strstr(message, "oxygen margin") != NULL,
            "failed attack should explain the new oxygen-based exertion gate");

    player.gridX = 104;
    player.gridY = 54;
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    Player_ClearAllStatuses(&player);
    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_SWAMP_STALKER;
    tasks.monsters[0].gridX = 105;
    tasks.monsters[0].gridY = 54;
    tasks.monsters[0].area = Map_GetAreaAt(tasks.monsters[0].gridX, tasks.monsters[0].gridY);
    tasks.monsters[0].unlockStage = 1;
    tasks.monsters[0].health = 48.0f;
    tasks.monsters[0].maxHealth = 48.0f;
    tasks.monsters[0].attackTimer = 0.0f;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(player.poison > 0.0f,
            "swamp stalkers should now express their identity through poison pressure");

    player.gridX = 112;
    player.gridY = 62;
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    Player_ClearAllStatuses(&player);
    tasks.monsters[0].type = MONSTER_SENTINEL_JELLY;
    tasks.monsters[0].gridX = 113;
    tasks.monsters[0].gridY = 62;
    tasks.monsters[0].area = Map_GetAreaAt(tasks.monsters[0].gridX, tasks.monsters[0].gridY);
    tasks.monsters[0].attackTimer = 0.0f;
    tasks.monsters[0].health = 64.0f;
    tasks.monsters[0].maxHealth = 64.0f;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK),
            "sentinel jelly attacks should now create oxygen leak pressure");

    puts("task_interaction smoke ok");
    return 0;
}
