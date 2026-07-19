#include "map.h"

/* Legacy-only furniture origins. Tiled Props data is always consulted first,
 * and keyed facilities fall back to Anchors before these coordinates matter.
 */
static const int kBunkOrigins[][2] = {{45, 44}};
static const int kStorageLockerOrigins[][2] = {{42, 51}};
static const int kTechTableOrigins[][2] = {{45, 58}};

static bool ResolveRectBounds(int gridX,
                              int gridY,
                              int candidateOriginX,
                              int candidateOriginY,
                              int candidateWidth,
                              int candidateHeight,
                              int *originX,
                              int *originY,
                              int *width,
                              int *height) {
    if (candidateWidth <= 0
        || candidateHeight <= 0
        || gridX < candidateOriginX
        || gridX >= candidateOriginX + candidateWidth
        || gridY < candidateOriginY
        || gridY >= candidateOriginY + candidateHeight) {
        return false;
    }

    *originX = candidateOriginX;
    *originY = candidateOriginY;
    *width = candidateWidth;
    *height = candidateHeight;
    return true;
}

static bool PropRowMatches(const GameMap *map, TileType tile, int left, int right, int gridY) {
    int gridX;

    if (map == NULL || gridY < 0 || gridY >= map->height) {
        return false;
    }
    for (gridX = left; gridX <= right; gridX++) {
        if (Map_GetPropTileAt(map, gridX, gridY) != tile) {
            return false;
        }
    }
    return true;
}

static bool ResolveMapPropRectangle(const GameMap *map,
                                    TileType tile,
                                    int gridX,
                                    int gridY,
                                    int *originX,
                                    int *originY,
                                    int *width,
                                    int *height) {
    int left;
    int right;
    int top;
    int bottom;

    if (map == NULL
        || !Map_IsWithinMapBounds(map, gridX, gridY)
        || Map_GetPropTileAt(map, gridX, gridY) != tile) {
        return false;
    }

    left = gridX;
    while (left > 0 && Map_GetPropTileAt(map, left - 1, gridY) == tile) {
        left--;
    }
    right = gridX;
    while (right + 1 < map->width && Map_GetPropTileAt(map, right + 1, gridY) == tile) {
        right++;
    }

    top = gridY;
    while (top > 0 && PropRowMatches(map, tile, left, right, top - 1)) {
        top--;
    }
    bottom = gridY;
    while (bottom + 1 < map->height && PropRowMatches(map, tile, left, right, bottom + 1)) {
        bottom++;
    }

    if (left == right && top == bottom) {
        return false;
    }

    *originX = left;
    *originY = top;
    *width = right - left + 1;
    *height = bottom - top + 1;
    return true;
}

static void GetMapAnchorOrFallback(const GameMap *map,
                                   MapAnchor anchor,
                                   int fallbackX,
                                   int fallbackY,
                                   int *gridX,
                                   int *gridY) {
    if (!Map_GetAnchorPosition(map, anchor, gridX, gridY)) {
        *gridX = fallbackX;
        *gridY = fallbackY;
    }
}

static bool ResolveAnchoredRectBounds(const GameMap *map,
                                      MapAnchor anchor,
                                      int fallbackX,
                                      int fallbackY,
                                      int fallbackWidth,
                                      int fallbackHeight,
                                      int gridX,
                                      int gridY,
                                      int *originX,
                                      int *originY,
                                      int *width,
                                      int *height) {
    int anchorX;
    int anchorY;

    GetMapAnchorOrFallback(map, anchor, fallbackX, fallbackY, &anchorX, &anchorY);
    return ResolveRectBounds(gridX,
                             gridY,
                             anchorX,
                             anchorY,
                             fallbackWidth,
                             fallbackHeight,
                             originX,
                             originY,
                             width,
                             height);
}

static bool ResolveRectBoundsFromOrigins(int gridX,
                                         int gridY,
                                         const int (*origins)[2],
                                         int originCount,
                                         int size,
                                         int *originX,
                                         int *originY,
                                         int *width,
                                         int *height) {
    int index;

    for (index = 0; index < originCount; index++) {
        if (ResolveRectBounds(gridX,
                              gridY,
                              origins[index][0],
                              origins[index][1],
                              size,
                              size,
                              originX,
                              originY,
                              width,
                              height)) {
            return true;
        }
    }
    return false;
}

bool Map_GetMultiTilePropBounds(const GameMap *map,
                                TileType tile,
                                int gridX,
                                int gridY,
                                int *originX,
                                int *originY,
                                int *width,
                                int *height) {
    if (originX == NULL || originY == NULL || width == NULL || height == NULL) {
        return false;
    }
    if (ResolveMapPropRectangle(map, tile, gridX, gridY, originX, originY, width, height)) {
        return true;
    }

    switch (tile) {
        case TILE_TECH_TABLE:
            return ResolveRectBoundsFromOrigins(gridX, gridY, kTechTableOrigins,
                                                (int)(sizeof(kTechTableOrigins) / sizeof(kTechTableOrigins[0])),
                                                SHIP_FURNITURE_FOOTPRINT_SIZE,
                                                originX, originY, width, height);
        case TILE_STORAGE_LOCKER:
            return ResolveRectBoundsFromOrigins(gridX, gridY, kStorageLockerOrigins,
                                                (int)(sizeof(kStorageLockerOrigins) / sizeof(kStorageLockerOrigins[0])),
                                                SHIP_FURNITURE_FOOTPRINT_SIZE,
                                                originX, originY, width, height);
        case TILE_BUNK:
            return ResolveRectBoundsFromOrigins(gridX, gridY, kBunkOrigins,
                                                (int)(sizeof(kBunkOrigins) / sizeof(kBunkOrigins[0])),
                                                SHIP_FURNITURE_FOOTPRINT_SIZE,
                                                originX, originY, width, height);
        case TILE_OXYGEN_CONSOLE:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_OXYGEN_CONSOLE,
                                             OXYGEN_CONSOLE_X, OXYGEN_CONSOLE_Y,
                                             STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_LOXI_TERMINAL:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_LOXI_TERMINAL,
                                             LOXI_TERMINAL_X, LOXI_TERMINAL_Y,
                                             STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_WORKBENCH:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_WORKBENCH,
                                             WORKBENCH_X, WORKBENCH_Y,
                                             STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_AIRLOCK_CONSOLE:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_AIRLOCK_CONSOLE,
                                             AIRLOCK_CONSOLE_X, AIRLOCK_CONSOLE_Y,
                                             STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_ENERGY_CONSOLE:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_ENERGY_CONSOLE,
                                             ENERGY_CONSOLE_X, ENERGY_CONSOLE_Y,
                                             STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_COMM_RELAY:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_COMM_RELAY,
                                             COMM_RELAY_X, COMM_RELAY_Y,
                                             WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_CRASH_CLUE:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_CRASH_CLUE,
                                             CRASH_CLUE_X, CRASH_CLUE_Y,
                                             CRASH_CLUE_FOOTPRINT_SIZE, CRASH_CLUE_FOOTPRINT_SIZE,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_SIGNAL_TOWER:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_SIGNAL_TOWER,
                                             SIGNAL_TOWER_X, SIGNAL_TOWER_Y,
                                             WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE,
                                             gridX, gridY, originX, originY, width, height);
        case TILE_MONOLITH:
            return ResolveAnchoredRectBounds(map, MAP_ANCHOR_MONOLITH_A,
                                             MONOLITH_A_X, MONOLITH_A_Y,
                                             WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE,
                                             gridX, gridY, originX, originY, width, height)
                || ResolveAnchoredRectBounds(map, MAP_ANCHOR_MONOLITH_B,
                                             MONOLITH_B_X, MONOLITH_B_Y,
                                             WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE,
                                             gridX, gridY, originX, originY, width, height)
                || ResolveAnchoredRectBounds(map, MAP_ANCHOR_MONOLITH_C,
                                             MONOLITH_C_X, MONOLITH_C_Y,
                                             WORLD_INTERACTIVE_FOOTPRINT_SIZE, WORLD_INTERACTIVE_FOOTPRINT_SIZE,
                                             gridX, gridY, originX, originY, width, height);
        default:
            return false;
    }
}
