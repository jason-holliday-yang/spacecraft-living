#ifndef MAP_SCENE_MANAGER_H
#define MAP_SCENE_MANAGER_H

#include <stdbool.h>
#include "c_compat.h"
#include "map.h"

SCL_EXTERN_C_BEGIN

typedef enum MapTransitionPhase {
    MAP_TRANSITION_IDLE = 0,
    MAP_TRANSITION_REQUESTED,
    MAP_TRANSITION_FADING_OUT,
    MAP_TRANSITION_LOADING,
    MAP_TRANSITION_READY_TO_SWAP,
    MAP_TRANSITION_SWAPPING,
    MAP_TRANSITION_FADING_IN,
    MAP_TRANSITION_ERROR
} MapTransitionPhase;

typedef struct MapRuntimeUnlockState {
    char unlockId[MAP_UNLOCK_ID_MAX];
    bool open;
} MapRuntimeUnlockState;

typedef struct MapRuntimeState {
    char mapId[MAP_ID_MAX];
    int contentVersion;
    int mapWidth;
    int mapHeight;
    MapRuntimeUnlockState unlocks[MAX_MAP_UNLOCKS];
    int unlockCount;
    int clearedDynamicTileX[256];
    int clearedDynamicTileY[256];
    int clearedDynamicTileCount;
    bool campPlaced;
    int campX;
    int campY;
} MapRuntimeState;

typedef struct MapSceneManager {
    MapCatalog catalog;
    char currentMapId[MAP_ID_MAX];
    char previousMapId[MAP_ID_MAX];
    char pendingMapId[MAP_ID_MAX];
    char targetAnchor[MAP_ANCHOR_ID_MAX];
    char transitionId[MAP_TRANSITION_ID_MAX];
    char sourcePortalId[MAP_ID_MAX];
    MapTransitionPhase phase;
    char loadError[MAP_LOAD_ERROR_MAX];
    GameMap stagedMap;
    int stagedSpawnX;
    int stagedSpawnY;
    int stagedFacingX;
    int stagedFacingY;
    MapRuntimeState runtimeStates[MAX_MAP_CATALOG_ENTRIES];
    int runtimeStateCount;
} MapSceneManager;

bool MapSceneManager_Init(MapSceneManager *manager,
                          const GameMap *activeMap,
                          const char *catalogPath);
void MapSceneManager_Destroy(MapSceneManager *manager);
void MapSceneManager_BindActiveMap(MapSceneManager *manager, const GameMap *activeMap);
void MapSceneManager_CaptureMapState(MapSceneManager *manager, const GameMap *map);
void MapSceneManager_ApplyMapState(const MapSceneManager *manager, GameMap *map);
MapRuntimeState *MapSceneManager_GetMapState(MapSceneManager *manager, const char *mapId, bool create);
const MapRuntimeState *MapSceneManager_FindMapState(const MapSceneManager *manager, const char *mapId);
bool MapSceneManager_RequestPortal(MapSceneManager *manager,
                                   const GameMap *activeMap,
                                   const MapPortal *portal);
void MapSceneManager_MarkFadingOut(MapSceneManager *manager);
bool MapSceneManager_PreparePending(MapSceneManager *manager, const GameMap *activeMap);
bool MapSceneManager_CommitPending(MapSceneManager *manager, GameMap *activeMap);
void MapSceneManager_FinishTransition(MapSceneManager *manager);
void MapSceneManager_SetError(MapSceneManager *manager, const char *message);
bool MapSceneManager_IsBusy(const MapSceneManager *manager);
const char *MapSceneManager_GetError(const MapSceneManager *manager);
const char *MapTransitionPhase_ToString(MapTransitionPhase phase);

SCL_EXTERN_C_END

#endif
