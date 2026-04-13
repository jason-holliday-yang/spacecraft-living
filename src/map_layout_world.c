#include "map_internal.h"

static const int kCrashVoidX = 39;
static const int kCrashVoidY = 40;
static const int kCrashVoidWidth = 55;
static const int kCrashVoidHeight = 23;

typedef struct {
    int x;
    int y;
    TileType tile;
} ExteriorPropSeed;

static void FillExteriorGroundRect(GameMap *map, int x, int y, int width, int height, TileType tile) {
    MapInternal_FillGroundRect(map, EXTERIOR_X(x), EXTERIOR_Y(y), EXTERIOR_SIZE(width), EXTERIOR_SIZE(height), tile);
}

static void SetExteriorNaturalProp(GameMap *map, int x, int y, TileType tile) {
    MapInternal_SetNaturalPropTile(map, EXTERIOR_X(x), EXTERIOR_Y(y), tile);
}

static void SetExteriorProp(GameMap *map, int x, int y, TileType tile) {
    MapInternal_SetPropTile(map, EXTERIOR_X(x), EXTERIOR_Y(y), tile);
}

static void SetExteriorSeedGroup(GameMap *map, const ExteriorPropSeed *seeds, int count, bool natural) {
    int index;

    for (index = 0; index < count; ++index) {
        if (natural) {
            SetExteriorNaturalProp(map, seeds[index].x, seeds[index].y, seeds[index].tile);
        } else {
            SetExteriorProp(map, seeds[index].x, seeds[index].y, seeds[index].tile);
        }
    }
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

    /* West route: break the old survey barricades into irregular thickets and rubble pockets. */
    {
        static const ExteriorPropSeed kWestRouteSeeds[] = {
            {18, 38, TILE_TREE},
            {20, 43, TILE_TREE},
            {17, 49, TILE_TREE},
            {19, 55, TILE_TREE},
            {18, 73, TILE_TREE},
            {21, 80, TILE_TREE},
            {19, 86, TILE_TREE},
            {26, 40, TILE_ROCK},
            {29, 47, TILE_ROCK},
            {27, 55, TILE_ROCK},
            {30, 63, TILE_ROCK},
            {28, 74, TILE_ROCK},
            {32, 82, TILE_TREE},
            {38, 45, TILE_TREE},
            {40, 52, TILE_ROCK},
            {37, 59, TILE_TREE},
            {42, 67, TILE_ROCK},
            {39, 76, TILE_TREE},
            {41, 84, TILE_ROCK},
            {22, 35, TILE_TREE},
            {29, 33, TILE_ROCK},
            {35, 35, TILE_ROCK},
            {43, 36, TILE_TREE},
            {19, 89, TILE_ROCK},
            {26, 91, TILE_TREE},
            {34, 88, TILE_ROCK},
            {42, 90, TILE_TREE},
            {47, 87, TILE_ROCK},
            {22, 68, TILE_TREE},
            {30, 74, TILE_ROCK},
            {36, 78, TILE_ROCK},
            {43, 60, TILE_TREE},
            {47, 82, TILE_ROCK}
        };

        SetExteriorSeedGroup(map,
                             kWestRouteSeeds,
                             sizeof(kWestRouteSeeds) / sizeof(kWestRouteSeeds[0]),
                             true);
    }

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

    /* East route: turn the levees into offset banks so the swamp reads less like editor walls. */
    {
        static const ExteriorPropSeed kEastRouteSeeds[] = {
            {95, 33, TILE_TREE},
            {96, 39, TILE_TREE},
            {94, 52, TILE_TREE},
            {96, 58, TILE_TREE},
            {95, 67, TILE_TREE},
            {97, 74, TILE_ROCK},
            {95, 82, TILE_TREE},
            {102, 36, TILE_ROCK},
            {104, 42, TILE_TREE},
            {102, 50, TILE_ROCK},
            {104, 57, TILE_TREE},
            {105, 63, TILE_TREE},
            {103, 69, TILE_ROCK},
            {106, 76, TILE_TREE},
            {117, 34, TILE_ROCK},
            {119, 41, TILE_TREE},
            {116, 50, TILE_ROCK},
            {118, 60, TILE_TREE},
            {117, 68, TILE_ROCK},
            {119, 80, TILE_TREE},
            {117, 84, TILE_ROCK},
            {98, 30, TILE_ROCK},
            {106, 31, TILE_TREE},
            {113, 29, TILE_ROCK},
            {100, 88, TILE_ROCK},
            {109, 86, TILE_ROCK},
            {116, 89, TILE_ROCK},
            {102, 46, TILE_TREE},
            {104, 66, TILE_TREE},
            {114, 71, TILE_ROCK},
            {118, 79, TILE_TREE}
        };

        SetExteriorSeedGroup(map,
                             kEastRouteSeeds,
                             sizeof(kEastRouteSeeds) / sizeof(kEastRouteSeeds[0]),
                             true);
    }

    /* South route: swap the ruler-straight deck edge for staggered collapse debris. */
    {
        static const ExteriorPropSeed kSouthNaturalSeeds[] = {
            {61, 91, TILE_ROCK},
            {66, 93, TILE_TREE},
            {72, 90, TILE_ROCK},
            {78, 92, TILE_TREE},
            {86, 91, TILE_ROCK},
            {94, 93, TILE_ROCK},
            {102, 91, TILE_TREE},
            {109, 93, TILE_ROCK},
            {116, 92, TILE_ROCK},
            {122, 91, TILE_TREE}
        };
        static const ExteriorPropSeed kSouthFacilitySeeds[] = {
            {85, 94, TILE_ROCK},
            {84, 97, TILE_ROCK},
            {86, 100, TILE_ROCK},
            {99, 95, TILE_ROCK},
            {98, 99, TILE_ROCK},
            {100, 102, TILE_ROCK},
            {111, 94, TILE_ROCK},
            {110, 98, TILE_ROCK},
            {112, 101, TILE_ROCK},
            {88, 103, TILE_ROCK},
            {96, 102, TILE_ROCK},
            {105, 103, TILE_ROCK},
            {114, 102, TILE_ROCK},
            {120, 103, TILE_ROCK},
            {76, 97, TILE_ROCK},
            {91, 98, TILE_ROCK},
            {106, 96, TILE_ROCK}
        };

        SetExteriorSeedGroup(map,
                             kSouthNaturalSeeds,
                             sizeof(kSouthNaturalSeeds) / sizeof(kSouthNaturalSeeds[0]),
                             true);
        SetExteriorSeedGroup(map,
                             kSouthFacilitySeeds,
                             sizeof(kSouthFacilitySeeds) / sizeof(kSouthFacilitySeeds[0]),
                             false);
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
