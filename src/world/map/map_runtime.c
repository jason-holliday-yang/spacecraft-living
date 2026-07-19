#include "map.h"
#include "map_anchors_internal.h"
#include "map_internal.h"
#include "map_regions_internal.h"

#include <string.h>

static int gRuntimeMapWidth = MAP_WIDTH;
static int gRuntimeMapHeight = MAP_HEIGHT;

bool Map_IsWithinBounds(int gridX, int gridY) {
    return gridX >= 0 && gridX < gRuntimeMapWidth && gridY >= 0 && gridY < gRuntimeMapHeight;
}

bool Map_IsWithinMapBounds(const GameMap *map, int gridX, int gridY) {
    return map != NULL
        && gridX >= 0
        && gridX < map->width
        && gridY >= 0
        && gridY < map->height;
}

static bool IsBlockingEntityAt(const GameMap *map, int gridX, int gridY) {
    int index;

    if (map == NULL || !map->hasEntityLayer) {
        return false;
    }

    for (index = 0; index < map->entityCount; index++) {
        const MapEntity *entity = &map->entities[index];

        if (entity->blocksMovement
            && MapInternal_IsRectBounds(gridX, gridY, entity->gridX, entity->gridY, entity->width, entity->height)) {
            return true;
        }
    }

    return false;
}

bool Map_IsWalkable(const GameMap *map, int gridX, int gridY) {
    TileType groundTile;
    TileType propTile;

    if (!Map_IsWithinMapBounds(map, gridX, gridY)) {
        return false;
    }
    if (IsBlockingEntityAt(map, gridX, gridY)) {
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
        && propTile != TILE_LOXI_ROOM_DOOR
        && propTile != TILE_COMM_RELAY
        && propTile != TILE_CRASH_CLUE
        && propTile != TILE_ENERGY_CONSOLE
        && propTile != TILE_SIGNAL_TOWER
        && propTile != TILE_MONOLITH
        && propTile != TILE_BARRIER_SWAMP
        && propTile != TILE_BARRIER_DEEP
        && propTile != TILE_BARRIER_RUINS
        && propTile != TILE_ENTITY_BLOCKER;
}

bool Map_IsOpaque(const GameMap *map, int gridX, int gridY) {
    TileType tile;

    if (IsBlockingEntityAt(map, gridX, gridY)) {
        return true;
    }

    tile = Map_GetPropTileAt(map, gridX, gridY);
    return tile == TILE_TREE
        || tile == TILE_ROCK
        || tile == TILE_STORAGE_LOCKER
        || tile == TILE_ENTITY_BLOCKER;
}

TileType Map_GetTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinMapBounds(map, gridX, gridY)) {
        return TILE_VOID;
    }
    if (map->propTiles[gridY][gridX] != TILE_VOID) {
        return map->propTiles[gridY][gridX];
    }
    return map->groundTiles[gridY][gridX];
}

TileType Map_GetGroundTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinMapBounds(map, gridX, gridY)) {
        return TILE_VOID;
    }
    return map->groundTiles[gridY][gridX];
}

TileType Map_GetPropTileAt(const GameMap *map, int gridX, int gridY) {
    if (!Map_IsWithinMapBounds(map, gridX, gridY)) {
        return TILE_VOID;
    }
    return map->propTiles[gridY][gridX];
}

HazardType Map_GetHazardAt(const GameMap *map, int gridX, int gridY) {
    if (map != NULL && map->hasHazardLayer && Map_IsWithinMapBounds(map, gridX, gridY)) {
        return map->hazardTiles[gridY][gridX];
    }
    return MapInternal_GetFallbackHazardAt(map, gridX, gridY);
}

MapDecorType Map_GetDecorAt(const GameMap *map, int gridX, int gridY) {
    if (map == NULL || !map->hasDecorLayer || !Map_IsWithinMapBounds(map, gridX, gridY)) {
        return MAP_DECOR_NONE;
    }
    return map->decorTiles[gridY][gridX];
}

Vector2 Map_GridToWorld(int gridX, int gridY) {
    return (Vector2){
        gridX * TILE_SIZE + TILE_SIZE * 0.5f,
        gridY * TILE_SIZE + TILE_SIZE * 0.5f
    };
}

const MapPortal *Map_GetPortalById(const GameMap *map, const char *portalId) {
    int index;

    if (map == NULL || portalId == NULL || portalId[0] == '\0' || !map->hasPortalLayer) {
        return NULL;
    }
    for (index = 0; index < map->portalCount; index++) {
        if (strcmp(map->portals[index].portalId, portalId) == 0) {
            return &map->portals[index];
        }
    }
    return NULL;
}

const MapPortal *Map_FindPortalNear(const GameMap *map, int gridX, int gridY) {
    const MapPortal *bestPortal;
    int bestDistance;
    int index;

    if (map == NULL || !map->hasPortalLayer) {
        return NULL;
    }
    bestPortal = NULL;
    bestDistance = 2147483647;
    for (index = 0; index < map->portalCount; index++) {
        const MapPortal *portal;
        int nearestX;
        int nearestY;
        int deltaX;
        int deltaY;
        int distance;

        portal = &map->portals[index];
        nearestX = gridX < portal->gridX ? portal->gridX
            : (gridX >= portal->gridX + portal->width ? portal->gridX + portal->width - 1 : gridX);
        nearestY = gridY < portal->gridY ? portal->gridY
            : (gridY >= portal->gridY + portal->height ? portal->gridY + portal->height - 1 : gridY);
        deltaX = gridX - nearestX;
        deltaY = gridY - nearestY;
        if (deltaX < 0) {
            deltaX = -deltaX;
        }
        if (deltaY < 0) {
            deltaY = -deltaY;
        }
        distance = deltaX + deltaY;
        if (distance <= portal->interactionRadius && distance < bestDistance) {
            bestPortal = portal;
            bestDistance = distance;
        }
    }
    return bestPortal;
}

const MapEntity *Map_GetEntityById(const GameMap *map, const char *entityId) {
    int index;

    if (map == NULL || entityId == NULL || entityId[0] == '\0' || !map->hasEntityLayer) {
        return NULL;
    }
    for (index = 0; index < map->entityCount; index++) {
        if (strcmp(map->entities[index].entityId, entityId) == 0) {
            return &map->entities[index];
        }
    }
    return NULL;
}

void Map_ActivateRuntime(const GameMap *map) {
    if (map != NULL && map->width > 0 && map->height > 0) {
        gRuntimeMapWidth = map->width;
        gRuntimeMapHeight = map->height;
    } else {
        gRuntimeMapWidth = MAP_WIDTH;
        gRuntimeMapHeight = MAP_HEIGHT;
    }

    MapInternal_ActivateRuntimeAreaLayer(map);
    MapInternal_ActivateRuntimeAnchorLayer(map);
    MapInternal_ActivateRuntimeRegionLayer(map);
}
