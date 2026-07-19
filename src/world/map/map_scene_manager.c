#include "map_scene_manager.h"

#include <stdio.h>
#include <string.h>

#define MAP_RUNTIME_DYNAMIC_TILE_MAX 256

static bool IsTrackedRuntimeTile(const GameMap *map, int gridX, int gridY) {
    return Map_GetUnlockAt(map, gridX, gridY) != NULL;
}

MapRuntimeState *MapSceneManager_GetMapState(MapSceneManager *manager, const char *mapId, bool create) {
    int index;
    if (manager == NULL || mapId == NULL || mapId[0] == '\0') return NULL;
    for (index = 0; index < manager->runtimeStateCount; index++) {
        if (strcmp(manager->runtimeStates[index].mapId, mapId) == 0) return &manager->runtimeStates[index];
    }
    if (!create || manager->runtimeStateCount >= MAX_MAP_CATALOG_ENTRIES) return NULL;
    index = manager->runtimeStateCount++;
    memset(&manager->runtimeStates[index], 0, sizeof(manager->runtimeStates[index]));
    snprintf(manager->runtimeStates[index].mapId, MAP_ID_MAX, "%s", mapId);
    return &manager->runtimeStates[index];
}

const MapRuntimeState *MapSceneManager_FindMapState(const MapSceneManager *manager, const char *mapId) {
    int index;
    if (manager == NULL || mapId == NULL) return NULL;
    for (index = 0; index < manager->runtimeStateCount; index++) {
        if (strcmp(manager->runtimeStates[index].mapId, mapId) == 0) return &manager->runtimeStates[index];
    }
    return NULL;
}

void MapSceneManager_CaptureMapState(MapSceneManager *manager, const GameMap *map) {
    MapRuntimeState *state;
    int index;
    int y;
    int x;
    if (manager == NULL || map == NULL || map->mapId[0] == '\0') return;
    state = MapSceneManager_GetMapState(manager, map->mapId, true);
    if (state == NULL) return;
    state->contentVersion = map->contentVersion;
    state->mapWidth = map->width;
    state->mapHeight = map->height;
    state->campPlaced = map->campPlaced;
    state->campX = map->campX;
    state->campY = map->campY;
    state->unlockCount = map->unlockCount;
    if (state->unlockCount > MAX_MAP_UNLOCKS) state->unlockCount = MAX_MAP_UNLOCKS;
    for (index = 0; index < state->unlockCount; index++) {
        snprintf(state->unlocks[index].unlockId, MAP_UNLOCK_ID_MAX, "%s", map->unlocks[index].id);
        state->unlocks[index].open = Map_IsUnlockOpen(map, map->unlocks[index].id);
    }
    state->clearedDynamicTileCount = 0;
    for (y = 0; y < map->height; y++) {
        for (x = 0; x < map->width; x++) {
            if (IsTrackedRuntimeTile(map, x, y) && Map_GetPropTileAt(map, x, y) == TILE_VOID
                && state->clearedDynamicTileCount < MAP_RUNTIME_DYNAMIC_TILE_MAX) {
                int slot = state->clearedDynamicTileCount++;
                state->clearedDynamicTileX[slot] = x;
                state->clearedDynamicTileY[slot] = y;
            }
        }
    }
}

void MapSceneManager_ApplyMapState(const MapSceneManager *manager, GameMap *map) {
    const MapRuntimeState *state;
    int index;
    if (manager == NULL || map == NULL) return;
    state = MapSceneManager_FindMapState(manager, map->mapId);
    if (state == NULL) return;
    for (index = 0; index < state->unlockCount; index++) {
        Map_SetUnlockOpen(map, state->unlocks[index].unlockId, state->unlocks[index].open);
    }
    for (index = 0; index < state->clearedDynamicTileCount; index++) {
        int x = state->clearedDynamicTileX[index];
        int y = state->clearedDynamicTileY[index];
        if (Map_IsWithinMapBounds(map, x, y) && IsTrackedRuntimeTile(map, x, y)) {
            map->propTiles[y][x] = TILE_VOID;
        }
    }
    if (state->campPlaced && Map_IsWithinMapBounds(map, state->campX, state->campY)) {
        Map_SetFieldCamp(map, state->campX, state->campY);
    }
}

static int AbsInt(int value) {
    return value < 0 ? -value : value;
}

static int SignInt(int value) {
    return value < 0 ? -1 : (value > 0 ? 1 : 0);
}

static bool IsSafeSpawn(const GameMap *map, int gridX, int gridY) {
    return Map_IsWithinMapBounds(map, gridX, gridY)
        && Map_IsWalkable(map, gridX, gridY)
        && Map_GetHazardAt(map, gridX, gridY) == HAZARD_NONE;
}

static bool FindNearestSafeSpawn(const GameMap *map,
                                 int originX,
                                 int originY,
                                 int *gridX,
                                 int *gridY) {
    int radius;
    int maxRadius;

    if (IsSafeSpawn(map, originX, originY)) {
        *gridX = originX;
        *gridY = originY;
        return true;
    }

    maxRadius = map->width + map->height;
    for (radius = 1; radius <= maxRadius; radius++) {
        int offsetY;

        for (offsetY = -radius; offsetY <= radius; offsetY++) {
            int offsetX;
            int candidateX;
            int candidateY;

            offsetX = radius - AbsInt(offsetY);
            candidateY = originY + offsetY;
            candidateX = originX - offsetX;
            if (IsSafeSpawn(map, candidateX, candidateY)) {
                *gridX = candidateX;
                *gridY = candidateY;
                return true;
            }
            if (offsetX == 0) {
                continue;
            }
            candidateX = originX + offsetX;
            if (IsSafeSpawn(map, candidateX, candidateY)) {
                *gridX = candidateX;
                *gridY = candidateY;
                return true;
            }
        }
    }

    return false;
}

static void ResolveArrivalFacing(MapSceneManager *manager, const GameMap *targetMap) {
    int index;

    manager->stagedFacingX = 0;
    manager->stagedFacingY = 1;
    for (index = 0; index < targetMap->portalCount; index++) {
        const MapPortal *portal;
        int portalCenterX;
        int portalCenterY;
        int deltaX;
        int deltaY;

        portal = &targetMap->portals[index];
        if (strcmp(portal->targetMapId, manager->currentMapId) != 0) {
            continue;
        }
        portalCenterX = portal->gridX + portal->width / 2;
        portalCenterY = portal->gridY + portal->height / 2;
        deltaX = portalCenterX - manager->stagedSpawnX;
        deltaY = portalCenterY - manager->stagedSpawnY;
        if (AbsInt(deltaX) >= AbsInt(deltaY) && deltaX != 0) {
            manager->stagedFacingX = SignInt(deltaX);
            manager->stagedFacingY = 0;
        } else if (deltaY != 0) {
            manager->stagedFacingX = 0;
            manager->stagedFacingY = SignInt(deltaY);
        } else if (strcmp(manager->transitionId, "enter_ship") == 0) {
            manager->stagedFacingX = 1;
            manager->stagedFacingY = 0;
        }
        return;
    }
}

void MapSceneManager_SetError(MapSceneManager *manager, const char *message) {
    if (manager == NULL) {
        return;
    }
    manager->phase = MAP_TRANSITION_ERROR;
    snprintf(manager->loadError,
             sizeof(manager->loadError),
             "%s",
             message != NULL && message[0] != '\0' ? message : "Unknown map transition error");
    Map_Destroy(&manager->stagedMap);
}

bool MapSceneManager_Init(MapSceneManager *manager,
                          const GameMap *activeMap,
                          const char *catalogPath) {
    if (manager == NULL) {
        return false;
    }

    memset(manager, 0, sizeof(*manager));
    manager->phase = MAP_TRANSITION_IDLE;
    manager->stagedSpawnX = -1;
    manager->stagedSpawnY = -1;
    if (!MapCatalog_Load(&manager->catalog,
                         catalogPath != NULL ? catalogPath : "maps/map_catalog.json")) {
        MapSceneManager_SetError(manager, MapCatalog_GetLoadError(&manager->catalog));
        return false;
    }
    MapSceneManager_BindActiveMap(manager, activeMap);
    return true;
}

void MapSceneManager_Destroy(MapSceneManager *manager) {
    if (manager == NULL) {
        return;
    }
    Map_Destroy(&manager->stagedMap);
    memset(manager, 0, sizeof(*manager));
}

void MapSceneManager_BindActiveMap(MapSceneManager *manager, const GameMap *activeMap) {
    if (manager == NULL || activeMap == NULL) {
        return;
    }
    snprintf(manager->currentMapId, sizeof(manager->currentMapId), "%s", activeMap->mapId);
    if (manager->phase != MAP_TRANSITION_ERROR) {
        manager->phase = MAP_TRANSITION_IDLE;
    }
}

bool MapSceneManager_RequestPortal(MapSceneManager *manager,
                                   const GameMap *activeMap,
                                   const MapPortal *portal) {
    if (manager == NULL || activeMap == NULL || portal == NULL || MapSceneManager_IsBusy(manager)) {
        return false;
    }
    if (portal->targetMapId[0] == '\0' || portal->targetAnchor[0] == '\0'
        || portal->transitionId[0] == '\0') {
        MapSceneManager_SetError(manager, "Portal is missing target map, target Anchor, or transition ID");
        return false;
    }
    if (MapCatalog_Find(&manager->catalog, portal->targetMapId) == NULL) {
        char error[MAP_LOAD_ERROR_MAX];
        snprintf(error, sizeof(error), "Portal '%s' targets unknown map ID '%s'",
                 portal->portalId, portal->targetMapId);
        MapSceneManager_SetError(manager, error);
        return false;
    }

    Map_Destroy(&manager->stagedMap);
    manager->loadError[0] = '\0';
    snprintf(manager->currentMapId, sizeof(manager->currentMapId), "%s", activeMap->mapId);
    snprintf(manager->pendingMapId, sizeof(manager->pendingMapId), "%s", portal->targetMapId);
    snprintf(manager->targetAnchor, sizeof(manager->targetAnchor), "%s", portal->targetAnchor);
    snprintf(manager->transitionId, sizeof(manager->transitionId), "%s", portal->transitionId);
    snprintf(manager->sourcePortalId, sizeof(manager->sourcePortalId), "%s", portal->portalId);
    manager->phase = MAP_TRANSITION_REQUESTED;
    return true;
}

void MapSceneManager_MarkFadingOut(MapSceneManager *manager) {
    if (manager != NULL && manager->phase == MAP_TRANSITION_REQUESTED) {
        manager->phase = MAP_TRANSITION_FADING_OUT;
    }
}

bool MapSceneManager_PreparePending(MapSceneManager *manager, const GameMap *activeMap) {
    int anchorX;
    int anchorY;
    char error[MAP_LOAD_ERROR_MAX];

    if (manager == NULL || activeMap == NULL
        || (manager->phase != MAP_TRANSITION_FADING_OUT
            && manager->phase != MAP_TRANSITION_REQUESTED)) {
        return false;
    }

    MapSceneManager_CaptureMapState(manager, activeMap);
    manager->phase = MAP_TRANSITION_LOADING;
    Map_Destroy(&manager->stagedMap);
    if (!Map_LoadById(&manager->stagedMap, &manager->catalog, manager->pendingMapId)) {
        snprintf(error, sizeof(error), "%s", Map_GetLoadError(&manager->stagedMap));
        MapSceneManager_SetError(manager, error);
        return false;
    }
    if (!Map_IsSceneReady(&manager->stagedMap)) {
        snprintf(error, sizeof(error), "Map '%s' is missing one or more required scene layers",
                 manager->pendingMapId);
        MapSceneManager_SetError(manager, error);
        return false;
    }
    MapSceneManager_ApplyMapState(manager, &manager->stagedMap);
    if (!Map_GetAnchorPositionByName(&manager->stagedMap,
                                     manager->targetAnchor,
                                     &anchorX,
                                     &anchorY)) {
        snprintf(error, sizeof(error), "Map '%s' does not contain target Anchor '%s'",
                 manager->pendingMapId, manager->targetAnchor);
        MapSceneManager_SetError(manager, error);
        return false;
    }
    if (!FindNearestSafeSpawn(&manager->stagedMap,
                              anchorX,
                              anchorY,
                              &manager->stagedSpawnX,
                              &manager->stagedSpawnY)) {
        snprintf(error, sizeof(error), "Map '%s' has no safe spawn near Anchor '%s'",
                 manager->pendingMapId, manager->targetAnchor);
        MapSceneManager_SetError(manager, error);
        return false;
    }

    ResolveArrivalFacing(manager, &manager->stagedMap);
    manager->phase = MAP_TRANSITION_READY_TO_SWAP;
    return true;
}

bool MapSceneManager_CommitPending(MapSceneManager *manager, GameMap *activeMap) {
    GameMap previousMap;

    if (manager == NULL || activeMap == NULL || manager->phase != MAP_TRANSITION_READY_TO_SWAP) {
        return false;
    }

    manager->phase = MAP_TRANSITION_SWAPPING;
    previousMap = *activeMap;
    *activeMap = manager->stagedMap;
    memset(&manager->stagedMap, 0, sizeof(manager->stagedMap));
    snprintf(manager->previousMapId, sizeof(manager->previousMapId), "%s", previousMap.mapId);
    snprintf(manager->currentMapId, sizeof(manager->currentMapId), "%s", activeMap->mapId);
    Map_ActivateRuntime(activeMap);
    Map_Destroy(&previousMap);
    manager->phase = MAP_TRANSITION_FADING_IN;
    return true;
}

void MapSceneManager_FinishTransition(MapSceneManager *manager) {
    if (manager == NULL) {
        return;
    }
    if (manager->phase == MAP_TRANSITION_FADING_IN) {
        manager->phase = MAP_TRANSITION_IDLE;
        manager->pendingMapId[0] = '\0';
        manager->targetAnchor[0] = '\0';
        manager->transitionId[0] = '\0';
        manager->sourcePortalId[0] = '\0';
    }
}

bool MapSceneManager_IsBusy(const MapSceneManager *manager) {
    return manager != NULL
        && manager->phase != MAP_TRANSITION_IDLE
        && manager->phase != MAP_TRANSITION_ERROR;
}

const char *MapSceneManager_GetError(const MapSceneManager *manager) {
    return manager != NULL ? manager->loadError : "";
}

const char *MapTransitionPhase_ToString(MapTransitionPhase phase) {
    switch (phase) {
        case MAP_TRANSITION_IDLE: return "idle";
        case MAP_TRANSITION_REQUESTED: return "requested";
        case MAP_TRANSITION_FADING_OUT: return "fading_out";
        case MAP_TRANSITION_LOADING: return "loading";
        case MAP_TRANSITION_READY_TO_SWAP: return "ready_to_swap";
        case MAP_TRANSITION_SWAPPING: return "swapping";
        case MAP_TRANSITION_FADING_IN: return "fading_in";
        case MAP_TRANSITION_ERROR: return "error";
        default: return "unknown";
    }
}
