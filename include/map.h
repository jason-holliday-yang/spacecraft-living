#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>
#include "c_compat.h"
#include "config.h"

#define MAP_REGION_NAME_MAX 64
#define MAX_MAP_REGIONS 48
#define MAP_UNLOCK_ID_MAX 64
#define MAP_UNLOCK_TYPE_MAX 32
#define MAX_MAP_UNLOCKS 32
#define MAP_LOAD_ERROR_MAX 192
#define MAP_ID_MAX 64
#define MAP_SOURCE_PATH_MAX 256
#define MAP_ANCHOR_ID_MAX 64
#define MAP_TRANSITION_ID_MAX 64
#define MAP_PROMPT_KEY_MAX 64
#define MAP_FLAG_ID_MAX 64
#define MAP_ENTITY_ID_MAX 64
#define MAP_ENTITY_TYPE_MAX 64
#define MAP_TEXTURE_KEY_MAX 96
#define MAP_DRAW_LAYER_MAX 32
#define MAX_MAP_PORTALS 32
#define MAX_MAP_ENTITIES 32
#define MAX_MAP_CATALOG_ENTRIES 16
#include "raylib.h"

/* Public world-map data, tile queries, area identity, and traversal helpers.
 * Drawing APIs live in map_render.h.
 */

SCL_EXTERN_C_BEGIN

typedef enum MapArea {
    MAP_AREA_BASE = 0,
    MAP_AREA_FOREST,
    MAP_AREA_SWAMP_OUTER,
    MAP_AREA_SWAMP_DEEP,
    MAP_AREA_RUINS,
    MAP_AREA_BOSS_ARENA,
    MAP_AREA_UNKNOWN
} MapArea;

typedef enum TileType {
    TILE_VOID = 0,
    TILE_BASE_FLOOR,
    TILE_FOREST_GROUND,
    TILE_SWAMP_GROUND,
    TILE_DEEP_SWAMP_GROUND,
    TILE_RUINS_GROUND,
    TILE_TREE,
    TILE_ROCK,
    TILE_TECH_TABLE,
    TILE_STORAGE_LOCKER,
    TILE_BUNK,
    TILE_OXYGEN_CONSOLE,
    TILE_LOXI_TERMINAL,
    TILE_WORKBENCH,
    TILE_AIRLOCK_CONSOLE,
    TILE_AIRLOCK_DOOR,
    TILE_LOXI_ROOM_DOOR,
    TILE_COMM_RELAY,
    TILE_CRASH_CLUE,
    TILE_ENERGY_CONSOLE,
    TILE_SIGNAL_TOWER,
    TILE_MONOLITH,
    TILE_BARRIER_SWAMP,
    TILE_BARRIER_DEEP,
    TILE_BARRIER_RUINS,
    TILE_LOG_SITE,
    TILE_ENTITY_BLOCKER
} TileType;

typedef enum HazardType {
    HAZARD_NONE = 0,
    HAZARD_TRIP,
    HAZARD_SWAMP,
    HAZARD_POISON
} HazardType;

typedef enum MapDecorType {
    MAP_DECOR_NONE = 0,
    MAP_DECOR_DEBRIS,
    MAP_DECOR_GRASS,
    MAP_DECOR_REEDS,
    MAP_DECOR_RUINS_CRACK,
    MAP_DECOR_WARNING_LIGHT
} MapDecorType;

typedef enum MapKind {
    MAP_KIND_UNKNOWN = 0,
    MAP_KIND_INTERIOR,
    MAP_KIND_EXTERIOR,
    MAP_KIND_LEGACY,
    MAP_KIND_TEST
} MapKind;

typedef enum MapAnchor {
    MAP_ANCHOR_PLAYER_START = 0,
    MAP_ANCHOR_PLAYER_RESPAWN,
    MAP_ANCHOR_AIRLOCK_DOOR,
    MAP_ANCHOR_AIRLOCK_EXIT,
    MAP_ANCHOR_LOXI_TERMINAL,
    MAP_ANCHOR_OXYGEN_CONSOLE,
    MAP_ANCHOR_WORKBENCH,
    MAP_ANCHOR_AIRLOCK_CONSOLE,
    MAP_ANCHOR_ENERGY_CONSOLE,
    MAP_ANCHOR_COMM_RELAY,
    MAP_ANCHOR_CRASH_CLUE,
    MAP_ANCHOR_ENERGY_CORE,
    MAP_ANCHOR_SIGNAL_TOWER,
    MAP_ANCHOR_BOSS_PLAYER_ENTRY,
    MAP_ANCHOR_BOSS_SPAWN,
    MAP_ANCHOR_MONOLITH_A,
    MAP_ANCHOR_MONOLITH_B,
    MAP_ANCHOR_MONOLITH_C,
    MAP_ANCHOR_SWAMP_OUTER_GATE,
    MAP_ANCHOR_SWAMP_DEEP_GATE,
    MAP_ANCHOR_RUINS_GATE,
    MAP_ANCHOR_RUINS_APPROACH,
    MAP_ANCHOR_ROPE_BARRIER_A,
    MAP_ANCHOR_ROPE_BARRIER_B,
    MAP_ANCHOR_ROPE_BARRIER_C,
    MAP_ANCHOR_WEST_ECHO_BASIN,
    MAP_ANCHOR_SOUTH_FACILITY,
    MAP_ANCHOR_FINAL_GATE,
    MAP_ANCHOR_SHIP_INTERIOR_RETURN,
    MAP_ANCHOR_SHIP_EXIT_SPAWN,
    MAP_ANCHOR_SHIP_EXTERIOR_ORIGIN,
    MAP_ANCHOR_COUNT
} MapAnchor;

typedef struct MapResourceSeed {
    int resourceType;
    int gridX;
    int gridY;
    int baseYield;
    int respawnsRemaining;
    bool special;
} MapResourceSeed;

typedef struct MapMonsterSeed {
    int monsterType;
    int gridX;
    int gridY;
    int unlockStage;
} MapMonsterSeed;

typedef struct MapLogSeed {
    int sourceIndex;
    int logCategory;
    int gridX;
    int gridY;
} MapLogSeed;

typedef struct MapRegion {
    char name[MAP_REGION_NAME_MAX];
    int gridX;
    int gridY;
    int width;
    int height;
    int priority;
    MapArea area;
} MapRegion;

typedef struct MapUnlock {
    char id[MAP_UNLOCK_ID_MAX];
    char type[MAP_UNLOCK_TYPE_MAX];
    int gridX;
    int gridY;
    int width;
    int height;
    TileType clearsTile;
} MapUnlock;

typedef struct MapPortal {
    char portalId[MAP_ID_MAX];
    char targetMapId[MAP_ID_MAX];
    char targetAnchor[MAP_ANCHOR_ID_MAX];
    char transitionId[MAP_TRANSITION_ID_MAX];
    char promptKey[MAP_PROMPT_KEY_MAX];
    char requiresFlag[MAP_FLAG_ID_MAX];
    int gridX;
    int gridY;
    int width;
    int height;
    int interactionRadius;
} MapPortal;

typedef struct MapEntity {
    char entityId[MAP_ENTITY_ID_MAX];
    char entityType[MAP_ENTITY_TYPE_MAX];
    char textureKey[MAP_TEXTURE_KEY_MAX];
    char originAnchor[MAP_ANCHOR_ID_MAX];
    char portalId[MAP_ID_MAX];
    char drawLayer[MAP_DRAW_LAYER_MAX];
    int gridX;
    int gridY;
    int width;
    int height;
    bool blocksMovement;
} MapEntity;

typedef struct MapCatalogEntry {
    char id[MAP_ID_MAX];
    char file[MAP_SOURCE_PATH_MAX];
    MapKind kind;
    char defaultSpawn[MAP_ANCHOR_ID_MAX];
    char respawnAnchor[MAP_ANCHOR_ID_MAX];
    bool minimapEnabled;
} MapCatalogEntry;

typedef struct MapCatalog {
    char sourcePath[MAP_SOURCE_PATH_MAX];
    char defaultMapId[MAP_ID_MAX];
    MapCatalogEntry entries[MAX_MAP_CATALOG_ENTRIES];
    int entryCount;
    bool loaded;
    char loadError[MAP_LOAD_ERROR_MAX];
} MapCatalog;

typedef struct GameMap {
    char mapId[MAP_ID_MAX];
    char sourcePath[MAP_SOURCE_PATH_MAX];
    MapKind mapKind;
    int contentVersion;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    bool loadedFromTiled;
    bool loadFailed;
    char loadError[MAP_LOAD_ERROR_MAX];
    TileType **groundTiles;
    TileType **propTiles;
    MapArea **areaTiles;
    HazardType **hazardTiles;
    MapDecorType **decorTiles;
    bool hasAreaLayer;
    bool hasHazardLayer;
    bool hasDecorLayer;
    bool hasAnchorLayer;
    bool hasResourceLayer;
    bool hasMonsterLayer;
    bool hasLogLayer;
    bool hasRegionLayer;
    bool hasUnlockLayer;
    bool hasPortalLayer;
    bool hasEntityLayer;
    bool anchorSet[MAP_ANCHOR_COUNT];
    int anchorX[MAP_ANCHOR_COUNT];
    int anchorY[MAP_ANCHOR_COUNT];
    MapResourceSeed resourceSeeds[MAX_RESOURCE_NODES];
    int resourceSeedCount;
    MapMonsterSeed monsterSeeds[MAX_MONSTERS];
    int monsterSeedCount;
    MapLogSeed logSeeds[MAX_LOGS];
    int logSeedCount;
    MapRegion regions[MAX_MAP_REGIONS];
    int regionCount;
    MapUnlock unlocks[MAX_MAP_UNLOCKS];
    int unlockCount;
    MapPortal portals[MAX_MAP_PORTALS];
    int portalCount;
    MapEntity entities[MAX_MAP_ENTITIES];
    int entityCount;
    bool campPlaced;
    int campX;
    int campY;
} GameMap;

bool Map_Init(GameMap *map);
bool Map_LoadTiled(GameMap *map, const char *relativePath);
bool Map_LoadTiledMap(GameMap *map,
                      const char *mapId,
                      const char *sourcePath,
                      MapKind mapKind);
bool Map_LoadById(GameMap *map, const MapCatalog *catalog, const char *mapId);
bool MapCatalog_Load(MapCatalog *catalog, const char *relativePath);
const MapCatalogEntry *MapCatalog_Find(const MapCatalog *catalog, const char *mapId);
const char *MapCatalog_GetLoadError(const MapCatalog *catalog);
MapKind MapKind_FromString(const char *name);
const char *MapKind_ToString(MapKind kind);
bool Map_IsProductionReady(const GameMap *map);
bool Map_IsSceneReady(const GameMap *map);
void Map_ActivateRuntime(const GameMap *map);
const char *Map_GetLoadError(const GameMap *map);
void Map_Destroy(GameMap *map);
bool Map_Resize(GameMap *map, int width, int height, int tileWidth, int tileHeight);
bool Map_Clone(GameMap *destination, const GameMap *source);

bool Map_IsWithinBounds(int gridX, int gridY);
bool Map_IsWithinMapBounds(const GameMap *map, int gridX, int gridY);
bool Map_IsWalkable(const GameMap *map, int gridX, int gridY);
bool Map_IsOpaque(const GameMap *map, int gridX, int gridY);
TileType Map_GetTileAt(const GameMap *map, int gridX, int gridY);
TileType Map_GetGroundTileAt(const GameMap *map, int gridX, int gridY);
TileType Map_GetPropTileAt(const GameMap *map, int gridX, int gridY);
HazardType Map_GetHazardAt(const GameMap *map, int gridX, int gridY);
MapDecorType Map_GetDecorAt(const GameMap *map, int gridX, int gridY);
bool Map_GetMultiTilePropBounds(const GameMap *map,
                                TileType tile,
                                int gridX,
                                int gridY,
                                int *originX,
                                int *originY,
                                int *width,
                                int *height);
MapArea Map_GetAreaAt(int gridX, int gridY);
MapArea Map_GetAreaAtMap(const GameMap *map, int gridX, int gridY);
MapArea Map_GetAreaTileAt(const GameMap *map, int gridX, int gridY);
Vector2 Map_GridToWorld(int gridX, int gridY);
const char *Map_GetAreaName(MapArea area);
const char *Map_GetLocationNameAt(int gridX, int gridY);
const char *Map_GetLocationNameAtMap(const GameMap *map, int gridX, int gridY);
const char *Map_GetRoomNameAt(int gridX, int gridY);
const char *Map_GetRoomNameAtMap(const GameMap *map, int gridX, int gridY);
const MapRegion *Map_GetRegionByName(const GameMap *map, const char *regionName);
const MapRegion *Map_GetRegionAt(const GameMap *map, int gridX, int gridY);
const MapRegion *Map_GetRuntimeRegionByName(const char *regionName);
const char *Map_GetAnchorName(MapAnchor anchor);
bool Map_GetAnchorPosition(const GameMap *map, MapAnchor anchor, int *gridX, int *gridY);
bool Map_GetAnchorPositionByName(const GameMap *map, const char *anchorName, int *gridX, int *gridY);
bool Map_GetRuntimeAnchorPosition(MapAnchor anchor, int *gridX, int *gridY);
const MapPortal *Map_GetPortalById(const GameMap *map, const char *portalId);
const MapPortal *Map_FindPortalNear(const GameMap *map, int gridX, int gridY);
const MapEntity *Map_GetEntityById(const GameMap *map, const char *entityId);
const MapUnlock *Map_GetUnlockById(const GameMap *map, const char *unlockId);
const MapUnlock *Map_GetUnlockAt(const GameMap *map, int gridX, int gridY);
bool Map_SetUnlockOpen(GameMap *map, const char *unlockId, bool open);
bool Map_IsUnlockOpen(const GameMap *map, const char *unlockId);

void Map_UnlockSwampOuter(GameMap *map);
void Map_LockSwampOuter(GameMap *map);
bool Map_IsSwampOuterUnlocked(const GameMap *map);
void Map_UnlockLoxiRoom(GameMap *map);
void Map_LockLoxiRoom(GameMap *map);
bool Map_IsLoxiRoomUnlocked(const GameMap *map);
void Map_UnlockSwampDeep(GameMap *map);
void Map_UnlockRuins(GameMap *map);
void Map_SetFieldCamp(GameMap *map, int gridX, int gridY);

bool Map_CanCrossWithRope(const GameMap *map, int fromX, int fromY, int toX, int toY);
void Map_CreateRopeBridge(GameMap *map, int gridX, int gridY);

SCL_EXTERN_C_END

#endif
