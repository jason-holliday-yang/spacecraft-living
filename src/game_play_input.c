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

static bool SameDirection(int ax, int ay, int bx, int by) {
    return ax == bx && ay == by;
}

static const char *GetCrouchToggleMessage(const Game *game) {
    MapArea area;

    area = Map_GetAreaAt(game->player.gridX, game->player.gridY);
    if (game->player.crouching) {
        if (area == MAP_AREA_FOREST) {
            return Loc_PickLiteral("Stealth crouch enabled. Forest cover now reduces monster detection.",
                                   "蹲伏潜行已开启。森林掩护现在会降低怪物的发现概率。");
        }
        return Loc_PickLiteral("Stealth crouch enabled. It slows movement, but forest cover is where stealth matters most.",
                               "蹲伏潜行已开启。移动会变慢，但真正能发挥潜行效果的还是森林掩护。");
    }

    if (area == MAP_AREA_FOREST) {
        return Loc_PickLiteral("Stealth crouch disabled. You are easier to notice in the forest now.",
                               "蹲伏潜行已关闭。你现在在森林里会更容易被发现。");
    }
    return Loc_PickLiteral("Stealth crouch disabled.", "蹲伏潜行已关闭。");
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

static AudioCue SelectInteractionCue(const char *message) {
    if (MessageStartsWith(message, "Collected ") || MessageStartsWith(message, "Collected")
        || MessageStartsWith(message, "获得了") || MessageStartsWith(message, "收集了")) {
        return AUDIO_CUE_COLLECT;
    }
    if (MessageStartsWith(message, "Recovered ") || MessageStartsWith(message, "Recovered")
        || MessageStartsWith(message, "Recovered log")
        || strstr(message, "已回收") != NULL
        || strstr(message, "日志") != NULL) {
        return AUDIO_CUE_LOG;
    }
    if (strstr(message, "monolith") != NULL || strstr(message, "Monolith") != NULL
        || strstr(message, "石碑") != NULL) {
        return AUDIO_CUE_MONOLITH;
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
    return game != NULL
        && Tasks_CanChooseSettlement(&game->tasks)
        && IsNearLoxiTerminal(game);
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
    int pressedX;
    int pressedY;
    int heldX;
    int heldY;
    bool hasPressedInput;
    bool hasHeldInput;

    pressedX = 0;
    pressedY = 0;
    heldX = 0;
    heldY = 0;
    hasPressedInput = GetPressedDirection(&pressedX, &pressedY);
    hasHeldInput = GetHeldDirection(&heldX, &heldY);

    if (hasPressedInput) {
        game->bufferedMoveX = pressedX;
        game->bufferedMoveY = pressedY;
        game->inputBufferTimer = INPUT_BUFFER_TIME;
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
        game->mapOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
    }

    rect = UI_GetHudShortcutRect(GetScreenWidth(), GetScreenHeight(), 1);
    if (CheckCollisionPointRec(mouse, rect)) {
        game->backpackOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
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
        game->backpackOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
    }
    if (IsKeyPressed(KEY_N)) {
        if (Tasks_IsCommunicatorUnlocked(&game->tasks)) {
            game->communicatorOpen = true;
            game->communicatorTab = COMMUNICATOR_TAB_TASKS;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        } else {
            Game_PostMessage(game, Loc_PickLiteral("Loxi link is offline. Sync with the terminal bay uplink first, then press N.",
                                                   "洛希链路尚未上线。请先在终端舱完成同步，再按 N。"),
                             2.8f);
        }
        return true;
    }
    if (IsKeyPressed(KEY_H)) {
        game->helpOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
    }
    if (IsKeyPressed(KEY_M)) {
        game->mapOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return true;
    }
    if (TryHandleHudShortcutClick(game)) {
        return true;
    }
    if (IsKeyPressed(KEY_C)) {
        game->player.crouching = !game->player.crouching;
        Game_PostMessage(game, GetCrouchToggleMessage(game), 2.4f);
    }

    if (IsKeyPressed(KEY_F)) {
        GameEnding endingBeforeInteraction;
        bool shouldStartSleepTransition;
        bool shouldPrioritizePickup;
        TaskInteractionTarget preferredTarget;

        shouldPrioritizePickup = TasksRuntime_HasNearbyPickupPriority(&game->tasks, &game->player);
        preferredTarget = TasksRuntime_GetPreferredInteractionTarget(&game->player);
        shouldStartSleepTransition = IsCrewQuartersInteraction(game, preferredTarget);

        if (preferredTarget == TASK_INTERACTION_LOXI_TERMINAL
            && ShouldOpenSettlementConfirm(game)) {
            game->settlementConfirmOpen = true;
            game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_PEACEFUL;
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
                Audio_PlayCue(&game->audio, SelectInteractionCue(actionMessage));
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
        if (Tasks_HandleAttack(&game->tasks, &game->map, &game->player, actionMessage, messageSize)) {
            Audio_PlayCue(&game->audio, game->player.hasLaserGun ? AUDIO_CUE_LASER : AUDIO_CUE_MELEE);
            Game_PostMessage(game, actionMessage, 2.8f);
        } else {
            Game_PostMessage(game, actionMessage, 2.2f);
        }
    }

    if (IsKeyPressed(KEY_Z)) {
        if (Player_UseQuickConsumable(&game->player, CONSUMABLE_FOOD, actionMessage, (int)messageSize)) {
            Game_PostMessage(game, actionMessage, 2.8f);
        } else {
            Game_PostMessage(game, actionMessage, 2.4f);
        }
    }

    if (IsKeyPressed(KEY_X)) {
        if (Player_UseQuickConsumable(&game->player, CONSUMABLE_CALM, actionMessage, (int)messageSize)) {
            Game_PostMessage(game, actionMessage, 2.8f);
        } else {
            Game_PostMessage(game, actionMessage, 2.4f);
        }
    }

    return false;
}
