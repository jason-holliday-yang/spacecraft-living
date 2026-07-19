#include "map_layout_internal.h"
#include "map_loader_internal.h"
#include "map_anchors_internal.h"
#include "map_regions_internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            if (Map_IsWithinMapBounds(map, column, row)) {
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
            if (Map_IsWithinMapBounds(map, column, row)) {
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
    if (!Map_IsWithinMapBounds(map, x, y) || map->groundTiles[y][x] == TILE_VOID || IsExteriorEntranceGap(x, y)) {
        return;
    }

    if (map->propTiles[y][x] == TILE_VOID) {
        map->propTiles[y][x] = tile;
    }
}

void MapInternal_SetGroundTile(GameMap *map, int x, int y, TileType tile) {
    if (Map_IsWithinMapBounds(map, x, y)) {
        map->groundTiles[y][x] = tile;
    }
}

void MapInternal_SetPropTile(GameMap *map, int x, int y, TileType tile) {
    if (Map_IsWithinMapBounds(map, x, y)) {
        map->propTiles[y][x] = tile;
    }
}

void MapInternal_SetNaturalPropTile(GameMap *map, int x, int y, TileType tile) {
    if (!Map_IsWithinMapBounds(map, x, y)) {
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

#define DEFINE_LAYER_ALLOCATOR(functionName, elementType) \
    static elementType **functionName(int width, int height) { \
        elementType **rows; \
        elementType *data; \
        int row; \
        if (width <= 0 || height <= 0 || (size_t)width > SIZE_MAX / (size_t)height) { \
            return NULL; \
        } \
        rows = (elementType **)calloc((size_t)height, sizeof(*rows)); \
        data = (elementType *)calloc((size_t)width * (size_t)height, sizeof(*data)); \
        if (rows == NULL || data == NULL) { \
            free(rows); \
            free(data); \
            return NULL; \
        } \
        for (row = 0; row < height; row++) { \
            rows[row] = data + (size_t)row * (size_t)width; \
        } \
        return rows; \
    }

DEFINE_LAYER_ALLOCATOR(AllocateTileLayer, TileType)
DEFINE_LAYER_ALLOCATOR(AllocateAreaLayer, MapArea)
DEFINE_LAYER_ALLOCATOR(AllocateHazardLayer, HazardType)
DEFINE_LAYER_ALLOCATOR(AllocateDecorLayer, MapDecorType)

static void ResetMapMetadata(GameMap *map);

static void FreeLayer(void **rows) {
    if (rows == NULL) {
        return;
    }
    free(rows[0]);
    free(rows);
}

void Map_Destroy(GameMap *map) {
    if (map == NULL) {
        return;
    }
    FreeLayer((void **)map->groundTiles);
    FreeLayer((void **)map->propTiles);
    FreeLayer((void **)map->areaTiles);
    FreeLayer((void **)map->hazardTiles);
    FreeLayer((void **)map->decorTiles);
    map->groundTiles = NULL;
    map->propTiles = NULL;
    map->areaTiles = NULL;
    map->hazardTiles = NULL;
    map->decorTiles = NULL;
    map->width = 0;
    map->height = 0;
    map->tileWidth = 0;
    map->tileHeight = 0;
    ResetMapMetadata(map);
}

bool Map_Resize(GameMap *map, int width, int height, int tileWidth, int tileHeight) {
    TileType **groundTiles;
    TileType **propTiles;
    MapArea **areaTiles;
    HazardType **hazardTiles;
    MapDecorType **decorTiles;
    int row;
    int column;

    if (map == NULL || width <= 0 || height <= 0 || tileWidth <= 0 || tileHeight <= 0) {
        return false;
    }
    groundTiles = AllocateTileLayer(width, height);
    propTiles = AllocateTileLayer(width, height);
    areaTiles = AllocateAreaLayer(width, height);
    hazardTiles = AllocateHazardLayer(width, height);
    decorTiles = AllocateDecorLayer(width, height);
    if (groundTiles == NULL || propTiles == NULL || areaTiles == NULL || hazardTiles == NULL || decorTiles == NULL) {
        FreeLayer((void **)groundTiles);
        FreeLayer((void **)propTiles);
        FreeLayer((void **)areaTiles);
        FreeLayer((void **)hazardTiles);
        FreeLayer((void **)decorTiles);
        return false;
    }

    Map_Destroy(map);
    map->groundTiles = groundTiles;
    map->propTiles = propTiles;
    map->areaTiles = areaTiles;
    map->hazardTiles = hazardTiles;
    map->decorTiles = decorTiles;
    map->width = width;
    map->height = height;
    map->tileWidth = tileWidth;
    map->tileHeight = tileHeight;
    for (row = 0; row < height; row++) {
        for (column = 0; column < width; column++) {
            map->areaTiles[row][column] = MAP_AREA_UNKNOWN;
        }
    }
    return true;
}

bool Map_Clone(GameMap *destination, const GameMap *source) {
    TileType **groundTiles;
    TileType **propTiles;
    MapArea **areaTiles;
    HazardType **hazardTiles;
    MapDecorType **decorTiles;
    int row;

    if (destination == NULL || source == NULL || source->width <= 0 || source->height <= 0) {
        return false;
    }
    if (!Map_Resize(destination, source->width, source->height, source->tileWidth, source->tileHeight)) {
        return false;
    }

    groundTiles = destination->groundTiles;
    propTiles = destination->propTiles;
    areaTiles = destination->areaTiles;
    hazardTiles = destination->hazardTiles;
    decorTiles = destination->decorTiles;
    *destination = *source;
    destination->groundTiles = groundTiles;
    destination->propTiles = propTiles;
    destination->areaTiles = areaTiles;
    destination->hazardTiles = hazardTiles;
    destination->decorTiles = decorTiles;

    for (row = 0; row < source->height; row++) {
        memcpy(destination->groundTiles[row], source->groundTiles[row], (size_t)source->width * sizeof(**source->groundTiles));
        memcpy(destination->propTiles[row], source->propTiles[row], (size_t)source->width * sizeof(**source->propTiles));
        memcpy(destination->areaTiles[row], source->areaTiles[row], (size_t)source->width * sizeof(**source->areaTiles));
        memcpy(destination->hazardTiles[row], source->hazardTiles[row], (size_t)source->width * sizeof(**source->hazardTiles));
        memcpy(destination->decorTiles[row], source->decorTiles[row], (size_t)source->width * sizeof(**source->decorTiles));
    }
    return true;
}

static void ResetMapMetadata(GameMap *map) {
    int anchor;

    map->mapId[0] = '\0';
    map->sourcePath[0] = '\0';
    map->mapKind = MAP_KIND_UNKNOWN;
    map->loadedFromTiled = false;
    map->loadFailed = false;
    map->loadError[0] = '\0';
    map->hasAreaLayer = false;
    map->hasHazardLayer = false;
    map->hasDecorLayer = false;
    map->hasAnchorLayer = false;
    map->hasResourceLayer = false;
    map->hasMonsterLayer = false;
    map->hasLogLayer = false;
    map->hasRegionLayer = false;
    map->hasUnlockLayer = false;
    map->hasPortalLayer = false;
    map->hasEntityLayer = false;
    map->resourceSeedCount = 0;
    map->monsterSeedCount = 0;
    map->logSeedCount = 0;
    map->regionCount = 0;
    map->unlockCount = 0;
    map->portalCount = 0;
    map->entityCount = 0;
    for (anchor = 0; anchor < MAP_ANCHOR_COUNT; anchor++) {
        map->anchorSet[anchor] = false;
        map->anchorX[anchor] = -1;
        map->anchorY[anchor] = -1;
    }
    map->campPlaced = false;
    map->campX = -1;
    map->campY = -1;
}

bool Map_LoadTiledMap(GameMap *map,
                      const char *mapId,
                      const char *sourcePath,
                      MapKind mapKind) {
    if (map == NULL
        || mapId == NULL
        || mapId[0] == '\0'
        || sourcePath == NULL
        || sourcePath[0] == '\0') {
        return false;
    }

    Map_Destroy(map);
    ResetMapMetadata(map);
    if (!MapInternal_LoadTiledMap(map, sourcePath)) {
        Map_Destroy(map);
        ResetMapMetadata(map);
        snprintf(map->mapId, sizeof(map->mapId), "%s", mapId);
        snprintf(map->sourcePath, sizeof(map->sourcePath), "%s", sourcePath);
        map->mapKind = mapKind;
        map->loadFailed = true;
        snprintf(map->loadError, sizeof(map->loadError),
                 "Unable to load map ID '%s' from '%s'", mapId, sourcePath);
        return false;
    }
    snprintf(map->mapId, sizeof(map->mapId), "%s", mapId);
    snprintf(map->sourcePath, sizeof(map->sourcePath), "%s", sourcePath);
    map->mapKind = mapKind;
    return true;
}

bool Map_LoadById(GameMap *map, const MapCatalog *catalog, const char *mapId) {
    const MapCatalogEntry *entry;

    if (map == NULL || catalog == NULL || mapId == NULL || mapId[0] == '\0') {
        return false;
    }
    entry = MapCatalog_Find(catalog, mapId);
    if (entry == NULL) {
        Map_Destroy(map);
        ResetMapMetadata(map);
        snprintf(map->mapId, sizeof(map->mapId), "%s", mapId);
        map->loadFailed = true;
        snprintf(map->loadError, sizeof(map->loadError),
                 "Unable to load map ID '%s': file path is not registered in catalog '%s'",
                 mapId,
                 catalog->sourcePath[0] != '\0' ? catalog->sourcePath : "<unknown>");
        return false;
    }
    return Map_LoadTiledMap(map, entry->id, entry->file, entry->kind);
}

bool Map_LoadTiled(GameMap *map, const char *relativePath) {
    bool loaded;

    loaded = Map_LoadTiledMap(map, "legacy_direct", relativePath, MAP_KIND_LEGACY);
    if (loaded) {
        Map_ActivateRuntime(map);
    }
    return loaded;
}

bool Map_IsSceneReady(const GameMap *map) {
    return map != NULL
        && map->loadedFromTiled
        && !map->loadFailed
        && map->mapId[0] != '\0'
        && map->width > 0
        && map->height > 0
        && map->groundTiles != NULL
        && map->propTiles != NULL
        && map->hasAreaLayer
        && map->hasHazardLayer
        && map->hasDecorLayer
        && map->hasAnchorLayer
        && map->hasResourceLayer
        && map->hasMonsterLayer
        && map->hasLogLayer
        && map->hasRegionLayer
        && map->hasUnlockLayer
        && map->hasPortalLayer
        && map->hasEntityLayer;
}


bool Map_IsProductionReady(const GameMap *map) {
    int anchor;

    if (map == NULL
        || !map->loadedFromTiled
        || map->width <= 0
        || map->height <= 0
        || map->groundTiles == NULL
        || map->propTiles == NULL
        || !map->hasAreaLayer
        || !map->hasHazardLayer
        || !map->hasDecorLayer
        || !map->hasAnchorLayer
        || !map->hasResourceLayer
        || !map->hasMonsterLayer
        || !map->hasLogLayer
        || !map->hasRegionLayer
        || !map->hasUnlockLayer
        || map->resourceSeedCount <= 0
        || map->monsterSeedCount <= 0
        || map->logSeedCount <= 0
        || map->regionCount <= 0
        || map->unlockCount <= 0) {
        return false;
    }
    for (anchor = 0; anchor <= MAP_ANCHOR_FINAL_GATE; anchor++) {
        if (!map->anchorSet[anchor]) {
            return false;
        }
    }
    return true;
}

const char *Map_GetLoadError(const GameMap *map) {
    if (map == NULL || !map->loadFailed || map->loadError[0] == '\0') {
        return "";
    }
    return map->loadError;
}

bool MapInternal_InitLegacyFixture(GameMap *map) {
    if (map == NULL || !Map_Resize(map, MAP_WIDTH, MAP_HEIGHT, TILE_SIZE, TILE_SIZE)) {
        return false;
    }
    ResetMapMetadata(map);
    MapInternal_SeedWorldLayout(map);
    MapInternal_SeedShipLayout(map);
    MapInternal_RebuildDerivedLayers(map);
    MapInternal_RebuildLegacyAnchors(map);
    Map_ActivateRuntime(map);
    return true;
}

bool Map_Init(GameMap *map) {
    MapCatalog catalog;
    char loadError[MAP_LOAD_ERROR_MAX];

    if (map == NULL) {
        return false;
    }

    if (MapCatalog_Load(&catalog, "maps/map_catalog.json")
        && Map_LoadById(map, &catalog, catalog.defaultMapId)
        && Map_IsProductionReady(map)) {
        Map_ActivateRuntime(map);
        return true;
    }
    if (!catalog.loaded && catalog.loadError[0] != '\0') {
        snprintf(loadError, sizeof(loadError), "%s", catalog.loadError);
    } else if (map->loadError[0] != '\0') {
        snprintf(loadError, sizeof(loadError), "%s", map->loadError);
    } else {
        snprintf(loadError, sizeof(loadError),
                 "Map ID '%s' from catalog '%s' is missing one or more required production layers",
                 catalog.defaultMapId,
                 catalog.sourcePath);
    }

    Map_Destroy(map);
    ResetMapMetadata(map);
    if (!Map_Resize(map, 1, 1, TILE_SIZE, TILE_SIZE)) {
        map->loadFailed = true;
        snprintf(map->loadError, sizeof(map->loadError), "%s", loadError);
        fprintf(stderr, "Map initialization failed: %s\n", map->loadError);
        return false;
    }
    ResetMapMetadata(map);
    map->loadFailed = true;
    snprintf(map->loadError, sizeof(map->loadError), "%s", loadError);
    Map_ActivateRuntime(map);
    fprintf(stderr, "Map initialization failed: %s\n", map->loadError);
    return false;
}
