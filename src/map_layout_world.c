#include "map_internal.h"

static const int kCrashVoidX = 39;
static const int kCrashVoidY = 40;
static const int kCrashVoidWidth = 55;
static const int kCrashVoidHeight = 23;

static void FillExteriorGroundRect(GameMap *map, int x, int y, int width, int height, TileType tile) {
    MapInternal_FillGroundRect(map, EXTERIOR_X(x), EXTERIOR_Y(y), EXTERIOR_SIZE(width), EXTERIOR_SIZE(height), tile);
}

static void SetExteriorNaturalProp(GameMap *map, int x, int y, TileType tile) {
    MapInternal_SetNaturalPropTile(map, EXTERIOR_X(x), EXTERIOR_Y(y), tile);
}

static void SetExteriorProp(GameMap *map, int x, int y, TileType tile) {
    MapInternal_SetPropTile(map, EXTERIOR_X(x), EXTERIOR_Y(y), tile);
}

bool MapInternal_IsCrashSiteVoidBounds(int gridX, int gridY) {
    return gridX >= kCrashVoidX
        && gridX <= kCrashVoidX + kCrashVoidWidth - 1
        && gridY >= kCrashVoidY
        && gridY <= kCrashVoidY + kCrashVoidHeight - 1;
}

void MapInternal_SeedWorldLayout(GameMap *map) {
    int row;
    int column;
    int crashVoidRight;
    int crashVoidBottom;

    crashVoidRight = kCrashVoidX + kCrashVoidWidth - 1;
    crashVoidBottom = kCrashVoidY + kCrashVoidHeight - 1;

    MapInternal_FillGroundRect(map,
                               WORLD_MIN_X,
                               WORLD_MIN_Y,
                               WORLD_MAX_X - WORLD_MIN_X + 1,
                               WORLD_MAX_Y - WORLD_MIN_Y + 1,
                               TILE_FOREST_GROUND);

    MapInternal_FillGroundRect(map, kCrashVoidX, kCrashVoidY, kCrashVoidWidth, kCrashVoidHeight, TILE_VOID);
    MapInternal_FillGroundRect(map, 83, 50, 12, 5, TILE_FOREST_GROUND);

    /* North: ruins approach -> monolith ring -> tower plateau. */
    FillExteriorGroundRect(map, 56, 22, 25, 14, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 50, 14, 36, 10, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 46, 10, 45, 13, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 56, 6, 18, 9, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 53, 8, 24, 5, TILE_RUINS_GROUND);

    /* East: outer shelf, flooded detour, then deep gate and basin. */
    FillExteriorGroundRect(map, 96, 28, 23, 22, TILE_SWAMP_GROUND);
    FillExteriorGroundRect(map, 97, 49, 22, 18, TILE_SWAMP_GROUND);
    FillExteriorGroundRect(map, 100, 64, 19, 22, TILE_SWAMP_GROUND);
    FillExteriorGroundRect(map, 101, 34, 5, 16, TILE_FOREST_GROUND);
    FillExteriorGroundRect(map, 108, 34, 10, 28, TILE_DEEP_SWAMP_GROUND);
    FillExteriorGroundRect(map, 110, 61, 11, 24, TILE_DEEP_SWAMP_GROUND);

    /* South: exposed facility shelves and archive decks under the forest canopy. */
    FillExteriorGroundRect(map, 74, 95, 10, 7, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 86, 94, 14, 10, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 100, 94, 14, 10, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 112, 94, 14, 10, TILE_RUINS_GROUND);

    for (column = WORLD_MIN_X; column <= WORLD_MAX_X; column++) {
        TileType northTile;
        TileType southTile;

        northTile = (column >= EXTERIOR_X(44) && column <= EXTERIOR_X(92)) ? TILE_ROCK : TILE_TREE;
        southTile = (column >= EXTERIOR_X(30) && column <= EXTERIOR_X(112)) ? TILE_ROCK : TILE_TREE;
        MapInternal_SetPerimeterProp(map, column, WORLD_MIN_Y, northTile);
        MapInternal_SetPerimeterProp(map, column, WORLD_MAX_Y, southTile);
        if (column % 4 == 0) {
            MapInternal_SetPerimeterProp(map, column, WORLD_MIN_Y + 1, northTile);
        }
        if (column % 4 == 2) {
            MapInternal_SetPerimeterProp(map, column, WORLD_MAX_Y - 1, southTile);
        }
    }

    for (row = WORLD_MIN_Y; row <= WORLD_MAX_Y; row++) {
        TileType westTile;
        TileType eastTile;

        westTile = row >= EXTERIOR_Y(22) && row <= EXTERIOR_Y(90) ? TILE_ROCK : TILE_TREE;
        eastTile = row >= EXTERIOR_Y(24) && row <= EXTERIOR_Y(90) ? TILE_ROCK : TILE_TREE;
        MapInternal_SetPerimeterProp(map, WORLD_MIN_X, row, westTile);
        MapInternal_SetPerimeterProp(map, WORLD_MAX_X, row, eastTile);
        if (row % 4 == 0) {
            MapInternal_SetPerimeterProp(map, WORLD_MIN_X + 1, row, westTile);
        }
        if (row % 4 == 2) {
            MapInternal_SetPerimeterProp(map, WORLD_MAX_X - 1, row, eastTile);
        }
    }

    for (column = kCrashVoidX; column <= crashVoidRight; column += 3) {
        MapInternal_SetPerimeterProp(map, column, kCrashVoidY, TILE_ROCK);
        MapInternal_SetPerimeterProp(map, column, crashVoidBottom, TILE_ROCK);
    }

    for (row = kCrashVoidY + 2; row <= crashVoidBottom - 2; row += 3) {
        MapInternal_SetPerimeterProp(map, kCrashVoidX, row, TILE_ROCK);
        if (row < 48 || row > 56) {
            MapInternal_SetPerimeterProp(map, crashVoidRight, row, TILE_ROCK);
        }
    }

    /* West route: survey trail with staged clearings and cover. */
    for (row = 36; row <= 88; row += 3) {
        if (row < 60 || row > 66) {
            SetExteriorNaturalProp(map, 18, row, TILE_TREE);
        }
        if (row < 72 || row > 82) {
            SetExteriorNaturalProp(map, 27, row, TILE_ROCK);
        }
        if (row >= 44 && row <= 80 && row != 60 && row != 72) {
            SetExteriorNaturalProp(map, 39, row, (row % 6 == 0) ? TILE_ROCK : TILE_TREE);
        }
    }
    for (column = 18; column <= 48; column += 4) {
        if (column != 24 && column != 48) {
            SetExteriorNaturalProp(map, column, 34, (column % 8 == 2) ? TILE_TREE : TILE_ROCK);
        }
        if (column != 24 && column != 44) {
            SetExteriorNaturalProp(map, column, 90, (column % 8 == 2) ? TILE_TREE : TILE_ROCK);
        }
    }
    SetExteriorNaturalProp(map, 22, 68, TILE_TREE);
    SetExteriorNaturalProp(map, 30, 74, TILE_ROCK);
    SetExteriorNaturalProp(map, 36, 78, TILE_ROCK);
    SetExteriorNaturalProp(map, 43, 60, TILE_TREE);
    SetExteriorNaturalProp(map, 47, 82, TILE_ROCK);

    /* North route: ceremonial stone framing without forest bleed. */
    for (column = 50; column <= 88; column += 4) {
        SetExteriorProp(map, column, 8, TILE_ROCK);
        SetExteriorProp(map, column, 20, TILE_ROCK);
        if (column >= 54 && column <= 80) {
            SetExteriorProp(map, column, 28, (column % 8 == 2) ? TILE_ROCK : TILE_VOID);
        }
    }
    for (row = 10; row <= 33; row += 3) {
        if (row < 24 || row > 30) {
            SetExteriorProp(map, 48, row, TILE_ROCK);
            SetExteriorProp(map, 89, row, TILE_ROCK);
        }
        if (row >= 23) {
            SetExteriorProp(map, 55, row, TILE_ROCK);
            SetExteriorProp(map, 80, row, TILE_ROCK);
        }
    }
    SetExteriorProp(map, 58, 12, TILE_ROCK);
    SetExteriorProp(map, 71, 12, TILE_ROCK);
    SetExteriorProp(map, 60, 17, TILE_ROCK);
    SetExteriorProp(map, 75, 18, TILE_ROCK);

    /* East route: levees and tree walls to clarify risk escalation. */
    for (row = 30; row <= 84; row += 3) {
        if (row < 42 || row > 48) {
            SetExteriorNaturalProp(map, 95, row, TILE_TREE);
        }
        if (row < 56 || row > 60) {
            SetExteriorNaturalProp(map, 103, row, (row % 6 == 0) ? TILE_TREE : TILE_ROCK);
        }
        if (row < 72 || row > 78) {
            SetExteriorNaturalProp(map, 118, row, (row % 6 == 3) ? TILE_TREE : TILE_ROCK);
        }
    }
    for (column = 98; column <= 118; column += 4) {
        if (column < 104 || column > 108) {
            SetExteriorNaturalProp(map, column, 30, (column % 8 == 2) ? TILE_TREE : TILE_ROCK);
        }
        if (column < 100 || column > 108) {
            SetExteriorNaturalProp(map, column, 88, TILE_ROCK);
        }
    }
    SetExteriorNaturalProp(map, 102, 46, TILE_TREE);
    SetExteriorNaturalProp(map, 104, 66, TILE_TREE);
    SetExteriorNaturalProp(map, 114, 71, TILE_ROCK);
    SetExteriorNaturalProp(map, 118, 79, TILE_TREE);

    /* South route: collapsed deck line and buried facility edges. */
    for (column = 60; column <= 123; column += 4) {
        SetExteriorNaturalProp(map, column, 92, (column % 8 == 0) ? TILE_TREE : TILE_ROCK);
        if (column >= 86) {
            SetExteriorProp(map, column, 103, TILE_ROCK);
        }
    }
    for (row = 94; row <= 102; row += 2) {
        SetExteriorProp(map, 85, row, TILE_ROCK);
        SetExteriorProp(map, 99, row, TILE_ROCK);
        if (row != 102) {
            SetExteriorProp(map, 111, row, TILE_ROCK);
        }
    }
    SetExteriorProp(map, 76, 97, TILE_ROCK);
    SetExteriorProp(map, 91, 98, TILE_ROCK);
    SetExteriorProp(map, 106, 96, TILE_ROCK);

    for (row = SWAMP_GATE_TOP_Y; row < SWAMP_GATE_TOP_Y + SWAMP_GATE_HEIGHT; row++) {
        MapInternal_SetGroundTile(map, SWAMP_GATE_X, row, TILE_DEEP_SWAMP_GROUND);
        MapInternal_SetGroundTile(map, SWAMP_GATE_X + 1, row, TILE_DEEP_SWAMP_GROUND);
        MapInternal_SetPropTile(map, SWAMP_GATE_X, row, TILE_BARRIER_DEEP);
        MapInternal_SetPropTile(map, SWAMP_GATE_X + 1, row, TILE_BARRIER_DEEP);
    }

    for (row = RUINS_GATE_TOP_Y; row < RUINS_GATE_TOP_Y + RUINS_GATE_HEIGHT; row++) {
        MapInternal_SetGroundTile(map, RUINS_GATE_X, row, TILE_RUINS_GROUND);
        MapInternal_SetGroundTile(map, RUINS_GATE_X + 1, row, TILE_RUINS_GROUND);
        MapInternal_SetPropTile(map, RUINS_GATE_X, row, TILE_BARRIER_RUINS);
        MapInternal_SetPropTile(map, RUINS_GATE_X + 1, row, TILE_BARRIER_RUINS);
        if ((row - RUINS_GATE_TOP_Y) % 3 != 1) {
            MapInternal_SetPropTile(map, RUINS_GATE_X - 1, row, TILE_ROCK);
            MapInternal_SetPropTile(map, RUINS_GATE_X + 2, row, TILE_ROCK);
        }
    }

    MapInternal_SetGroundTile(map, ROPE_BARRIER_A_X, ROPE_BARRIER_A_Y, TILE_SWAMP_GROUND);
    MapInternal_SetGroundTile(map, ROPE_BARRIER_B_X, ROPE_BARRIER_B_Y, TILE_SWAMP_GROUND);
    MapInternal_SetGroundTile(map, ROPE_BARRIER_C_X, ROPE_BARRIER_C_Y, TILE_SWAMP_GROUND);
    MapInternal_SetPropTile(map, ROPE_BARRIER_A_X, ROPE_BARRIER_A_Y, TILE_BARRIER_SWAMP);
    MapInternal_SetPropTile(map, ROPE_BARRIER_B_X, ROPE_BARRIER_B_Y, TILE_BARRIER_SWAMP);
    MapInternal_SetPropTile(map, ROPE_BARRIER_C_X, ROPE_BARRIER_C_Y, TILE_BARRIER_SWAMP);

    MapInternal_FillPropRect(map, COMM_RELAY_X, COMM_RELAY_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, TILE_COMM_RELAY);
    MapInternal_FillPropRect(map, CRASH_CLUE_X, CRASH_CLUE_Y, CRASH_CLUE_FOOTPRINT_SIZE, CRASH_CLUE_FOOTPRINT_SIZE, TILE_CRASH_CLUE);
    MapInternal_FillPropRect(map, SIGNAL_TOWER_X, SIGNAL_TOWER_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, TILE_SIGNAL_TOWER);
    MapInternal_FillPropRect(map, MONOLITH_A_X, MONOLITH_A_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, TILE_MONOLITH);
    MapInternal_FillPropRect(map, MONOLITH_B_X, MONOLITH_B_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, TILE_MONOLITH);
    MapInternal_FillPropRect(map, MONOLITH_C_X, MONOLITH_C_Y, WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE, TILE_MONOLITH);
    MapInternal_SetPropTile(map, EXTERIOR_X(24), EXTERIOR_Y(63), TILE_LOG_SITE);
    MapInternal_SetPropTile(map, EXTERIOR_X(48), EXTERIOR_Y(86), TILE_LOG_SITE);
    MapInternal_SetPropTile(map, EXTERIOR_X(122), EXTERIOR_Y(102), TILE_LOG_SITE);
}
