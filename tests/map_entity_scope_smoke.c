#include "map.h"
#include "player.h"
#include "task_system.h"
#include "task_encounter_internal.h"
#include "task_runtime_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) return;
    fprintf(stderr, "map_entity_scope smoke failed: %s\n", message);
    exit(1);
}

int main(void) {
    MapCatalog catalog;
    GameMap ship = {0};
    GameMap surface = {0};
    TaskSystem tasks;
    Player player;
    int exteriorMonsterIndex = -1;
    int exteriorNodeIndex = -1;
    int markerX;
    int markerY;
    int index;

    Require(MapCatalog_Load(&catalog, "maps/map_catalog.json"), "catalog should load");
    Require(Map_LoadById(&ship, &catalog, "ship_interior"), "ship map should load");
    Require(Map_LoadById(&surface, &catalog, "planet_surface_01"), "surface map should load");
    Player_Init(&player);
    Tasks_Init(&tasks, &ship);
    Require(Tasks_RegisterMapEntities(&tasks, &surface), "surface entities should register once");
    Require(Tasks_RegisterMapEntities(&tasks, &surface), "re-registering same map should be idempotent");
    Require(tasks.registeredMapCount == 2 && tasks.nodeCount == 69
                && tasks.monsterCount == 8 && tasks.logCount == 14,
            "split maps should reconstruct the stable V15 entity ordering without duplication");

    for (index = 0; index < tasks.monsterCount; index++) {
        if (strcmp(tasks.monsters[index].mapId, "planet_surface_01") == 0) {
            exteriorMonsterIndex = index;
            break;
        }
    }
    for (index = 0; index < tasks.nodeCount; index++) {
        if (strcmp(tasks.nodes[index].mapId, "planet_surface_01") == 0) {
            exteriorNodeIndex = index;
            break;
        }
    }
    Require(exteriorMonsterIndex >= 0 && exteriorNodeIndex >= 0,
            "surface monster and resource should carry stable map ownership");

    Tasks_SetActiveMap(&tasks, &ship);
    Require(TasksRuntime_FindMonsterAt(&tasks,
                                       tasks.monsters[exteriorMonsterIndex].gridX,
                                       tasks.monsters[exteriorMonsterIndex].gridY) == NULL,
            "surface monster must not collide or target while ship interior is active");
    tasks.nodes[exteriorNodeIndex].active = false;
    Tasks_SetActiveMap(&tasks, &surface);
    Require(TasksRuntime_FindMonsterAt(&tasks,
                                       tasks.monsters[exteriorMonsterIndex].gridX,
                                       tasks.monsters[exteriorMonsterIndex].gridY) != NULL,
            "surface monster should become active when its owning map is active");
    Tasks_SetActiveMap(&tasks, &ship);
    Tasks_SetActiveMap(&tasks, &surface);
    Require(!tasks.nodes[exteriorNodeIndex].active,
            "collected resource state must survive repeated active-map changes");

    tasks.stage = 4;
    player.hasLaserGun = false;
    player.hasProtectionSuit = false;
    Require(Tasks_GetObjectiveMarkerForMap(&tasks, &surface, &player, &markerX, &markerY)
                && markerX == surface.portals[0].gridX + surface.portals[0].width / 2,
            "surface objective for an interior workbench should route to the ship portal first");
    player.hasLaserGun = true;
    player.hasProtectionSuit = true;
    Tasks_SetActiveMap(&tasks, &ship);
    Require(Tasks_GetObjectiveMarkerForMap(&tasks, &ship, &player, &markerX, &markerY)
                && markerX == ship.portals[0].gridX + ship.portals[0].width / 2,
            "interior objective for an exterior crash site should route to the airlock first");

    Map_Destroy(&surface);
    Map_Destroy(&ship);
    puts("map_entity_scope smoke ok");
    return 0;
}
