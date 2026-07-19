#include "game_manager_internal.h"
#include "task_presentation.h"

#include <cmath>

static bool IsFiniteVector(Vector2 value) {
    return std::isfinite(value.x) && std::isfinite(value.y);
}

void Game_SanitizeGameplayState(Game *game, int screenWidth, int screenHeight) {
    bool playerTileInvalid;

    if (game == NULL
        || game->flow.state == GAME_STATE_INTRO
        || game->flow.state == GAME_STATE_OPENING
        || game->flow.storySceneOpen) {
        return;
    }

    playerTileInvalid = !Map_IsWithinMapBounds(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY)
        || !Map_IsWalkable(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY);
    if (playerTileInvalid) {
        game->runtime.player.gridX = PLAYER_RESPAWN_X;
        game->runtime.player.gridY = PLAYER_RESPAWN_Y;
        game->runtime.player.facingX = 0;
        game->runtime.player.facingY = 1;
        Player_UpdateWorldPosition(&game->runtime.player);
        MiniMap_Update(&game->runtime.miniMap, &game->runtime.player, &game->runtime.map);
    } else if (!IsFiniteVector(game->runtime.player.worldPos)
               || !IsFiniteVector(game->runtime.player.renderPos)
               || std::fabs(game->runtime.player.worldPos.x - Map_GridToWorld(game->runtime.player.gridX, game->runtime.player.gridY).x) > 0.5f
               || std::fabs(game->runtime.player.worldPos.y - Map_GridToWorld(game->runtime.player.gridX, game->runtime.player.gridY).y) > 0.5f) {
        Player_UpdateWorldPosition(&game->runtime.player);
    }

    if (game->runtime.tasks.objective[0] == '\0') {
        Tasks_UpdateObjective(&game->runtime.tasks, &game->runtime.player);
    }

    if (!std::isfinite(game->runtime.camera.zoom) || game->runtime.camera.zoom <= 0.0f) {
        game->runtime.camera.zoom = CAMERA_ZOOM;
    }
    if (!std::isfinite(game->runtime.camera.rotation)) {
        game->runtime.camera.rotation = 0.0f;
    }
    if (!IsFiniteVector(game->runtime.camera.target)) {
        game->runtime.camera.target = game->runtime.player.renderPos;
    }
    if (!IsFiniteVector(game->runtime.camera.offset)
        || std::fabs(game->runtime.camera.offset.x) < 0.5f
        || std::fabs(game->runtime.camera.offset.y) < 0.5f) {
        game->runtime.camera.offset = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    }
}
