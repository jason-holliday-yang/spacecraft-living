#include "map_internal.h"

bool MapInternal_IsNaturalGroundTile(TileType tile) {
    return tile == TILE_FOREST_GROUND
        || tile == TILE_SWAMP_GROUND
        || tile == TILE_DEEP_SWAMP_GROUND;
}
void MapInternal_FillGroundRect(GameMap *map, int x, int y, int width, int height, TileType tile) {
    int row;
    int column;

    for (row = y; row < y + height; row++) {
        for (column = x; column < x + width; column++) {
            if (Map_IsWithinBounds(column, row)) {
                map->groundTiles[row][column] = tile;
            }
        }
    }
}

void MapInternal_FillPropRect(GameMap *map, int x, int y, int width, int height, TileType tile) {
    int row;
    int column;

    for (row = y; row < y + height; row++) {
        for (column = x; column < x + width; column++) {
            if (Map_IsWithinBounds(column, row)) {
                map->propTiles[row][column] = tile;
            }
        }
    }
}

static bool IsExteriorEntranceGap(int x, int y) {
    return x >= AIRLOCK_DOOR_X + 1
        && x <= AIRLOCK_DOOR_X + AIRLOCK_EXIT_WIDTH
        && y >= AIRLOCK_DOOR_TOP_Y
        && y < AIRLOCK_DOOR_TOP_Y + AIRLOCK_DOOR_HEIGHT;
}

void MapInternal_SetPerimeterProp(GameMap *map, int x, int y, TileType tile) {
    if (!Map_IsWithinBounds(x, y) || map->groundTiles[y][x] == TILE_VOID || IsExteriorEntranceGap(x, y)) {
        return;
    }

    if (map->propTiles[y][x] == TILE_VOID) {
        map->propTiles[y][x] = tile;
    }
}

void MapInternal_SetGroundTile(GameMap *map, int x, int y, TileType tile) {
    if (Map_IsWithinBounds(x, y)) {
        map->groundTiles[y][x] = tile;
    }
}

void MapInternal_SetPropTile(GameMap *map, int x, int y, TileType tile) {
    if (Map_IsWithinBounds(x, y)) {
        map->propTiles[y][x] = tile;
    }
}

void MapInternal_SetNaturalPropTile(GameMap *map, int x, int y, TileType tile) {
    if (!Map_IsWithinBounds(x, y)) {
        return;
    }

    if (map->propTiles[y][x] != TILE_VOID) {
        return;
    }

    if (!MapInternal_IsNaturalGroundTile(map->groundTiles[y][x])) {
        return;
    }

    map->propTiles[y][x] = tile;
}

void Map_Init(GameMap *map) {
    int row;
    int column;

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (column = 0; column < MAP_WIDTH; column++) {
            map->groundTiles[row][column] = TILE_VOID;
            map->propTiles[row][column] = TILE_VOID;
        }
    }

    map->campPlaced = false;
    map->campX = -1;
    map->campY = -1;

    MapInternal_SeedWorldLayout(map);
    MapInternal_SeedShipLayout(map);
}
