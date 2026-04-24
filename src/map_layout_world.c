#include "map_internal.h"

static const int kCrashVoidX = 39;
static const int kCrashVoidY = 42;
static const int kCrashVoidWidth = 45;
static const int kCrashVoidHeight = 21;

typedef struct {
    int x;
    int y;
    TileType tile;
} ExteriorPropSeed;

static void FillExteriorGroundRect(GameMap *map, int x, int y, int width, int height, TileType tile) {
    MapInternal_FillGroundRect(map, EXTERIOR_X(x), EXTERIOR_Y(y), EXTERIOR_SIZE(width), EXTERIOR_SIZE(height), tile);
}

static void ClearExteriorPropRect(GameMap *map, int x, int y, int width, int height) {
    MapInternal_FillPropRect(map, EXTERIOR_X(x), EXTERIOR_Y(y), EXTERIOR_SIZE(width), EXTERIOR_SIZE(height), TILE_VOID);
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
    const int bossArenaRight = BOSS_ARENA_X + BOSS_ARENA_WIDTH - 1;
    const int bossArenaBottom = BOSS_ARENA_Y + BOSS_ARENA_HEIGHT - 1;
    const int bossArenaEntryBottom = BOSS_ARENA_ENTRY_TOP_Y + BOSS_ARENA_ENTRY_HEIGHT - 1;

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
    MapInternal_FillGroundRect(map,
                               BOSS_ARENA_X,
                               BOSS_ARENA_Y,
                               BOSS_ARENA_WIDTH,
                               BOSS_ARENA_HEIGHT,
                               TILE_RUINS_GROUND);
    MapInternal_FillGroundRect(map,
                               BOSS_ARENA_X + BOSS_ARENA_WIDTH,
                               BOSS_ARENA_ENTRY_TOP_Y - 1,
                               3,
                               BOSS_ARENA_ENTRY_HEIGHT + 2,
                               TILE_RUINS_GROUND);

    for (column = BOSS_ARENA_X; column <= bossArenaRight; column++) {
        MapInternal_SetPropTile(map, column, BOSS_ARENA_Y, TILE_ROCK);
        MapInternal_SetPropTile(map, column, bossArenaBottom, TILE_ROCK);
    }
    for (row = BOSS_ARENA_Y; row <= bossArenaBottom; row++) {
        MapInternal_SetPropTile(map, BOSS_ARENA_X, row, TILE_ROCK);
        if (row < BOSS_ARENA_ENTRY_TOP_Y || row > bossArenaEntryBottom) {
            MapInternal_SetPropTile(map, bossArenaRight, row, TILE_ROCK);
        }
    }
    MapInternal_SetPropTile(map, BOSS_ARENA_X + 2, BOSS_ARENA_Y + 2, TILE_ROCK);
    MapInternal_SetPropTile(map, BOSS_ARENA_X + BOSS_ARENA_WIDTH - 4, BOSS_ARENA_Y + 2, TILE_ROCK);
    MapInternal_SetPropTile(map, BOSS_ARENA_X + 2, BOSS_ARENA_Y + BOSS_ARENA_HEIGHT - 4, TILE_ROCK);
    MapInternal_SetPropTile(map, BOSS_ARENA_X + BOSS_ARENA_WIDTH - 4, BOSS_ARENA_Y + BOSS_ARENA_HEIGHT - 4, TILE_ROCK);
    MapInternal_SetPropTile(map, BOSS_ARENA_X + (BOSS_ARENA_WIDTH / 2) - 3, BOSS_ARENA_Y + 3, TILE_ROCK);
    MapInternal_SetPropTile(map, BOSS_ARENA_X + (BOSS_ARENA_WIDTH / 2) + 2, bossArenaBottom - 3, TILE_ROCK);
    MapInternal_SetNaturalPropTile(map, bossArenaRight + 1, BOSS_ARENA_Y + 1, TILE_TREE);
    MapInternal_SetNaturalPropTile(map, bossArenaRight + 2, BOSS_ARENA_Y + 4, TILE_TREE);
    MapInternal_SetNaturalPropTile(map, bossArenaRight + 1, bossArenaBottom - 4, TILE_TREE);
    MapInternal_SetNaturalPropTile(map, BOSS_ARENA_X + 1, BOSS_ARENA_Y - 1, TILE_TREE);
    MapInternal_SetNaturalPropTile(map, BOSS_ARENA_X + 4, BOSS_ARENA_Y - 1, TILE_TREE);

    /* North: thicken the ruins into a real upper plateau with a broad approach shelf. */
    FillExteriorGroundRect(map, 48, 22, 36, 18, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 43, 14, 46, 14, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 40, 8, 52, 18, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 54, 6, 22, 9, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 38, 18, 10, 10, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 83, 13, 10, 12, TILE_RUINS_GROUND);

    /* East: keep a clear grass approach, then a broad relay bog, then deep swamp filling the upper-right. */
    FillExteriorGroundRect(map, 94, 34, 16, 13, TILE_SWAMP_GROUND);
    FillExteriorGroundRect(map, 96, 46, 20, 14, TILE_SWAMP_GROUND);
    FillExteriorGroundRect(map, 100, 59, 20, 13, TILE_SWAMP_GROUND);
    FillExteriorGroundRect(map, 107, 21, 19, 12, TILE_DEEP_SWAMP_GROUND);
    FillExteriorGroundRect(map, 108, 32, 18, 15, TILE_DEEP_SWAMP_GROUND);
    FillExteriorGroundRect(map, 110, 46, 16, 14, TILE_DEEP_SWAMP_GROUND);

    /* South: widen the facility into staggered decks so the archive descent uses the whole lower rim. */
    FillExteriorGroundRect(map, 62, 86, 18, 10, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 70, 92, 20, 10, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 84, 88, 18, 13, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 96, 86, 20, 14, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 108, 89, 18, 13, TILE_RUINS_GROUND);
    FillExteriorGroundRect(map, 118, 96, 8, 8, TILE_RUINS_GROUND);

    for (column = WORLD_MIN_X; column <= WORLD_MAX_X; column++) {
        TileType northTile;
        TileType southTile;

        northTile = (column >= EXTERIOR_X(44) && column <= EXTERIOR_X(92)) ? TILE_ROCK : TILE_TREE;
        southTile = (column >= EXTERIOR_X(10) && column <= EXTERIOR_X(112)) ? TILE_ROCK : TILE_TREE;
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

        westTile = row >= EXTERIOR_Y(22) && row <= EXTERIOR_Y(100) ? TILE_ROCK : TILE_TREE;
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

    /* West route: keep the west side as chunky pockets instead of thin corridors and prop clutter. */
    {
        static const ExteriorPropSeed kWestRouteSeeds[] = {
            {26, 40, TILE_ROCK},
            {29, 47, TILE_ROCK},
            {24, 58, TILE_ROCK},
            {30, 67, TILE_ROCK},
            {18, 90, TILE_ROCK},
            {33, 91, TILE_ROCK},
            {47, 90, TILE_ROCK},
            {21, 44, TILE_TREE},
            {20, 62, TILE_TREE},
            {40, 60, TILE_TREE},
            {44, 84, TILE_TREE},
            {40, 52, TILE_ROCK},
            {44, 72, TILE_ROCK}
        };

        SetExteriorSeedGroup(map,
                             kWestRouteSeeds,
                             sizeof(kWestRouteSeeds) / sizeof(kWestRouteSeeds[0]),
                             true);
    }

    ClearExteriorPropRect(map, 14, 36, 18, 16);
    ClearExteriorPropRect(map, 16, 54, 16, 16);
    ClearExteriorPropRect(map, 30, 58, 16, 16);
    ClearExteriorPropRect(map, 38, 50, 14, 16);

    /* Echo Basin should read as the whole lower-left basin, not a thin corner pocket. */
    ClearExteriorPropRect(map, 10, 78, 24, 24);
    ClearExteriorPropRect(map, 30, 78, 20, 24);
    ClearExteriorPropRect(map, 10, 94, 40, 10);
    ClearExteriorPropRect(map, 40, 84, 12, 14);
    {
        static const ExteriorPropSeed kEchoBasinRimSeeds[] = {
            {11, 79, TILE_ROCK},
            {18, 78, TILE_ROCK},
            {26, 79, TILE_ROCK},
            {33, 78, TILE_ROCK},
            {43, 80, TILE_ROCK},
            {48, 90, TILE_ROCK},
            {44, 101, TILE_ROCK},
            {30, 102, TILE_ROCK},
            {18, 101, TILE_ROCK},
            {11, 98, TILE_ROCK}
        };

        SetExteriorSeedGroup(map,
                             kEchoBasinRimSeeds,
                             sizeof(kEchoBasinRimSeeds) / sizeof(kEchoBasinRimSeeds[0]),
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

    /* East route: define the swamp edge with a few deliberate banks instead of scattered clutter. */
    {
        static const ExteriorPropSeed kEastRouteSeeds[] = {
            {93, 34, TILE_TREE},
            {94, 42, TILE_TREE},
            {94, 56, TILE_TREE},
            {95, 63, TILE_TREE},
            {98, 74, TILE_ROCK},
            {101, 35, TILE_ROCK},
            {103, 44, TILE_ROCK},
            {104, 55, TILE_ROCK},
            {106, 66, TILE_TREE},
            {108, 24, TILE_ROCK},
            {114, 22, TILE_TREE},
            {121, 24, TILE_ROCK},
            {122, 35, TILE_TREE},
            {123, 46, TILE_ROCK},
            {122, 57, TILE_TREE},
            {121, 69, TILE_ROCK},
            {112, 74, TILE_TREE},
            {118, 80, TILE_ROCK},
            {104, 82, TILE_ROCK},
            {111, 84, TILE_TREE}
        };

        SetExteriorSeedGroup(map,
                             kEastRouteSeeds,
                             sizeof(kEastRouteSeeds) / sizeof(kEastRouteSeeds[0]),
                             true);
    }

    /* South route: swap the ruler-straight deck edge for a stepped collapse spine. */
    {
        static const ExteriorPropSeed kSouthNaturalSeeds[] = {
            {58, 90, TILE_TREE},
            {61, 91, TILE_ROCK},
            {66, 93, TILE_TREE},
            {72, 90, TILE_ROCK},
            {78, 92, TILE_TREE},
            {86, 91, TILE_ROCK},
            {94, 93, TILE_ROCK},
            {102, 91, TILE_TREE},
            {109, 93, TILE_ROCK},
            {116, 92, TILE_ROCK},
            {122, 91, TILE_TREE},
            {124, 96, TILE_ROCK}
        };
        static const ExteriorPropSeed kSouthFacilitySeeds[] = {
            {72, 94, TILE_ROCK},
            {75, 99, TILE_ROCK},
            {85, 94, TILE_ROCK},
            {84, 97, TILE_ROCK},
            {86, 100, TILE_ROCK},
            {99, 95, TILE_ROCK},
            {98, 99, TILE_ROCK},
            {100, 102, TILE_ROCK},
            {103, 90, TILE_ROCK},
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
            {106, 96, TILE_ROCK},
            {118, 96, TILE_ROCK},
            {123, 100, TILE_ROCK}
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
}
