#include "map_anchors_internal.h"

#include <string.h>

static bool gRuntimeAnchorSet[MAP_ANCHOR_COUNT];
static int gRuntimeAnchorX[MAP_ANCHOR_COUNT];
static int gRuntimeAnchorY[MAP_ANCHOR_COUNT];
static bool gRuntimeAnchorLayerActive = false;

typedef struct MapAnchorDefault {
    MapAnchor anchor;
    const char *name;
    int gridX;
    int gridY;
} MapAnchorDefault;

static const MapAnchorDefault kMapAnchorDefaults[] = {
    {MAP_ANCHOR_PLAYER_START, "PLAYER_START", PLAYER_START_X, PLAYER_START_Y},
    {MAP_ANCHOR_PLAYER_RESPAWN, "PLAYER_RESPAWN", PLAYER_RESPAWN_X, PLAYER_RESPAWN_Y},
    {MAP_ANCHOR_AIRLOCK_DOOR, "AIRLOCK_DOOR", AIRLOCK_DOOR_X, AIRLOCK_DOOR_Y},
    {MAP_ANCHOR_AIRLOCK_EXIT, "AIRLOCK_EXIT", AIRLOCK_DOOR_X + 1, AIRLOCK_DOOR_Y},
    {MAP_ANCHOR_LOXI_TERMINAL, "LOXI_TERMINAL", LOXI_TERMINAL_X, LOXI_TERMINAL_Y},
    {MAP_ANCHOR_OXYGEN_CONSOLE, "OXYGEN_CONSOLE", OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y},
    {MAP_ANCHOR_WORKBENCH, "WORKBENCH", WORKBENCH_X, WORKBENCH_Y},
    {MAP_ANCHOR_AIRLOCK_CONSOLE, "AIRLOCK_CONSOLE", AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y},
    {MAP_ANCHOR_ENERGY_CONSOLE, "ENERGY_CONSOLE", ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y},
    {MAP_ANCHOR_COMM_RELAY, "COMM_RELAY", COMM_RELAY_X, COMM_RELAY_Y},
    {MAP_ANCHOR_CRASH_CLUE, "CRASH_CLUE", CRASH_CLUE_X, CRASH_CLUE_Y},
    {MAP_ANCHOR_ENERGY_CORE, "ENERGY_CORE", ENERGY_CORE_NODE_X, ENERGY_CORE_NODE_Y},
    {MAP_ANCHOR_SIGNAL_TOWER, "SIGNAL_TOWER", SIGNAL_TOWER_X, SIGNAL_TOWER_Y},
    {MAP_ANCHOR_BOSS_PLAYER_ENTRY, "BOSS_PLAYER_ENTRY", BOSS_ARENA_PLAYER_ENTRY_X, BOSS_ARENA_PLAYER_ENTRY_Y},
    {MAP_ANCHOR_BOSS_SPAWN, "BOSS_SPAWN", BOSS_ARENA_BOSS_X, BOSS_ARENA_BOSS_Y},
    {MAP_ANCHOR_MONOLITH_A, "MONOLITH_A", MONOLITH_A_X, MONOLITH_A_Y},
    {MAP_ANCHOR_MONOLITH_B, "MONOLITH_B", MONOLITH_B_X, MONOLITH_B_Y},
    {MAP_ANCHOR_MONOLITH_C, "MONOLITH_C", MONOLITH_C_X, MONOLITH_C_Y},
    {MAP_ANCHOR_SWAMP_OUTER_GATE, "SWAMP_OUTER_GATE", SWAMP_GATE_X, SWAMP_GATE_TOP_Y},
    {MAP_ANCHOR_SWAMP_DEEP_GATE, "SWAMP_DEEP_GATE", SWAMP_GATE_X, SWAMP_GATE_TOP_Y},
    {MAP_ANCHOR_RUINS_GATE, "RUINS_GATE", RUINS_GATE_X, RUINS_GATE_TOP_Y},
    {MAP_ANCHOR_RUINS_APPROACH, "RUINS_APPROACH", EXTERIOR_X(64), EXTERIOR_Y(27)},
    {MAP_ANCHOR_ROPE_BARRIER_A, "ROPE_BARRIER_A", ROPE_BARRIER_A_X, ROPE_BARRIER_A_Y},
    {MAP_ANCHOR_ROPE_BARRIER_B, "ROPE_BARRIER_B", ROPE_BARRIER_B_X, ROPE_BARRIER_B_Y},
    {MAP_ANCHOR_ROPE_BARRIER_C, "ROPE_BARRIER_C", ROPE_BARRIER_C_X, ROPE_BARRIER_C_Y},
    {MAP_ANCHOR_WEST_ECHO_BASIN, "WEST_ECHO_BASIN", EXTERIOR_X(40), EXTERIOR_Y(89)},
    {MAP_ANCHOR_SOUTH_FACILITY, "SOUTH_FACILITY", EXTERIOR_X(85), EXTERIOR_Y(93)},
    {MAP_ANCHOR_FINAL_GATE, "FINAL_GATE", SIGNAL_TOWER_X, SIGNAL_TOWER_Y},
    {MAP_ANCHOR_SHIP_INTERIOR_RETURN, "SHIP_INTERIOR_RETURN", AIRLOCK_DOOR_X - 1, AIRLOCK_DOOR_Y},
    {MAP_ANCHOR_SHIP_EXIT_SPAWN, "SHIP_EXIT_SPAWN", AIRLOCK_DOOR_X + 2, AIRLOCK_DOOR_Y},
    {MAP_ANCHOR_SHIP_EXTERIOR_ORIGIN, "SHIP_EXTERIOR_ORIGIN", SHIP_CARGO_HOLD_X, SHIP_CREW_QUARTERS_Y}
};

static void MapInternal_SetAnchor(GameMap *map, MapAnchor anchor, int gridX, int gridY) {
    if (map == NULL || anchor < 0 || anchor >= MAP_ANCHOR_COUNT || !Map_IsWithinMapBounds(map, gridX, gridY)) {
        return;
    }
    map->anchorSet[anchor] = true;
    map->anchorX[anchor] = gridX;
    map->anchorY[anchor] = gridY;
}

const char *Map_GetAnchorName(MapAnchor anchor) {
    int index;

    for (index = 0; index < (int)(sizeof(kMapAnchorDefaults) / sizeof(kMapAnchorDefaults[0])); index++) {
        if (kMapAnchorDefaults[index].anchor == anchor) {
            return kMapAnchorDefaults[index].name;
        }
    }
    return "UNKNOWN_ANCHOR";
}

bool Map_GetAnchorPosition(const GameMap *map, MapAnchor anchor, int *gridX, int *gridY) {
    if (map == NULL || anchor < 0 || anchor >= MAP_ANCHOR_COUNT || !map->anchorSet[anchor]) {
        return false;
    }
    if (gridX != NULL) {
        *gridX = map->anchorX[anchor];
    }
    if (gridY != NULL) {
        *gridY = map->anchorY[anchor];
    }
    return true;
}

bool Map_GetAnchorPositionByName(const GameMap *map,
                                    const char *anchorName,
                                    int *gridX,
                                    int *gridY) {
    int anchor;

    if (map == NULL || anchorName == NULL || anchorName[0] == '\0') {
        return false;
    }
    for (anchor = 0; anchor < MAP_ANCHOR_COUNT; anchor++) {
        if (strcmp(Map_GetAnchorName((MapAnchor)anchor), anchorName) == 0) {
            return Map_GetAnchorPosition(map, (MapAnchor)anchor, gridX, gridY);
        }
    }
    return false;
}

bool Map_GetRuntimeAnchorPosition(MapAnchor anchor, int *gridX, int *gridY) {
    if (!gRuntimeAnchorLayerActive || anchor < 0 || anchor >= MAP_ANCHOR_COUNT || !gRuntimeAnchorSet[anchor]) {
        return false;
    }
    if (gridX != NULL) {
        *gridX = gRuntimeAnchorX[anchor];
    }
    if (gridY != NULL) {
        *gridY = gRuntimeAnchorY[anchor];
    }
    return true;
}

void MapInternal_ActivateRuntimeAnchorLayer(const GameMap *map) {
    int anchor;

    if (map == NULL) {
        gRuntimeAnchorLayerActive = false;
        for (anchor = 0; anchor < MAP_ANCHOR_COUNT; anchor++) {
            gRuntimeAnchorSet[anchor] = false;
            gRuntimeAnchorX[anchor] = -1;
            gRuntimeAnchorY[anchor] = -1;
        }
        return;
    }

    for (anchor = 0; anchor < MAP_ANCHOR_COUNT; anchor++) {
        gRuntimeAnchorSet[anchor] = map->anchorSet[anchor];
        gRuntimeAnchorX[anchor] = map->anchorX[anchor];
        gRuntimeAnchorY[anchor] = map->anchorY[anchor];
    }
    gRuntimeAnchorLayerActive = true;
}

void MapInternal_RebuildLegacyAnchors(GameMap *map) {
    int index;

    if (map == NULL) {
        return;
    }

    for (index = 0; index < (int)(sizeof(kMapAnchorDefaults) / sizeof(kMapAnchorDefaults[0])); index++) {
        MapInternal_SetAnchor(map,
                              kMapAnchorDefaults[index].anchor,
                              kMapAnchorDefaults[index].gridX,
                              kMapAnchorDefaults[index].gridY);
    }
}
