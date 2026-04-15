#include "map.h"

static bool IsRectBounds(int gridX, int gridY, int x, int y, int width, int height) {
    return gridX >= x && gridX < x + width && gridY >= y && gridY < y + height;
}

bool Map_IsWithinBounds(int gridX, int gridY) {
    return gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT;
}

bool Map_IsWalkable(const GameMap *map, int gridX, int gridY) {
    TileType groundTile;
    TileType propTile;

    if (!Map_IsWithinBounds(gridX, gridY)) {
        return false;
    }

    groundTile = map->groundTiles[gridY][gridX];
    propTile = map->propTiles[gridY][gridX];

    if (groundTile == TILE_VOID) {
        return false;
    }

    return propTile != TILE_TREE
        && propTile != TILE_ROCK
        && propTile != TILE_TECH_TABLE
        && propTile != TILE_STORAGE_LOCKER
        && propTile != TILE_BUNK
        && propTile != TILE_OXYGEN_CONSOLE
        && propTile != TILE_LOXI_TERMINAL
        && propTile != TILE_WORKBENCH
        && propTile != TILE_AIRLOCK_CONSOLE
        && propTile != TILE_AIRLOCK_DOOR
        && propTile != TILE_COMM_RELAY
        && propTile != TILE_CRASH_CLUE
        && propTile != TILE_ENERGY_CONSOLE
        && propTile != TILE_SIGNAL_TOWER
        && propTile != TILE_MONOLITH
        && propTile != TILE_BARRIER_SWAMP
        && propTile != TILE_BARRIER_DEEP
        && propTile != TILE_BARRIER_RUINS;
}

bool Map_IsOpaque(const GameMap *map, int gridX, int gridY) {
    TileType tile;

    tile = Map_GetPropTileAt(map, gridX, gridY);
    return tile == TILE_TREE || tile == TILE_ROCK || tile == TILE_STORAGE_LOCKER;
}

TileType Map_GetTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinBounds(gridX, gridY)) {
        return TILE_VOID;
    }
    if (map->propTiles[gridY][gridX] != TILE_VOID) {
        return map->propTiles[gridY][gridX];
    }
    return map->groundTiles[gridY][gridX];
}

TileType Map_GetGroundTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinBounds(gridX, gridY)) {
        return TILE_VOID;
    }
    return map->groundTiles[gridY][gridX];
}

TileType Map_GetPropTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinBounds(gridX, gridY)) {
        return TILE_VOID;
    }
    return map->propTiles[gridY][gridX];
}

HazardType Map_GetHazardAt(const GameMap *map, int gridX, int gridY) {
    switch (Map_GetGroundTileAt(map, gridX, gridY)) {
        case TILE_FOREST_GROUND:
            if ((gridX + gridY) % 11 == 0) {
                return HAZARD_TRIP;
            }
            return HAZARD_NONE;
        case TILE_SWAMP_GROUND:
            return HAZARD_SWAMP;
        case TILE_DEEP_SWAMP_GROUND:
            return HAZARD_POISON;
        default:
            return HAZARD_NONE;
    }
}

MapArea Map_GetAreaAt(int gridX, int gridY) {
    if (IsRectBounds(gridX, gridY, BOSS_ARENA_X, BOSS_ARENA_Y, BOSS_ARENA_WIDTH, BOSS_ARENA_HEIGHT)) {
        return MAP_AREA_BOSS_ARENA;
    }
    if (IsRectBounds(gridX, gridY, SHIP_CARGO_HOLD_X, SHIP_CREW_QUARTERS_Y, AIRLOCK_DOOR_X - SHIP_CARGO_HOLD_X + 1, SHIP_POWER_BAY_Y + SHIP_POWER_BAY_HEIGHT - SHIP_CREW_QUARTERS_Y)) {
        return MAP_AREA_BASE;
    }
    if (IsRectBounds(gridX, gridY, RUINS_MAIN_X, RUINS_MAIN_Y, RUINS_MAIN_WIDTH, RUINS_MAIN_HEIGHT)
        || IsRectBounds(gridX, gridY, RUINS_APPROACH_X, RUINS_APPROACH_Y, RUINS_APPROACH_WIDTH, RUINS_APPROACH_HEIGHT)) {
        return MAP_AREA_RUINS;
    }
    if (IsRectBounds(gridX, gridY, SWAMP_DEEP_X, SWAMP_DEEP_Y, SWAMP_DEEP_WIDTH, SWAMP_DEEP_HEIGHT)) {
        return MAP_AREA_SWAMP_DEEP;
    }
    if (IsRectBounds(gridX, gridY, SWAMP_OUTER_X, SWAMP_OUTER_Y, SWAMP_OUTER_WIDTH, SWAMP_OUTER_HEIGHT)) {
        return MAP_AREA_SWAMP_OUTER;
    }
    if (gridX >= WORLD_MIN_X && gridX <= WORLD_MAX_X && gridY >= WORLD_MIN_Y && gridY <= WORLD_MAX_Y) {
        return MAP_AREA_FOREST;
    }
    return MAP_AREA_UNKNOWN;
}

Vector2 Map_GridToWorld(int gridX, int gridY) {
    return (Vector2){
        gridX * TILE_SIZE + TILE_SIZE * 0.5f,
        gridY * TILE_SIZE + TILE_SIZE * 0.5f
    };
}

Rectangle Map_GridToRect(int gridX, int gridY) {
    return (Rectangle){
        gridX * TILE_SIZE,
        gridY * TILE_SIZE,
        TILE_SIZE,
        TILE_SIZE
    };
}

const char *Map_GetAreaName(MapArea area) {
    switch (area) {
        case MAP_AREA_BASE:
            return "Ship Base";
        case MAP_AREA_FOREST:
            return "Crash Forest";
        case MAP_AREA_SWAMP_OUTER:
            return "Spore Swamp";
        case MAP_AREA_SWAMP_DEEP:
            return "Spore Swamp";
        case MAP_AREA_RUINS:
            return "Ruins";
        case MAP_AREA_BOSS_ARENA:
            return "Boss Arena";
        case MAP_AREA_UNKNOWN:
        default:
            return "Unknown Area";
    }
}

const char *Map_GetLocationNameAt(int gridX, int gridY) {
    if (IsRectBounds(gridX, gridY, SHIP_CORRIDOR_X, SHIP_CORRIDOR_Y, SHIP_CORRIDOR_WIDTH, SHIP_CORRIDOR_HEIGHT)) {
        return "Central Corridor";
    }
    if (IsRectBounds(gridX, gridY, SHIP_CARGO_HOLD_X, SHIP_CARGO_HOLD_Y, SHIP_CARGO_HOLD_WIDTH, SHIP_CARGO_HOLD_HEIGHT)) {
        return "Cargo Hold";
    }
    if (IsRectBounds(gridX, gridY, SHIP_CREW_QUARTERS_X, SHIP_CREW_QUARTERS_Y, SHIP_CREW_QUARTERS_WIDTH, SHIP_CREW_QUARTERS_HEIGHT)) {
        return "Crew Quarters";
    }
    if (IsRectBounds(gridX, gridY, SHIP_DIAGNOSTICS_X, SHIP_DIAGNOSTICS_Y, SHIP_DIAGNOSTICS_WIDTH, SHIP_DIAGNOSTICS_HEIGHT)) {
        return "Diagnostics";
    }
    if (IsRectBounds(gridX, gridY, SHIP_TERMINAL_BAY_X, SHIP_TERMINAL_BAY_Y, SHIP_TERMINAL_BAY_WIDTH, SHIP_TERMINAL_BAY_HEIGHT)) {
        return "Terminal Bay";
    }
    if (IsRectBounds(gridX, gridY, SHIP_LIFE_SUPPORT_X, SHIP_LIFE_SUPPORT_Y, SHIP_LIFE_SUPPORT_WIDTH, SHIP_LIFE_SUPPORT_HEIGHT)) {
        return "Life Support";
    }
    if (IsRectBounds(gridX, gridY, SHIP_WORKSHOP_X, SHIP_WORKSHOP_Y, SHIP_WORKSHOP_WIDTH, SHIP_WORKSHOP_HEIGHT)) {
        return "Workshop";
    }
    if (IsRectBounds(gridX, gridY, SHIP_POWER_BAY_X, SHIP_POWER_BAY_Y, SHIP_POWER_BAY_WIDTH, SHIP_POWER_BAY_HEIGHT)) {
        return "Power Bay";
    }
    if (IsRectBounds(gridX, gridY, SHIP_AIRLOCK_LINK_X, SHIP_AIRLOCK_LINK_Y, SHIP_AIRLOCK_LINK_WIDTH, SHIP_AIRLOCK_LINK_HEIGHT)) {
        return "Airlock Link";
    }
    if (IsRectBounds(gridX, gridY, BOSS_ARENA_X, BOSS_ARENA_Y, BOSS_ARENA_WIDTH, BOSS_ARENA_HEIGHT)) {
        return "Guardian Arena";
    }

    if (IsRectBounds(gridX, gridY, RUINS_MAIN_X, RUINS_MAIN_Y, RUINS_MAIN_WIDTH, RUINS_MAIN_HEIGHT)) {
        if (IsRectBounds(gridX,
                         gridY,
                         EXTERIOR_X(57),
                         EXTERIOR_Y(6),
                         EXTERIOR_X(70) - EXTERIOR_X(57) + 1,
                         (SIGNAL_TOWER_Y + 6) - EXTERIOR_Y(6) + 1)) {
            return "Signal Tower Plateau";
        }
        return "Monolith Ring";
    }
    if (IsRectBounds(gridX, gridY, RUINS_APPROACH_X, RUINS_APPROACH_Y, RUINS_APPROACH_WIDTH, RUINS_APPROACH_HEIGHT)) {
        return "Ruins Approach";
    }

    if (IsRectBounds(gridX, gridY, SWAMP_DEEP_X, SWAMP_DEEP_Y, SWAMP_DEEP_WIDTH, SWAMP_DEEP_HEIGHT)) {
        if (gridY <= EXTERIOR_Y(61)) {
            return "Deep Gate";
        }
        return "Deep Basin";
    }
    if (IsRectBounds(gridX, gridY, SWAMP_OUTER_X, SWAMP_OUTER_Y, SWAMP_OUTER_WIDTH, SWAMP_OUTER_HEIGHT)) {
        if (gridY <= EXTERIOR_Y(54)) {
            return "Outer Swamp Rim";
        }
        return "Flooded Detour";
    }
    if (gridX >= WORLD_MIN_X && gridX <= WORLD_MAX_X && gridY >= WORLD_MIN_Y && gridY <= WORLD_MAX_Y) {
        if (gridX >= EXTERIOR_X(22) && gridX < EXTERIOR_X(49) && gridY >= EXTERIOR_Y(34) && gridY <= EXTERIOR_Y(88)) {
            if (gridX >= EXTERIOR_X(45)) {
                if (gridY >= EXTERIOR_Y(84)) {
                    return "Last Camp";
                }
                return "Echo Basin";
            }
            if (gridX >= EXTERIOR_X(38)) {
                return "Canopy Hollow";
            }
            if (gridX >= EXTERIOR_X(30)) {
                return "Survey Break";
            }
            return "West Frontier";
        }
        if (gridY > EXTERIOR_Y(93)) {
            if (gridX >= EXTERIOR_X(119)) {
                return "Root Vault";
            }
            if (gridX >= EXTERIOR_X(111)) {
                return "Purifier Ring";
            }
            if (gridX >= EXTERIOR_X(103)) {
                return "Service Shafts";
            }
            if (gridX >= EXTERIOR_X(87)) {
                return "Vent Galleries";
            }
            return "South Collapse";
        }
        return "Crash Forest";
    }

    return Map_GetAreaName(Map_GetAreaAt(gridX, gridY));
}

const char *Map_GetRoomNameAt(int gridX, int gridY) {
    if (Map_GetAreaAt(gridX, gridY) == MAP_AREA_BASE) {
        return Map_GetLocationNameAt(gridX, gridY);
    }
    return Map_GetAreaName(Map_GetAreaAt(gridX, gridY));
}

void Map_UnlockSwampOuter(GameMap *map) {
    int row;

    for (row = AIRLOCK_DOOR_TOP_Y; row < AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT; row++) {
        map->propTiles[row][AIRLOCK_DOOR_X] = TILE_VOID;
    }
}

void Map_LockSwampOuter(GameMap *map) {
    int row;

    for (row = AIRLOCK_DOOR_TOP_Y; row < AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT; row++) {
        map->propTiles[row][AIRLOCK_DOOR_X] = TILE_AIRLOCK_DOOR;
    }
}

bool Map_IsSwampOuterUnlocked(const GameMap *map) {
    int row;

    for (row = AIRLOCK_DOOR_TOP_Y; row < AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT; row++) {
        if (map->propTiles[row][AIRLOCK_DOOR_X] == TILE_AIRLOCK_DOOR) {
            return false;
        }
    }

    return true;
}

void Map_UnlockSwampDeep(GameMap *map) {
    int row;

    for (row = SWAMP_GATE_TOP_Y; row < SWAMP_GATE_TOP_Y + SWAMP_GATE_HEIGHT; row++) {
        map->propTiles[row][SWAMP_GATE_X] = TILE_VOID;
        map->propTiles[row][SWAMP_GATE_X + 1] = TILE_VOID;
    }
}

void Map_UnlockRuins(GameMap *map) {
    int row;

    for (row = RUINS_GATE_TOP_Y; row < RUINS_GATE_TOP_Y + RUINS_GATE_HEIGHT; row++) {
        map->propTiles[row][RUINS_GATE_X] = TILE_VOID;
        map->propTiles[row][RUINS_GATE_X + 1] = TILE_VOID;
    }
}

void Map_SetFieldCamp(GameMap *map, int gridX, int gridY) {
    map->campPlaced = true;
    map->campX = gridX;
    map->campY = gridY;
}

bool Map_CanCrossWithRope(const GameMap *map, int fromX, int fromY, int toX, int toY) {
    TileType toTile;
    int dx;
    int dy;

    if (!Map_IsWithinBounds(fromX, fromY) || !Map_IsWithinBounds(toX, toY)) {
        return false;
    }

    dx = toX - fromX;
    dy = toY - fromY;

    if ((dx == 0 && dy == 0)) {
        return false;
    }

    if (dx != 0 && dy != 0) {
        return false;
    }

    if (dx > 1 || dx < -1 || dy > 1 || dy < -1) {
        return false;
    }

    toTile = map->propTiles[toY][toX];
    return toTile == TILE_BARRIER_SWAMP || toTile == TILE_BARRIER_DEEP;
}

void Map_CreateRopeBridge(GameMap *map, int gridX, int gridY) {
    TileType tile;

    if (!Map_IsWithinBounds(gridX, gridY)) {
        return;
    }

    tile = map->propTiles[gridY][gridX];
    if (tile == TILE_BARRIER_SWAMP || tile == TILE_BARRIER_DEEP) {
        map->propTiles[gridY][gridX] = TILE_VOID;
    }
}
