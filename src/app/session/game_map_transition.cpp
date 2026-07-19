#include "game_session_internal.h"
#include "task_presentation.h"

#include <cstdio>
#include <cstring>

static const MapPortal *FindReturnPortal(const GameMap *map, const char *targetMapId) {
    int index;
    if (map == nullptr || targetMapId == nullptr) {
        return nullptr;
    }
    for (index = 0; index < map->portalCount; ++index) {
        if (std::strcmp(map->portals[index].targetMapId, targetMapId) == 0) {
            return &map->portals[index];
        }
    }
    return nullptr;
}

bool Game_BeginPortalTransition(Game *game, const MapPortal *portal) {
    if (game == nullptr || portal == nullptr || game->flow.state != GAME_STATE_PLAYING
        || game->flow.screenTransitionActive || MapSceneManager_IsBusy(&game->services.mapScene)) {
        return false;
    }
    if (portal->requiresFlag[0] != '\0') {
        int requiredStage = 0;
        if (std::sscanf(portal->requiresFlag, "stage_%d", &requiredStage) == 1
            && game->runtime.tasks.stage < requiredStage) {
            Game_PostMessage(game,
                             Loc_PickLiteral("The airlock route is not unlocked yet.", "气闸路线尚未解锁。"),
                             3.2f);
            return false;
        }
    }
    if (!MapSceneManager_RequestPortal(&game->services.mapScene, &game->runtime.map, portal)) {
        const char *error = MapSceneManager_GetError(&game->services.mapScene);
        if (error[0] != '\0') {
            Game_PostMessage(game, error, 5.0f);
        }
        return false;
    }

    Game_CloseTransientOverlays(game);
    Game_ResetTransientGameplayState(game);
    MapSceneManager_MarkFadingOut(&game->services.mapScene);
    Game_BeginScreenTransition(game, SCREEN_TRANSITION_MAP_CHANGE, -1);
    return true;
}

bool Game_ResolveMapTransition(Game *game) {
    int spawnX;
    int spawnY;

    if (game == nullptr) {
        return false;
    }
    if (!MapSceneManager_PreparePending(&game->services.mapScene, &game->runtime.map)) {
        char message[256];
        std::snprintf(message, sizeof(message), "%s: %s",
                      Loc_PickLiteral("Map transition failed", "地图切换失败"),
                      MapSceneManager_GetError(&game->services.mapScene));
        Game_PostMessage(game, message, 6.0f);
        Map_ActivateRuntime(&game->runtime.map);
        return false;
    }
    if (!Tasks_RegisterMapEntities(&game->runtime.tasks, &game->services.mapScene.stagedMap)) {
        MapSceneManager_SetError(&game->services.mapScene, "Target map entity state could not be registered safely");
        Game_PostMessage(game,
                         Loc_PickLiteral("Map transition failed: world entity capacity or seed validation error.",
                                         "地图切换失败：世界实体容量或生成点校验错误。"),
                         6.0f);
        Map_ActivateRuntime(&game->runtime.map);
        return false;
    }
    MapSceneManager_ApplyMapState(&game->services.mapScene, &game->services.mapScene.stagedMap);

    spawnX = game->services.mapScene.stagedSpawnX;
    spawnY = game->services.mapScene.stagedSpawnY;
    if (!MapSceneManager_CommitPending(&game->services.mapScene, &game->runtime.map)) {
        MapSceneManager_SetError(&game->services.mapScene, "Prepared map could not be committed");
        return false;
    }

    Tasks_SetActiveMap(&game->runtime.tasks, &game->runtime.map);
    Tasks_ApplyProgressToMap(&game->runtime.tasks, &game->runtime.map);
    if (Tasks_IsBlockingActorTile(&game->runtime.tasks, spawnX, spawnY)
        && !Game_FindNearestSafeLoadedPlayerTile(game, spawnX, spawnY, &spawnX, &spawnY)) {
        const MapPortal *returnPortal = FindReturnPortal(&game->runtime.map, game->services.mapScene.previousMapId);
        if (returnPortal != nullptr) {
            spawnX = returnPortal->gridX;
            spawnY = returnPortal->gridY;
        }
    }

    game->runtime.player.gridX = spawnX;
    game->runtime.player.gridY = spawnY;
    game->runtime.player.facingX = game->services.mapScene.stagedFacingX;
    game->runtime.player.facingY = game->services.mapScene.stagedFacingY;
    Player_UpdateWorldPosition(&game->runtime.player);
    Game_ResetTransientGameplayState(game);
    MiniMap_Init(&game->runtime.miniMap, &game->runtime.map);
    MiniMap_Update(&game->runtime.miniMap, &game->runtime.player, &game->runtime.map);
    Game_ResetCameraToPlayer(game);
    game->runtime.lastLocationName[0] = '\0';
    Game_SyncTrackedLocation(game);
    Tasks_UpdateObjective(&game->runtime.tasks, &game->runtime.player);
    Audio_SetScene(&game->services.audio, Game_SelectAudioScene(game));
    Audio_SetMusicStage(&game->services.audio, Game_SelectMusicStage(game));
    Game_PostMessage(game,
                     game->runtime.map.mapKind == MAP_KIND_INTERIOR
                         ? Loc_PickLiteral("Airlock seal confirmed. You are back inside the ship.", "气闸密封确认。你已返回飞船内部。")
                         : Loc_PickLiteral("Outer hatch clear. Surface link established.", "外舱门已清空。地表链路已建立。"),
                     3.8f);
    return true;
}

void Game_FinishMapTransition(Game *game) {
    if (game != nullptr) {
        MapSceneManager_FinishTransition(&game->services.mapScene);
    }
}

bool Game_LoadLegacyCompatibilityMap(Game *game) {
    GameMap compatibilityMap{};
    if (game == nullptr || !game->services.mapScene.catalog.loaded
        || !Map_LoadById(&compatibilityMap, &game->services.mapScene.catalog, "legacy_world")) {
        Map_Destroy(&compatibilityMap);
        return false;
    }
    Map_Destroy(&game->runtime.map);
    game->runtime.map = compatibilityMap;
    Map_ActivateRuntime(&game->runtime.map);
    MapSceneManager_BindActiveMap(&game->services.mapScene, &game->runtime.map);
    Tasks_SetActiveMap(&game->runtime.tasks, &game->runtime.map);
    MiniMap_Init(&game->runtime.miniMap, &game->runtime.map);
    return true;
}
