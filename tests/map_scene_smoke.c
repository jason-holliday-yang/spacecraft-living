#include "map_scene_manager.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) return;
    fprintf(stderr, "map_scene smoke failed: %s\n", message);
    exit(1);
}

static void CompletePortalTransition(MapSceneManager *manager, GameMap *activeMap, const char *portalId) {
    const MapPortal *portal = Map_GetPortalById(activeMap, portalId);
    Require(portal != NULL, "source portal should exist");
    Require(MapSceneManager_RequestPortal(manager, activeMap, portal), "portal request should be accepted");
    Require(!MapSceneManager_RequestPortal(manager, activeMap, portal), "duplicate portal request should be rejected while busy");
    MapSceneManager_MarkFadingOut(manager);
    Require(MapSceneManager_PreparePending(manager, activeMap), "target map should prepare in temporary storage");
    Require(MapSceneManager_CommitPending(manager, activeMap), "prepared target map should atomically replace active map");
    MapSceneManager_FinishTransition(manager);
    Require(manager->phase == MAP_TRANSITION_IDLE, "successful transition should return to idle");
}

int main(void) {
    GameMap activeMap = {0};
    MapSceneManager manager = {0};
    int trip;
    int savedFileIndex;
    char savedFile[MAP_SOURCE_PATH_MAX];
    const MapPortal *exitPortal;

    Require(MapCatalog_Load(&manager.catalog, "maps/map_catalog.json"), "catalog baseline should load");
    Require(Map_LoadById(&activeMap, &manager.catalog, "ship_interior"), "ship interior should load");
    memset(&manager, 0, sizeof(manager));
    Require(MapSceneManager_Init(&manager, &activeMap, "maps/map_catalog.json"), "scene manager should initialize");

    for (trip = 0; trip < 20; trip++) {
        CompletePortalTransition(&manager, &activeMap, "SHIP_AIRLOCK_EXIT");
        Require(strcmp(activeMap.mapId, "planet_surface_01") == 0,
                "exit transition should reach surface map");
        Require(activeMap.mapId[0] != '\0' && manager.stagedSpawnX == 83 && manager.stagedSpawnY == 52,
                "surface arrival should use safe ship-side Anchor");
        CompletePortalTransition(&manager, &activeMap, "SHIP_EXTERIOR_ENTRY");
        Require(strcmp(activeMap.mapId, "ship_interior") == 0,
                "entry transition should return to ship interior");
        Require(manager.stagedSpawnX == 76 && manager.stagedSpawnY == 52,
                "interior arrival should use airlock return Anchor");
    }

    exitPortal = Map_GetPortalById(&activeMap, "SHIP_AIRLOCK_EXIT");
    Require(exitPortal != NULL, "exit portal should remain available after repeated trips");
    savedFileIndex = (int)(MapCatalog_Find(&manager.catalog, "planet_surface_01") - manager.catalog.entries);
    snprintf(savedFile, sizeof(savedFile), "%s", manager.catalog.entries[savedFileIndex].file);
    snprintf(manager.catalog.entries[savedFileIndex].file,
             sizeof(manager.catalog.entries[savedFileIndex].file),
             "tests/fixtures/does_not_exist.tmj");
    Require(MapSceneManager_RequestPortal(&manager, &activeMap, exitPortal), "failure case request should start");
    MapSceneManager_MarkFadingOut(&manager);
    Require(!MapSceneManager_PreparePending(&manager, &activeMap), "missing target file should fail before swap");
    Require(strcmp(activeMap.mapId, "ship_interior") == 0,
            "failed target load must keep the original active map");
    Require(manager.phase == MAP_TRANSITION_ERROR && MapSceneManager_GetError(&manager)[0] != '\0',
            "failed target load should retain explicit manager error state");
    snprintf(manager.catalog.entries[savedFileIndex].file,
             sizeof(manager.catalog.entries[savedFileIndex].file), "%s", savedFile);

    MapSceneManager_Destroy(&manager);
    Map_Destroy(&activeMap);
    puts("map_scene smoke ok");
    return 0;
}
