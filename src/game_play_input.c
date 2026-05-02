#include "game_play_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"

#include <string.h>

static bool GetPressedDirection(int *deltaX, int *deltaY) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        *deltaX = 0;
        *deltaY = -1;
        return true;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        *deltaX = 0;
        *deltaY = 1;
        return true;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        *deltaX = -1;
        *deltaY = 0;
        return true;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        *deltaX = 1;
        *deltaY = 0;
        return true;
    }
    return false;
}

static bool GetHeldDirection(int *deltaX, int *deltaY) {
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        *deltaX = 0;
        *deltaY = -1;
        return true;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        *deltaX = 0;
        *deltaY = 1;
        return true;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        *deltaX = -1;
        *deltaY = 0;
        return true;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        *deltaX = 1;
        *deltaY = 0;
        return true;
    }
    return false;
}

static Vector2 GetMonsterCenterWorld(const Monster *monster) {
    int originX;
    int originY;
    int width;
    int height;

    TasksRuntime_GetMonsterFootprint(monster, &originX, &originY, &width, &height);
    return (Vector2){
        (float)(originX * TILE_SIZE) + (float)(width * TILE_SIZE) * 0.5f,
        (float)(originY * TILE_SIZE) + (float)(height * TILE_SIZE) * 0.5f
    };
}

static Vector2 GetLaserMissEndWorld(const Player *player) {
    int facingX;
    int facingY;
    int step;

    facingX = player->facingX;
    facingY = player->facingY;
    if (facingX == 0 && facingY == 0) {
        facingY = 1;
    }
    step = player->hasLaserGun ? 3 : 1;
    return Map_GridToWorld(player->gridX + facingX * step, player->gridY + facingY * step);
}

static void GamePlay_StartLaserEffect(Game *game, const Monster *target, bool hit) {
    if (game == NULL || !game->player.hasLaserGun) {
        return;
    }

    game->laserEffectTimer = 0.18f;
    game->laserEffectHit = hit && target != NULL;
    game->laserEffectStart = (Vector2){
        game->player.renderPos.x,
        game->player.renderPos.y - (float)TILE_SIZE * 0.28f
    };
    game->laserEffectEnd = target != NULL ? GetMonsterCenterWorld(target) : GetLaserMissEndWorld(&game->player);
}

static bool SameDirection(int ax, int ay, int bx, int by) {
    return ax == bx && ay == by;
}

static bool MessageStartsWith(const char *message, const char *prefix) {
    size_t prefixLength;

    if (message == NULL || prefix == NULL) {
        return false;
    }

    prefixLength = strlen(prefix);
    return strncmp(message, prefix, prefixLength) == 0;
}

static AudioCue GetFootstepCueForTile(TileType groundTile, MapArea area) {
    switch (groundTile) {
        case TILE_FOREST_GROUND:
            return AUDIO_CUE_STEP_FOREST;
        case TILE_SWAMP_GROUND:
        case TILE_DEEP_SWAMP_GROUND:
            return AUDIO_CUE_STEP_SWAMP;
        case TILE_BASE_FLOOR:
        case TILE_RUINS_GROUND:
        case TILE_VOID:
        default:
            break;
    }

    switch (area) {
        case MAP_AREA_FOREST:
            return AUDIO_CUE_STEP_FOREST;
        case MAP_AREA_SWAMP_OUTER:
        case MAP_AREA_SWAMP_DEEP:
            return AUDIO_CUE_STEP_SWAMP;
        case MAP_AREA_RUINS:
        case MAP_AREA_BOSS_ARENA:
        case MAP_AREA_BASE:
        case MAP_AREA_UNKNOWN:
        default:
            return AUDIO_CUE_STEP_METAL;
    }
}

static bool IsMonolithInteractionTarget(TaskInteractionTarget target) {
    return target == TASK_INTERACTION_MONOLITH_A
        || target == TASK_INTERACTION_MONOLITH_B
        || target == TASK_INTERACTION_MONOLITH_C;
}

static AudioCue SelectInteractionCue(const char *message,
                                     TaskInteractionTarget target,
                                     const ResourceNode *nearbyNode,
                                     const ShipLog *nearbyLog) {
    if (nearbyNode != NULL) {
        return AUDIO_CUE_COLLECT;
    }
    if (nearbyLog != NULL) {
        return AUDIO_CUE_LOG;
    }
    if (IsMonolithInteractionTarget(target)) {
        return AUDIO_CUE_MONOLITH;
    }
    if (MessageStartsWith(message, "Collected ") || MessageStartsWith(message, "Collected")
        || MessageStartsWith(message, "获得了") || MessageStartsWith(message, "收集了")) {
        return AUDIO_CUE_COLLECT;
    }
    if (strstr(message, "Missing materials") != NULL
        || strstr(message, "still needs") != NULL
        || strstr(message, "requires") != NULL
        || strstr(message, "offline") != NULL
        || strstr(message, "depleted") != NULL
        || strstr(message, "exhausted") != NULL
        || strstr(message, "缺少") != NULL
        || strstr(message, "仍需要") != NULL
        || strstr(message, "需要") != NULL
        || strstr(message, "未上线") != NULL
        || strstr(message, "已耗尽") != NULL) {
        return AUDIO_CUE_WARNING;
    }
    return AUDIO_CUE_REPAIR;
}

static bool IsCrewQuartersInteraction(const Game *game, TaskInteractionTarget target) {
    const char *roomName;

    if (game == NULL || target != TASK_INTERACTION_NONE || Map_GetAreaAt(game->player.gridX, game->player.gridY) != MAP_AREA_BASE) {
        return false;
    }

    roomName = Map_GetRoomNameAt(game->player.gridX, game->player.gridY);
    return roomName != NULL && strcmp(roomName, "Crew Quarters") == 0;
}

static bool IsNearLoxiTerminal(const Game *game) {
    if (game == NULL) {
        return false;
    }

    return game->player.gridX >= LOXI_TERMINAL_X - 2
        && game->player.gridX <= LOXI_TERMINAL_X + STATION_FOOTPRINT_WIDTH + 1
        && game->player.gridY >= LOXI_TERMINAL_Y - 2
        && game->player.gridY <= LOXI_TERMINAL_Y + STATION_FOOTPRINT_HEIGHT + 1;
}

static bool ShouldOpenSettlementConfirm(const Game *game) {
    const int availableEndingCount = game != NULL ? Tasks_GetAvailableEndingCount(&game->tasks) : 0;

    return game != NULL
        && availableEndingCount > 0
        && game->tasks.ending == ENDING_NONE
        && game->tasks.selectedEndingRoute == ENDING_NONE
        && game->tasks.endingArchiveReviewed
        && IsNearLoxiTerminal(game);
}

static bool TryOpenInfoOverlayTab(Game *game, InfoOverlayTab tab) {
    if (game == NULL) {
        return false;
    }

    if (tab == INFO_OVERLAY_TAB_LOXI && !Tasks_IsCommunicatorUnlocked(&game->tasks)) {
        Game_PostMessage(game,
                         Loc_PickLiteral("Loxi link is offline. Sync with the terminal bay uplink first, then press N.",
                                         "洛希链路尚未上线。请先在终端舱完成同步，再按 N。"),
                         2.8f);
        return true;
    }

    game->infoOverlayOpen = true;
    game->infoOverlayTab = tab;
    game->communicatorLogDetailOpen = false;
    game->communicatorLogDetailVisibility = 0.0f;
    game->communicatorLogDetailScroll = 0.0f;
    Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    return true;
}

static void HandleMovement(Game *game, int deltaX, int deltaY) {
    bool moved;
    Vector2 startPos;
    float moveDuration;
    int targetX;
    int targetY;

    if (deltaX != 0 || deltaY != 0) {
        game->player.facingX = deltaX;
        game->player.facingY = deltaY;
    }

    targetX = game->player.gridX + deltaX;
    targetY = game->player.gridY + deltaY;

    if (Tasks_IsBlockingActorTile(&game->tasks, targetX, targetY)) {
        Game_PostMessage(game, Loc_PickLiteral("A hostile creature is blocking the path.", "有敌对生物挡住了去路。"), 2.4f);
        game->player.moveTimer = BLOCKED_MOVE_COOLDOWN;
        return;
    }

    startPos = game->player.renderPos;
    moved = Player_Move(&game->player, &game->map, deltaX, deltaY);
    if (moved) {
        moveDuration = Player_GetMoveCooldown(&game->player);
        Player_StartMoveAnimation(&game->player, startPos, moveDuration);
        game->player.moveTimer = moveDuration;
        game->inputBufferTimer = 0.0f;
        game->bufferedMoveX = 0;
        game->bufferedMoveY = 0;
        Audio_PlayCue(&game->audio,
                      GetFootstepCueForTile(Map_GetGroundTileAt(&game->map, game->player.gridX, game->player.gridY),
                                            Map_GetAreaAt(game->player.gridX, game->player.gridY)));
        return;
    }

    Game_PostMessage(game, Loc_PickLiteral("That tile cannot be crossed.", "那里无法通行。"), 1.6f);
    game->player.moveTimer = BLOCKED_MOVE_COOLDOWN;
}

void GamePlay_UpdateMovement(Game *game) {
    static const float kBufferedMoveGrace = 0.03f;
    int pressedX;
    int pressedY;
    int heldX;
    int heldY;
    float queuedBufferTime;
    bool hasPressedInput;
    bool hasHeldInput;

    pressedX = 0;
    pressedY = 0;
    heldX = 0;
    heldY = 0;
    hasPressedInput = GetPressedDirection(&pressedX, &pressedY);
    hasHeldInput = GetHeldDirection(&heldX, &heldY);

    if (hasPressedInput) {
        queuedBufferTime = INPUT_BUFFER_TIME;
        if (game->player.moveTimer > 0.0f && queuedBufferTime < game->player.moveTimer + kBufferedMoveGrace) {
            queuedBufferTime = game->player.moveTimer + kBufferedMoveGrace;
        }
        game->bufferedMoveX = pressedX;
        game->bufferedMoveY = pressedY;
        game->inputBufferTimer = queuedBufferTime;
        game->heldMoveX = pressedX;
        game->heldMoveY = pressedY;
        game->holdRepeatTimer = HOLD_REPEAT_INITIAL_DELAY;
    }

    if (!hasHeldInput) {
        game->heldMoveX = 0;
        game->heldMoveY = 0;
        game->holdRepeatTimer = 0.0f;
    } else if (!SameDirection(game->heldMoveX, game->heldMoveY, heldX, heldY)) {
        game->heldMoveX = heldX;
        game->heldMoveY = heldY;
        game->holdRepeatTimer = HOLD_REPEAT_INITIAL_DELAY;
    } else if (!hasPressedInput && game->holdRepeatTimer <= 0.0f) {
        game->bufferedMoveX = heldX;
        game->bufferedMoveY = heldY;
        game->inputBufferTimer = INPUT_BUFFER_TIME;
        game->holdRepeatTimer = HOLD_REPEAT_INTERVAL;
    }

    if (game->player.moveTimer > 0.0f) {
        if (hasHeldInput
            && (heldX != 0 || heldY != 0)
            && (hasPressedInput || game->holdRepeatTimer <= kBufferedMoveGrace)) {
            queuedBufferTime = game->player.moveTimer + kBufferedMoveGrace;
            game->bufferedMoveX = heldX;
            game->bufferedMoveY = heldY;
            if (game->inputBufferTimer < queuedBufferTime) {
                game->inputBufferTimer = queuedBufferTime;
            }
        }
        return;
    }

    if (game->inputBufferTimer > 0.0f && (game->bufferedMoveX != 0 || game->bufferedMoveY != 0)) {
        HandleMovement(game, game->bufferedMoveX, game->bufferedMoveY);
    }
}

static bool TryHandleHudShortcutClick(Game *game) {
    Vector2 mouse;
    Rectangle rect;

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return false;
    }

    mouse = GetMousePosition();

    rect = UI_GetHudShortcutRect(GetScreenWidth(), GetScreenHeight(), 0);
    if (CheckCollisionPointRec(mouse, rect)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_MAP);
    }

    rect = UI_GetHudShortcutRect(GetScreenWidth(), GetScreenHeight(), 1);
    if (CheckCollisionPointRec(mouse, rect)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_BACKPACK);
    }

    rect = UI_GetHudShortcutRect(GetScreenWidth(), GetScreenHeight(), 2);
    if (CheckCollisionPointRec(mouse, rect)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_LOXI);
    }

    return false;
}

bool GamePlay_HandleImmediateInput(Game *game, char *actionMessage, size_t messageSize) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->pauseMenuOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
    }
    if (IsKeyPressed(KEY_B)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_BACKPACK);
    }
    if (IsKeyPressed(KEY_N)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_LOXI);
    }
    if (IsKeyPressed(KEY_H)) {
        game->helpOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
    }
    if (IsKeyPressed(KEY_O)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_SETTINGS);
    }
    if (IsKeyPressed(KEY_M)) {
        return TryOpenInfoOverlayTab(game, INFO_OVERLAY_TAB_MAP);
    }
    if (TryHandleHudShortcutClick(game)) {
        return true;
    }
    if (IsKeyPressed(KEY_F)) {
        GameEnding endingBeforeInteraction;
        bool shouldStartSleepTransition;
        bool shouldPrioritizePickup;
        TaskInteractionTarget preferredTarget;
        ResourceNode *nearbyNode;
        ShipLog *nearbyLog;

        shouldPrioritizePickup = TasksRuntime_HasNearbyPickupPriority(&game->tasks, &game->player);
        preferredTarget = TasksRuntime_GetPreferredInteractionTarget(&game->player);
        shouldStartSleepTransition = IsCrewQuartersInteraction(game, preferredTarget);
        nearbyNode = NULL;
        nearbyLog = NULL;

        if (preferredTarget != TASK_INTERACTION_LOXI_TERMINAL) {
            nearbyNode = TasksRuntime_FindNearbyNode(&game->tasks, &game->player);
            if (nearbyNode == NULL) {
                nearbyLog = TasksRuntime_FindNearbyLog(&game->tasks, &game->player);
            }
        }

        if (preferredTarget == TASK_INTERACTION_LOXI_TERMINAL
            && ShouldOpenSettlementConfirm(game)) {
            game->settlementConfirmOpen = true;
            game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return true;
        }

        endingBeforeInteraction = game->tasks.ending;
        if (!shouldPrioritizePickup && preferredTarget == TASK_INTERACTION_WORKBENCH) {
            game->craftOpen = true;
            game->selectedCraftIndex = 0;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return true;
        }

        if (Tasks_HandleInteraction(&game->tasks, &game->map, &game->player, actionMessage, messageSize)) {
            if (actionMessage[0] != '\0' && game->tasks.ending == endingBeforeInteraction) {
                Audio_PlayCue(&game->audio, SelectInteractionCue(actionMessage, preferredTarget, nearbyNode, nearbyLog));
            }
            if (actionMessage[0] != '\0') {
                Game_PostMessage(game, actionMessage, 4.0f);
            }
            if (shouldStartSleepTransition && game->tasks.ending == endingBeforeInteraction) {
                Game_BeginScreenTransition(game, SCREEN_TRANSITION_SLEEP_REST, -1);
            }
        } else {
            if (actionMessage[0] != '\0') {
                Game_PostMessage(game, actionMessage, 2.6f);
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        Monster *attackTarget;

        attackTarget = game->player.hasLaserGun ? TasksRuntime_FindAttackTarget(&game->tasks, &game->player) : NULL;
        if (Tasks_HandleAttack(&game->tasks, &game->map, &game->player, actionMessage, messageSize)) {
            GamePlay_StartLaserEffect(game, attackTarget, attackTarget != NULL);
            Audio_PlayCue(&game->audio, game->player.hasLaserGun ? AUDIO_CUE_LASER : AUDIO_CUE_MELEE);
            Game_PostMessage(game, actionMessage, 2.8f);
        } else {
            Game_PostMessage(game, actionMessage, 2.2f);
        }
    }

    if (IsKeyPressed(KEY_X)) {
        if (Player_UseQuickConsumable(&game->player, actionMessage, (int)messageSize)) {
            Game_PostMessage(game, actionMessage, 2.8f);
        } else {
            Game_PostMessage(game, actionMessage, 2.4f);
        }
    }

    return false;
}
