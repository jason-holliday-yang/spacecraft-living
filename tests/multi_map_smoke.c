#include "map.h"
#include "resource_path.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }
    fprintf(stderr, "multi_map smoke failed: %s\n", message);
    exit(1);
}

static bool GetAnchorByName(const GameMap *map, const char *name, int *gridX, int *gridY) {
    int index;

    for (index = 0; index < MAP_ANCHOR_COUNT; index++) {
        MapAnchor anchor;

        anchor = (MapAnchor)index;
        if (strcmp(Map_GetAnchorName(anchor), name) == 0) {
            return Map_GetAnchorPosition(map, anchor, gridX, gridY);
        }
    }
    return false;
}

static void RequireMultiTileBounds(const GameMap *map,
                                   TileType tile,
                                   int expectedX,
                                   int expectedY,
                                   int expectedWidth,
                                   int expectedHeight,
                                   const char *message) {
    int gridY;
    int gridX;

    for (gridY = expectedY; gridY < expectedY + expectedHeight; gridY++) {
        for (gridX = expectedX; gridX < expectedX + expectedWidth; gridX++) {
            int originX;
            int originY;
            int width;
            int height;

            Require(Map_GetMultiTilePropBounds(map,
                                                       tile,
                                                       gridX,
                                                       gridY,
                                                       &originX,
                                                       &originY,
                                                       &width,
                                                       &height)
                        && originX == expectedX
                        && originY == expectedY
                        && width == expectedWidth
                        && height == expectedHeight,
                    message);
        }
    }
}

static void RequirePortalTarget(const MapCatalog *catalog, const MapPortal *portal) {
    GameMap targetMap = {0};
    int targetX;
    int targetY;

    Require(portal != NULL, "portal should exist");
    Require(MapCatalog_Find(catalog, portal->targetMapId) != NULL,
            "portal target map ID should be registered in the catalog");
    Require(Map_LoadById(&targetMap, catalog, portal->targetMapId),
            "portal target map should load independently");
    Require(GetAnchorByName(&targetMap, portal->targetAnchor, &targetX, &targetY),
            "portal target anchor should exist on the target map");
    Require(Map_IsWalkable(&targetMap, targetX, targetY),
            "portal target anchor should be walkable");
    Map_Destroy(&targetMap);
}

int main(void) {
    MapCatalog catalog;
    const MapCatalogEntry *shipEntry;
    const MapCatalogEntry *surfaceEntry;
    GameMap ship = {0};
    GameMap surface = {0};
    GameMap dynamicMap = {0};
    GameMap missingMap = {0};
    GameMap invalidObjectMap = {0};
    const MapPortal *shipExit;
    const MapPortal *shipEntryPortal;
    const MapEntity *shipEntity;
    int spawnX;
    int spawnY;
    char shipTexturePath[2048];
    Image shipTextureImage;

    Require(MapCatalog_Load(&catalog, "maps/map_catalog.json"),
            "map catalog should load");
    Require(catalog.loaded && catalog.entryCount >= 3,
            "map catalog should register the legacy baseline and both split maps");
    Require(strcmp(catalog.defaultMapId, "legacy_world") == 0,
            "legacy combined map should remain the compatibility initialization default");

    shipEntry = MapCatalog_Find(&catalog, "ship_interior");
    surfaceEntry = MapCatalog_Find(&catalog, "planet_surface_01");
    Require(shipEntry != NULL && shipEntry->kind == MAP_KIND_INTERIOR,
            "ship interior catalog identity should be stable");
    Require(surfaceEntry != NULL && surfaceEntry->kind == MAP_KIND_EXTERIOR,
            "legacy surface catalog identity should be stable");

    Require(Map_LoadById(&ship, &catalog, "ship_interior"),
            "ship interior should load by map ID");
    Require(Map_LoadById(&surface, &catalog, "planet_surface_01"),
            "legacy surface should load by map ID");
    Require(strcmp(ship.mapId, "ship_interior") == 0
                && strcmp(ship.sourcePath, "maps/ship_interior.tmj") == 0
                && ship.mapKind == MAP_KIND_INTERIOR,
            "loaded ship map should retain ID, source path, and kind");
    Require(strcmp(surface.mapId, "planet_surface_01") == 0
                && strcmp(surface.sourcePath, "maps/planet_surface_01.tmj") == 0
                && surface.mapKind == MAP_KIND_EXTERIOR,
            "loaded surface map should retain ID, source path, and kind");

    Require(ship.width == 126 && ship.height == 104
                && surface.width == 126 && surface.height == 104,
            "first split should preserve legacy coordinates and map dimensions");
    Require(ship.hasPortalLayer && ship.portalCount == 1 && ship.hasEntityLayer,
            "ship map should load the Portals and Entities layers");
    Require(surface.hasPortalLayer && surface.portalCount == 1
                && surface.hasEntityLayer && surface.entityCount == 1,
            "surface map should load its portal and large ship entity");
    Require(ship.monsterSeedCount == 0,
            "ship interior should not load exterior monsters");
    Require(!ship.anchorSet[MAP_ANCHOR_COMM_RELAY]
                && !surface.anchorSet[MAP_ANCHOR_WORKBENCH],
            "split maps should not expose anchors owned by the other scene");
    Require(strcmp(Map_GetLocationNameAtMap(&ship, 63, 52), "Central Corridor") == 0,
            "explicit ship query should use the ship region layer");
    Require(strcmp(Map_GetLocationNameAtMap(&surface, 64, 17), "Signal Tower Plateau") == 0,
            "explicit surface query should use the surface region layer");

    shipExit = Map_GetPortalById(&ship, "SHIP_AIRLOCK_EXIT");
    shipEntryPortal = Map_GetPortalById(&surface, "SHIP_EXTERIOR_ENTRY");
    Require(shipExit != NULL && strcmp(shipExit->targetMapId, "planet_surface_01") == 0,
            "interior airlock portal should target the surface map ID");
    Require(shipEntryPortal != NULL && strcmp(shipEntryPortal->targetMapId, "ship_interior") == 0,
            "surface ship portal should target the interior map ID");
    RequirePortalTarget(&catalog, shipExit);
    RequirePortalTarget(&catalog, shipEntryPortal);

    shipEntity = Map_GetEntityById(&surface, "PLAYER_SHIP");
    Require(shipEntity != NULL
                && strcmp(shipEntity->entityType, "ship_exterior") == 0
                && strcmp(shipEntity->portalId, "SHIP_EXTERIOR_ENTRY") == 0
                && shipEntity->width == 6
                && shipEntity->height == 6
                && shipEntity->blocksMovement,
            "surface entity should describe a blocking large exterior ship visual");
    Require(ResourcePath_Resolve("resources/images/world/entities/entity_player_ship_exterior_alpha.png",
                                 shipTexturePath,
                                 sizeof(shipTexturePath)),
            "exterior ship texture should resolve as a runtime resource");
    shipTextureImage = LoadImage(shipTexturePath);
    Require(shipTextureImage.data != NULL
                && shipTextureImage.width > 0
                && shipTextureImage.height > 0
                && GetImageColor(shipTextureImage, 0, 0).a == 0,
            "exterior ship texture should load with a transparent background");
    UnloadImage(shipTextureImage);
    Require(GetAnchorByName(&surface, shipEntity->originAnchor, NULL, NULL),
            "ship entity origin anchor should exist");
    RequireMultiTileBounds(&surface, TILE_COMM_RELAY, 92, 46, 3, 3,
                           "comm relay tiles should resolve to one 3x3 visual");
    RequireMultiTileBounds(&surface, TILE_CRASH_CLUE, 106, 34, 4, 4,
                           "crash wreck tiles should resolve to one 4x4 visual");
    RequireMultiTileBounds(&surface, TILE_ENERGY_CONSOLE, 108, 49, 3, 3,
                           "energy core tiles should resolve to one 3x3 visual");
    RequireMultiTileBounds(&surface, TILE_SIGNAL_TOWER, 63, 16, 3, 3,
                           "signal tower tiles should resolve to one 3x3 visual");
    RequireMultiTileBounds(&surface, TILE_MONOLITH, 53, 22, 3, 3,
                           "first monolith tiles should resolve to one 3x3 visual");
    RequireMultiTileBounds(&surface, TILE_MONOLITH, 63, 19, 3, 3,
                           "second monolith tiles should resolve to one 3x3 visual");
    RequireMultiTileBounds(&surface, TILE_MONOLITH, 74, 23, 3, 3,
                           "third monolith tiles should resolve to one 3x3 visual");
    Require(Map_GetPropTileAt(&surface, shipEntity->gridX, shipEntity->gridY) == TILE_ENTITY_BLOCKER
                && !Map_IsWalkable(&surface, shipEntity->gridX, shipEntity->gridY)
                && Map_GetPropTileAt(&surface, shipEntity->gridX - 1, shipEntity->gridY) == TILE_VOID
                && Map_GetPropTileAt(&surface, shipEntity->gridX + shipEntity->width, 51) == TILE_VOID,
            "surface ship should use an invisible 6x6 Props collision volume without surrounding rocks");
    surface.propTiles[shipEntity->gridY][shipEntity->gridX] = TILE_VOID;
    Require(!Map_IsWalkable(&surface, shipEntity->gridX, shipEntity->gridY),
            "surface ship entity should preserve collision when its Props fallback is missing");
    Require(GetAnchorByName(&surface, "SHIP_EXIT_SPAWN", &spawnX, &spawnY)
                && Map_IsWalkable(&surface, spawnX, spawnY)
                && Map_GetHazardAt(&surface, spawnX, spawnY) == HAZARD_NONE,
            "surface ship exit spawn should be a safe walkable anchor");

    Require(Map_LoadTiledMap(&dynamicMap,
                             "dynamic_fixture",
                             "tests/fixtures/dynamic_map.tmj",
                             MAP_KIND_TEST),
            "generic loader should load an arbitrary-size map with explicit identity");
    Require(dynamicMap.width == 4 && dynamicMap.height == 3
                && strcmp(dynamicMap.mapId, "dynamic_fixture") == 0
                && dynamicMap.mapKind == MAP_KIND_TEST,
            "arbitrary-size map should retain dimensions and identity");
    Require(strcmp(Map_GetLocationNameAtMap(&ship, 63, 52), "Central Corridor") == 0,
            "loading another map must not change explicit queries against the ship map");
    Require(Map_GetAreaAtMap(&surface, 64, 17) == MAP_AREA_RUINS,
            "loading another map must not change explicit area queries against the surface map");

    Require(!Map_LoadTiledMap(&missingMap,
                              "missing_scene",
                              "tests/fixtures/does_not_exist.tmj",
                              MAP_KIND_TEST),
            "missing generic map should fail explicitly");
    Require(strstr(Map_GetLoadError(&missingMap), "missing_scene") != NULL
                && strstr(Map_GetLoadError(&missingMap), "tests/fixtures/does_not_exist.tmj") != NULL,
            "map load error should include both map ID and file path");

    Require(!Map_LoadTiledMap(&invalidObjectMap,
                              "invalid_objects",
                              "tests/fixtures/invalid_map_objects.tmj",
                              MAP_KIND_TEST),
            "malformed Portal or Entity data should fail map validation");
    Require(strstr(Map_GetLoadError(&invalidObjectMap), "invalid_objects") != NULL
                && strstr(Map_GetLoadError(&invalidObjectMap), "invalid_map_objects.tmj") != NULL,
            "object validation failure should retain map identity and source path");

    Map_Destroy(&invalidObjectMap);
    Map_Destroy(&missingMap);
    Map_Destroy(&dynamicMap);
    Map_Destroy(&surface);
    Map_Destroy(&ship);
    puts("multi_map smoke ok");
    return 0;
}
