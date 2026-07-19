#include "map_scene_manager.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool ok, const char *message) {
    if (!ok) { fprintf(stderr, "map_state smoke failed: %s\n", message); exit(1); }
}
static void Travel(MapSceneManager *manager, GameMap *map, const char *portalId) {
    const MapPortal *portal = Map_GetPortalById(map, portalId);
    Require(portal != NULL, "portal should exist");
    Require(MapSceneManager_RequestPortal(manager, map, portal), "portal request should start");
    MapSceneManager_MarkFadingOut(manager);
    Require(MapSceneManager_PreparePending(manager, map), "target should load");
    Require(MapSceneManager_CommitPending(manager, map), "target should commit");
    MapSceneManager_FinishTransition(manager);
}
int main(void) {
    GameMap map = {0};
    MapSceneManager manager = {0};
    Require(MapCatalog_Load(&manager.catalog, "maps/map_catalog.json"), "catalog should load");
    Require(Map_LoadById(&map, &manager.catalog, "planet_surface_01"), "formal surface should load");
    memset(&manager, 0, sizeof(manager));
    Require(MapSceneManager_Init(&manager, &map, "maps/map_catalog.json"), "manager should initialize");
    Require(!Map_IsUnlockOpen(&map, "RUINS_GATE"), "ruins gate should start closed");
    Require(Map_SetUnlockOpen(&map, "RUINS_GATE", true), "ruins gate should open");
    Map_SetFieldCamp(&map, 90, 60);
    Travel(&manager, &map, "SHIP_EXTERIOR_ENTRY");
    Require(strcmp(map.mapId, "ship_interior") == 0, "travel should enter ship");
    Travel(&manager, &map, "SHIP_AIRLOCK_EXIT");
    Require(strcmp(map.mapId, "planet_surface_01") == 0, "travel should return to surface");
    Require(Map_IsUnlockOpen(&map, "RUINS_GATE"), "opened unlock should survive round trip");
    Require(map.campPlaced && map.campX == 90 && map.campY == 60, "camp should survive round trip");
    MapSceneManager_Destroy(&manager);
    Map_Destroy(&map);
    puts("map_state smoke ok");
    return 0;
}
