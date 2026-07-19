#include "map_regions_internal.h"
#include "map_internal.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static MapArea *gRuntimeAreaTiles = NULL;
static int gRuntimeAreaWidth = 0;
static bool gRuntimeAreaLayerActive = false;
static MapRegion gRuntimeRegions[MAX_MAP_REGIONS];
static int gRuntimeRegionCount = 0;
static bool gRuntimeRegionLayerActive = false;

/* Coordinate-based rules below are intentionally limited to compatibility
 * for maps that predate Area/Regions layers. Production Tiled maps never use
 * these paths when their data layers are present.
 */
static bool IsSouthFacilityBounds(int gridX, int gridY) {
    return gridX >= EXTERIOR_X(62)
        && gridX <= WORLD_MAX_X
        && gridY >= EXTERIOR_Y(86)
        && gridY <= WORLD_MAX_Y;
}

HazardType MapInternal_GetFallbackHazardAt(const GameMap *map, int gridX, int gridY) {
    switch (Map_GetGroundTileAt(map, gridX, gridY)) {
        case TILE_FOREST_GROUND:
            if ((gridX + gridY) % 11 == 0) {
                return HAZARD_TRIP;
            }
            return HAZARD_NONE;
        case TILE_SWAMP_GROUND:
            return HAZARD_SWAMP;
        case TILE_DEEP_SWAMP_GROUND:
            return HAZARD_POISON;
        default:
            return HAZARD_NONE;
    }
}

MapArea MapInternal_GetFallbackAreaAt(int gridX, int gridY) {
    if (MapInternal_IsRectBounds(gridX, gridY, BOSS_ARENA_X, BOSS_ARENA_Y, BOSS_ARENA_WIDTH, BOSS_ARENA_HEIGHT)) {
        return MAP_AREA_BOSS_ARENA;
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_CARGO_HOLD_X, SHIP_CREW_QUARTERS_Y, AIRLOCK_DOOR_X - SHIP_CARGO_HOLD_X + 1, SHIP_POWER_BAY_Y + SHIP_POWER_BAY_HEIGHT - SHIP_CREW_QUARTERS_Y)) {
        return MAP_AREA_BASE;
    }
    if (MapInternal_IsRectBounds(gridX, gridY, RUINS_MAIN_X, RUINS_MAIN_Y, RUINS_MAIN_WIDTH, RUINS_MAIN_HEIGHT)
        || MapInternal_IsRectBounds(gridX, gridY, RUINS_APPROACH_X, RUINS_APPROACH_Y, RUINS_APPROACH_WIDTH, RUINS_APPROACH_HEIGHT)) {
        return MAP_AREA_RUINS;
    }
    if (IsSouthFacilityBounds(gridX, gridY)) {
        return MAP_AREA_RUINS;
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SWAMP_DEEP_X, SWAMP_DEEP_Y, SWAMP_DEEP_WIDTH, SWAMP_DEEP_HEIGHT)) {
        return MAP_AREA_SWAMP_DEEP;
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SWAMP_OUTER_X, SWAMP_OUTER_Y, SWAMP_OUTER_WIDTH, SWAMP_OUTER_HEIGHT)) {
        return MAP_AREA_SWAMP_OUTER;
    }
    if (gridX >= WORLD_MIN_X && gridX <= WORLD_MAX_X && gridY >= WORLD_MIN_Y && gridY <= WORLD_MAX_Y) {
        return MAP_AREA_FOREST;
    }
    return MAP_AREA_UNKNOWN;
}

MapArea Map_GetAreaAt(int gridX, int gridY) {
    if (gRuntimeAreaLayerActive && Map_IsWithinBounds(gridX, gridY)) {
        return gRuntimeAreaTiles[(size_t)gridY * (size_t)gRuntimeAreaWidth + (size_t)gridX];
    }
    return MapInternal_GetFallbackAreaAt(gridX, gridY);
}

MapArea Map_GetAreaAtMap(const GameMap *map, int gridX, int gridY) {
    if (map != NULL && map->hasAreaLayer && Map_IsWithinMapBounds(map, gridX, gridY)) {
        return map->areaTiles[gridY][gridX];
    }
    return MapInternal_GetFallbackAreaAt(gridX, gridY);
}

MapArea Map_GetAreaTileAt(const GameMap *map, int gridX, int gridY) {
    return Map_GetAreaAtMap(map, gridX, gridY);
}

void MapInternal_ActivateRuntimeAreaLayer(const GameMap *map) {
    MapArea *areas;
    int row;

    free(gRuntimeAreaTiles);
    gRuntimeAreaTiles = NULL;
    gRuntimeAreaWidth = 0;
    gRuntimeAreaLayerActive = false;
    if (map == NULL || map->width <= 0 || map->height <= 0 || !map->hasAreaLayer) {
        return;
    }

    areas = (MapArea *)malloc((size_t)map->width * (size_t)map->height * sizeof(*areas));
    if (areas == NULL) {
        return;
    }
    for (row = 0; row < map->height; row++) {
        memcpy(areas + (size_t)row * (size_t)map->width,
               map->areaTiles[row],
               (size_t)map->width * sizeof(*areas));
    }
    gRuntimeAreaTiles = areas;
    gRuntimeAreaWidth = map->width;
    gRuntimeAreaLayerActive = true;
}

void MapInternal_RebuildDerivedLayers(GameMap *map) {
    int row;
    int column;

    if (map == NULL) {
        return;
    }

    for (row = 0; row < map->height; row++) {
        for (column = 0; column < map->width; column++) {
            map->areaTiles[row][column] = MapInternal_GetFallbackAreaAt(column, row);
            map->hazardTiles[row][column] = MapInternal_GetFallbackHazardAt(map, column, row);
        }
    }

    map->hasAreaLayer = true;
    map->hasHazardLayer = true;
}

void MapInternal_ActivateRuntimeRegionLayer(const GameMap *map) {
    int index;

    gRuntimeRegionCount = 0;
    gRuntimeRegionLayerActive = false;
    if (map == NULL || !map->hasRegionLayer) {
        return;
    }

    for (index = 0; index < map->regionCount && index < MAX_MAP_REGIONS; index++) {
        gRuntimeRegions[index] = map->regions[index];
        gRuntimeRegionCount++;
    }
    gRuntimeRegionLayerActive = gRuntimeRegionCount > 0;
}

const MapRegion *Map_GetRegionByName(const GameMap *map, const char *regionName) {
    int index;

    if (map == NULL || regionName == NULL || regionName[0] == '\0' || !map->hasRegionLayer) {
        return NULL;
    }
    for (index = 0; index < map->regionCount; index++) {
        if (strcmp(map->regions[index].name, regionName) == 0) {
            return &map->regions[index];
        }
    }
    return NULL;
}

const MapRegion *Map_GetRegionAt(const GameMap *map, int gridX, int gridY) {
    const MapRegion *bestRegion;
    int bestPriority;
    int index;

    if (map == NULL || !map->hasRegionLayer || !Map_IsWithinMapBounds(map, gridX, gridY)) {
        return NULL;
    }
    bestRegion = NULL;
    bestPriority = -2147483647;
    for (index = 0; index < map->regionCount; index++) {
        const MapRegion *region;

        region = &map->regions[index];
        if (region->width > 0
            && region->height > 0
            && region->name[0] != '\0'
            && MapInternal_IsRectBounds(gridX, gridY, region->gridX, region->gridY, region->width, region->height)
            && (bestRegion == NULL || region->priority >= bestPriority)) {
            bestRegion = region;
            bestPriority = region->priority;
        }
    }
    return bestRegion;
}

const MapRegion *Map_GetRuntimeRegionByName(const char *regionName) {
    int index;

    if (!gRuntimeRegionLayerActive || regionName == NULL || regionName[0] == '\0') {
        return NULL;
    }
    for (index = 0; index < gRuntimeRegionCount; index++) {
        if (strcmp(gRuntimeRegions[index].name, regionName) == 0) {
            return &gRuntimeRegions[index];
        }
    }
    return NULL;
}

static const char *MapInternal_GetRuntimeRegionNameAt(int gridX, int gridY) {
    int index;
    int bestPriority;
    const char *bestName;

    if (!gRuntimeRegionLayerActive || !Map_IsWithinBounds(gridX, gridY)) {
        return NULL;
    }

    bestPriority = -2147483647;
    bestName = NULL;
    for (index = 0; index < gRuntimeRegionCount; index++) {
        const MapRegion *region;

        region = &gRuntimeRegions[index];
        if (region->width <= 0 || region->height <= 0 || region->name[0] == '\0') {
            continue;
        }
        if (MapInternal_IsRectBounds(gridX, gridY, region->gridX, region->gridY, region->width, region->height)
            && (bestName == NULL || region->priority >= bestPriority)) {
            bestPriority = region->priority;
            bestName = region->name;
        }
    }
    return bestName;
}

const char *Map_GetAreaName(MapArea area) {
    switch (area) {
        case MAP_AREA_BASE:
            return "Ship Base";
        case MAP_AREA_FOREST:
            return "Crash Forest";
        case MAP_AREA_SWAMP_OUTER:
            return "Spore Swamp";
        case MAP_AREA_SWAMP_DEEP:
            return "Spore Swamp";
        case MAP_AREA_RUINS:
            return "Ruins";
        case MAP_AREA_BOSS_ARENA:
            return "Ruins";
        case MAP_AREA_UNKNOWN:
        default:
            return "Unknown Area";
    }
}

const char *Map_GetLocationNameAtMap(const GameMap *map, int gridX, int gridY) {
    const MapRegion *region;

    region = Map_GetRegionAt(map, gridX, gridY);
    if (region != NULL) {
        return region->name;
    }
    if (map != NULL && map->hasRegionLayer) {
        return Map_GetAreaName(Map_GetAreaAtMap(map, gridX, gridY));
    }

    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_CORRIDOR_X, SHIP_CORRIDOR_Y, SHIP_CORRIDOR_WIDTH, SHIP_CORRIDOR_HEIGHT)) {
        return "Central Corridor";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_CARGO_HOLD_X, SHIP_CARGO_HOLD_Y, SHIP_CARGO_HOLD_WIDTH, SHIP_CARGO_HOLD_HEIGHT)) {
        return "Cargo Hold";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_CREW_QUARTERS_X, SHIP_CREW_QUARTERS_Y, SHIP_CREW_QUARTERS_WIDTH, SHIP_CREW_QUARTERS_HEIGHT)) {
        return "Crew Quarters";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_DIAGNOSTICS_X, SHIP_DIAGNOSTICS_Y, SHIP_DIAGNOSTICS_WIDTH, SHIP_DIAGNOSTICS_HEIGHT)) {
        return "Diagnostics";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_TERMINAL_BAY_X, SHIP_TERMINAL_BAY_Y, SHIP_TERMINAL_BAY_WIDTH, SHIP_TERMINAL_BAY_HEIGHT)) {
        return "Terminal Bay";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_LIFE_SUPPORT_X, SHIP_LIFE_SUPPORT_Y, SHIP_LIFE_SUPPORT_WIDTH, SHIP_LIFE_SUPPORT_HEIGHT)) {
        return "Life Support";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_WORKSHOP_X, SHIP_WORKSHOP_Y, SHIP_WORKSHOP_WIDTH, SHIP_WORKSHOP_HEIGHT)) {
        return "Workshop";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_POWER_BAY_X, SHIP_POWER_BAY_Y, SHIP_POWER_BAY_WIDTH, SHIP_POWER_BAY_HEIGHT)) {
        return "Power Bay";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SHIP_AIRLOCK_LINK_X, SHIP_AIRLOCK_LINK_Y, SHIP_AIRLOCK_LINK_WIDTH, SHIP_AIRLOCK_LINK_HEIGHT)) {
        return "Airlock Link";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, BOSS_ARENA_X, BOSS_ARENA_Y, BOSS_ARENA_WIDTH, BOSS_ARENA_HEIGHT)) {
        return "Northwest Ruins";
    }

    if (MapInternal_IsRectBounds(gridX, gridY, RUINS_MAIN_X, RUINS_MAIN_Y, RUINS_MAIN_WIDTH, RUINS_MAIN_HEIGHT)) {
        if (MapInternal_IsRectBounds(gridX,
                         gridY,
                         EXTERIOR_X(57),
                         EXTERIOR_Y(6),
                         EXTERIOR_X(70) - EXTERIOR_X(57) + 1,
                         (SIGNAL_TOWER_Y + 6) - EXTERIOR_Y(6) + 1)) {
            return "Signal Tower Plateau";
        }
        return "Monolith Ring";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, RUINS_APPROACH_X, RUINS_APPROACH_Y, RUINS_APPROACH_WIDTH, RUINS_APPROACH_HEIGHT)) {
        return "Ruins Approach";
    }

    if (MapInternal_IsRectBounds(gridX, gridY, SWAMP_DEEP_X, SWAMP_DEEP_Y, SWAMP_DEEP_WIDTH, SWAMP_DEEP_HEIGHT)) {
        if (gridY <= EXTERIOR_Y(36)) {
            return "Deep Gate";
        }
        return "Deep Basin";
    }
    if (MapInternal_IsRectBounds(gridX, gridY, SWAMP_OUTER_X, SWAMP_OUTER_Y, SWAMP_OUTER_WIDTH, SWAMP_OUTER_HEIGHT)) {
        if (gridY <= EXTERIOR_Y(46)) {
            return "Outer Swamp Rim";
        }
        return "Flooded Detour";
    }
    if (gridX >= WORLD_MIN_X && gridX <= WORLD_MAX_X && gridY >= WORLD_MIN_Y && gridY <= WORLD_MAX_Y) {
        if (gridX >= EXTERIOR_X(10) && gridX <= EXTERIOR_X(58) && gridY >= EXTERIOR_Y(30) && gridY <= EXTERIOR_Y(102)) {
            if (MapInternal_IsRectBounds(gridX, gridY, EXTERIOR_X(40), EXTERIOR_Y(84), EXTERIOR_SIZE(12), EXTERIOR_SIZE(12))) {
                return "Last Camp";
            }
            if (MapInternal_IsRectBounds(gridX, gridY, EXTERIOR_X(38), EXTERIOR_Y(50), EXTERIOR_SIZE(14), EXTERIOR_SIZE(19))) {
                return "Canopy Hollow";
            }
            if (MapInternal_IsRectBounds(gridX, gridY, EXTERIOR_X(30), EXTERIOR_Y(58), EXTERIOR_SIZE(16), EXTERIOR_SIZE(24))) {
                return "Survey Break";
            }
            if (MapInternal_IsRectBounds(gridX, gridY, EXTERIOR_X(10), EXTERIOR_Y(78), EXTERIOR_SIZE(24), EXTERIOR_SIZE(24))
                || MapInternal_IsRectBounds(gridX, gridY, EXTERIOR_X(30), EXTERIOR_Y(78), EXTERIOR_SIZE(20), EXTERIOR_SIZE(24))
                || MapInternal_IsRectBounds(gridX, gridY, EXTERIOR_X(10), EXTERIOR_Y(94), EXTERIOR_SIZE(40), EXTERIOR_SIZE(10))) {
                return "Echo Basin";
            }
            return "West Frontier";
        }
        if (gridX >= EXTERIOR_X(62) && gridY >= EXTERIOR_Y(86)) {
            if (gridX >= EXTERIOR_X(119) || (gridX >= EXTERIOR_X(116) && gridY >= EXTERIOR_Y(99))) {
                return "Root Vault";
            }
            if (gridX >= EXTERIOR_X(113)
                || (gridX >= EXTERIOR_X(110) && gridY <= EXTERIOR_Y(97))
                || (gridX >= EXTERIOR_X(108) && gridY >= EXTERIOR_Y(98))) {
                return "Purifier Ring";
            }
            if (gridX >= EXTERIOR_X(101) && gridY >= EXTERIOR_Y(93)) {
                return "Service Shafts";
            }
            if (gridX >= EXTERIOR_X(84) && gridY >= EXTERIOR_Y(90)) {
                return "Vent Galleries";
            }
            return "South Collapse";
        }
        return "Crash Forest";
    }

    return Map_GetAreaName(Map_GetAreaAtMap(map, gridX, gridY));
}

const char *Map_GetLocationNameAt(int gridX, int gridY) {
    const char *regionName;

    regionName = MapInternal_GetRuntimeRegionNameAt(gridX, gridY);
    if (regionName != NULL) {
        return regionName;
    }
    return Map_GetLocationNameAtMap(NULL, gridX, gridY);
}

const char *Map_GetRoomNameAtMap(const GameMap *map, int gridX, int gridY) {
    if (Map_GetAreaAtMap(map, gridX, gridY) == MAP_AREA_BASE) {
        return Map_GetLocationNameAtMap(map, gridX, gridY);
    }
    return Map_GetAreaName(Map_GetAreaAtMap(map, gridX, gridY));
}

const char *Map_GetRoomNameAt(int gridX, int gridY) {
    if (Map_GetAreaAt(gridX, gridY) == MAP_AREA_BASE) {
        return Map_GetLocationNameAt(gridX, gridY);
    }
    return Map_GetAreaName(Map_GetAreaAt(gridX, gridY));
}
