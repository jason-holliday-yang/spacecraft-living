#include "game_manager_internal.h"

#include <cmath>

static bool IsFiniteVector(Vector2 value) {
    return std::isfinite(value.x) && std::isfinite(value.y);
}

void Game_SanitizeGameplayState(Game *game, int screenWidth, int screenHeight) {
    bool playerTileInvalid;

    if (game == NULL
        || game->state == GAME_STATE_INTRO
        || game->state == GAME_STATE_OPENING
        || game->storySceneOpen) {
        return;
    }

    playerTileInvalid = !Map_IsWithinBounds(game->player.gridX, game->player.gridY)
        || !Map_IsWalkable(&game->map, game->player.gridX, game->player.gridY);
    if (playerTileInvalid) {
        game->player.gridX = PLAYER_RESPAWN_X;
        game->player.gridY = PLAYER_RESPAWN_Y;
        game->player.facingX = 0;
        game->player.facingY = 1;
        Player_UpdateWorldPosition(&game->player);
        MiniMap_Update(&game->miniMap, &game->player, &game->map);
    } else if (!IsFiniteVector(game->player.worldPos)
               || !IsFiniteVector(game->player.renderPos)
               || std::fabs(game->player.worldPos.x - Map_GridToWorld(game->player.gridX, game->player.gridY).x) > 0.5f
               || std::fabs(game->player.worldPos.y - Map_GridToWorld(game->player.gridX, game->player.gridY).y) > 0.5f) {
        Player_UpdateWorldPosition(&game->player);
    }

    if (game->tasks.objective[0] == '\0') {
        Tasks_UpdateObjective(&game->tasks, &game->player);
    }

    if (!std::isfinite(game->camera.zoom) || game->camera.zoom <= 0.0f) {
        game->camera.zoom = CAMERA_ZOOM;
    }
    if (!std::isfinite(game->camera.rotation)) {
        game->camera.rotation = 0.0f;
    }
    if (!IsFiniteVector(game->camera.target)) {
        game->camera.target = game->player.renderPos;
    }
    if (!IsFiniteVector(game->camera.offset)
        || std::fabs(game->camera.offset.x) < 0.5f
        || std::fabs(game->camera.offset.y) < 0.5f) {
        game->camera.offset = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    }
}
