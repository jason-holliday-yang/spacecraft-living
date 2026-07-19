#include "map.h"
#include "map_internal.h"

#include <string.h>

const MapUnlock *Map_GetUnlockById(const GameMap *map, const char *unlockId) {
    int index;

    if (map == NULL || unlockId == NULL || unlockId[0] == '\0' || !map->hasUnlockLayer) {
        return NULL;
    }
    for (index = 0; index < map->unlockCount; index++) {
        if (strcmp(map->unlocks[index].id, unlockId) == 0) {
            return &map->unlocks[index];
        }
    }
    return NULL;
}

const MapUnlock *Map_GetUnlockAt(const GameMap *map, int gridX, int gridY) {
    int index;

    if (map == NULL || !map->hasUnlockLayer || !Map_IsWithinMapBounds(map, gridX, gridY)) {
        return NULL;
    }
    for (index = 0; index < map->unlockCount; index++) {
        const MapUnlock *unlock;

        unlock = &map->unlocks[index];
        if (MapInternal_IsRectBounds(gridX, gridY, unlock->gridX, unlock->gridY, unlock->width, unlock->height)) {
            return unlock;
        }
    }
    return NULL;
}


static bool IsBarrierTile(TileType tile) {
    return tile == TILE_BARRIER_SWAMP
        || tile == TILE_BARRIER_DEEP
        || tile == TILE_BARRIER_RUINS;
}

static bool HasPropTile(const GameMap *map, TileType tile) {
    int y;
    int x;

    if (map == NULL) {
        return false;
    }
    for (y = 0; y < map->height; y++) {
        for (x = 0; x < map->width; x++) {
            if (map->propTiles[y][x] == tile) {
                return true;
            }
        }
    }
    return false;
}

static bool ClearPropTile(GameMap *map, TileType tile) {
    bool found;
    int y;
    int x;

    if (map == NULL) {
        return false;
    }
    found = false;
    for (y = 0; y < map->height; y++) {
        for (x = 0; x < map->width; x++) {
            if (map->propTiles[y][x] == tile) {
                map->propTiles[y][x] = TILE_VOID;
                found = true;
            }
        }
    }
    return found;
}

static void RestoreAnchoredRect(GameMap *map,
                                MapAnchor anchor,
                                int anchorOffsetX,
                                int anchorOffsetY,
                                int width,
                                int height,
                                int legacyX,
                                int legacyY,
                                TileType tile) {
    int originX;
    int originY;
    int y;
    int x;

    if (map == NULL) {
        return;
    }
    if (Map_GetAnchorPosition(map, anchor, &originX, &originY)) {
        originX += anchorOffsetX;
        originY += anchorOffsetY;
    } else {
        originX = legacyX;
        originY = legacyY;
    }
    for (y = originY; y < originY + height; y++) {
        for (x = originX; x < originX + width; x++) {
            if (Map_IsWithinMapBounds(map, x, y)) {
                map->propTiles[y][x] = tile;
            }
        }
    }
}

static const MapUnlock *FindUnlockByTile(const GameMap *map, int gridX, int gridY, TileType tile) {
    const MapUnlock *unlock;

    unlock = Map_GetUnlockAt(map, gridX, gridY);
    if (unlock != NULL && unlock->clearsTile == tile) {
        return unlock;
    }
    return NULL;
}

static void SetUnlockRectOpen(GameMap *map, const MapUnlock *unlock, bool open) {
    int y;
    int x;

    if (map == NULL || unlock == NULL) {
        return;
    }
    for (y = unlock->gridY; y < unlock->gridY + unlock->height; y++) {
        for (x = unlock->gridX; x < unlock->gridX + unlock->width; x++) {
            if (!Map_IsWithinMapBounds(map, x, y)) {
                continue;
            }
            if (open) {
                if (map->propTiles[y][x] == unlock->clearsTile) {
                    map->propTiles[y][x] = TILE_VOID;
                }
            } else {
                map->propTiles[y][x] = unlock->clearsTile;
            }
        }
    }
}

bool Map_SetUnlockOpen(GameMap *map, const char *unlockId, bool open) {
    const MapUnlock *unlock;

    unlock = Map_GetUnlockById(map, unlockId);
    if (unlock == NULL) {
        return false;
    }
    SetUnlockRectOpen(map, unlock, open);
    return true;
}

bool Map_IsUnlockOpen(const GameMap *map, const char *unlockId) {
    const MapUnlock *unlock;
    int y;
    int x;

    unlock = Map_GetUnlockById(map, unlockId);
    if (unlock == NULL) {
        return false;
    }
    for (y = unlock->gridY; y < unlock->gridY + unlock->height; y++) {
        for (x = unlock->gridX; x < unlock->gridX + unlock->width; x++) {
            if (Map_IsWithinMapBounds(map, x, y) && map->propTiles[y][x] == unlock->clearsTile) {
                return false;
            }
        }
    }
    return true;
}

void Map_UnlockSwampOuter(GameMap *map) {
    if (map == NULL || Map_SetUnlockOpen(map, "AIRLOCK_DOOR", true)) {
        return;
    }
    if (map->mapKind != MAP_KIND_LEGACY && map->mapKind != MAP_KIND_UNKNOWN) {
        return;
    }
    /* Missing Unlock metadata: use the actual Props layer before consulting
     * any legacy geometry.
     */
    ClearPropTile(map, TILE_AIRLOCK_DOOR);
}

void Map_LockSwampOuter(GameMap *map) {
    if (map == NULL || Map_SetUnlockOpen(map, "AIRLOCK_DOOR", false)) {
        return;
    }
    if (map->mapKind != MAP_KIND_LEGACY && map->mapKind != MAP_KIND_UNKNOWN) {
        return;
    }
    RestoreAnchoredRect(map,
                        MAP_ANCHOR_AIRLOCK_DOOR,
                        0,
                        -(AIRLOCK_DOOR_HEIGHT / 2),
                        1,
                        AIRLOCK_DOOR_HEIGHT,
                        AIRLOCK_DOOR_X,
                        AIRLOCK_DOOR_TOP_Y,
                        TILE_AIRLOCK_DOOR);
}

bool Map_IsSwampOuterUnlocked(const GameMap *map) {
    if (map == NULL) {
        return false;
    }
    if (Map_GetUnlockById(map, "AIRLOCK_DOOR") != NULL) {
        return Map_IsUnlockOpen(map, "AIRLOCK_DOOR");
    }
    if (map->mapKind != MAP_KIND_LEGACY && map->mapKind != MAP_KIND_UNKNOWN) {
        return true;
    }
    return !HasPropTile(map, TILE_AIRLOCK_DOOR);
}

void Map_UnlockLoxiRoom(GameMap *map) {
    if (map == NULL || Map_SetUnlockOpen(map, "LOXI_ROOM_DOOR", true)) {
        return;
    }
    ClearPropTile(map, TILE_LOXI_ROOM_DOOR);
}

void Map_LockLoxiRoom(GameMap *map) {
    if (map == NULL || Map_SetUnlockOpen(map, "LOXI_ROOM_DOOR", false)) {
        return;
    }
    RestoreAnchoredRect(map,
                        MAP_ANCHOR_LOXI_TERMINAL,
                        0,
                        LOXI_ROOM_DOOR_Y - LOXI_TERMINAL_Y,
                        LOXI_ROOM_DOOR_WIDTH,
                        1,
                        LOXI_ROOM_DOOR_X,
                        LOXI_ROOM_DOOR_Y,
                        TILE_LOXI_ROOM_DOOR);
}

bool Map_IsLoxiRoomUnlocked(const GameMap *map) {
    if (map == NULL) {
        return false;
    }
    if (Map_GetUnlockById(map, "LOXI_ROOM_DOOR") != NULL) {
        return Map_IsUnlockOpen(map, "LOXI_ROOM_DOOR");
    }
    return !HasPropTile(map, TILE_LOXI_ROOM_DOOR);
}

void Map_UnlockSwampDeep(GameMap *map) {
    if (map == NULL || Map_SetUnlockOpen(map, "SWAMP_DEEP_GATE", true)) {
        return;
    }
    ClearPropTile(map, TILE_BARRIER_DEEP);
}

void Map_UnlockRuins(GameMap *map) {
    if (map == NULL || Map_SetUnlockOpen(map, "RUINS_GATE", true)) {
        return;
    }
    ClearPropTile(map, TILE_BARRIER_RUINS);
}

void Map_SetFieldCamp(GameMap *map, int gridX, int gridY) {
    if (map == NULL) {
        return;
    }
    map->campPlaced = true;
    map->campX = gridX;
    map->campY = gridY;
}

bool Map_CanCrossWithRope(const GameMap *map, int fromX, int fromY, int toX, int toY) {
    TileType toTile;
    int dx;
    int dy;

    if (!Map_IsWithinMapBounds(map, fromX, fromY) || !Map_IsWithinMapBounds(map, toX, toY)) {
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
    return IsBarrierTile(toTile);
}

void Map_CreateRopeBridge(GameMap *map, int gridX, int gridY) {
    TileType tile;
    const MapUnlock *unlock;

    if (!Map_IsWithinMapBounds(map, gridX, gridY)) {
        return;
    }

    tile = map->propTiles[gridY][gridX];
    unlock = FindUnlockByTile(map, gridX, gridY, tile);
    if (unlock != NULL) {
        SetUnlockRectOpen(map, unlock, true);
        return;
    }
    if (IsBarrierTile(tile)) {
        map->propTiles[gridY][gridX] = TILE_VOID;
    }
}
