#ifndef GAME_OVERLAY_INTERNAL_H
#define GAME_OVERLAY_INTERNAL_H

#include "game_manager_internal.h"

/* Internal overlay-flow helpers shared by frontend/session overlays and in-game panels. */

typedef Rectangle (*GameOverlayIndexedRectGetter)(int screenWidth, int screenHeight, int itemIndex);

static inline bool GameOverlay_IsConfirmPressed(void) {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
}

static inline bool GameOverlay_IsCutsceneAdvancePressed(void) {
    return GameOverlay_IsConfirmPressed() || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

static inline bool GameOverlay_IsBackwardNavigationPressed(void) {
    return IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
}

static inline bool GameOverlay_IsForwardNavigationPressed(void) {
    return IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
}

static inline bool GameOverlay_TryCloseOverlay(Game *game, bool *open, KeyboardKey alternateKey) {
    if (open == NULL) {
        return false;
    }

    if (!IsKeyPressed(KEY_ESCAPE) && (alternateKey == KEY_NULL || !IsKeyPressed(alternateKey))) {
        return false;
    }

    *open = false;
    Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
    return true;
}

static inline bool GameOverlay_TryGetPrimaryClickPosition(Vector2 *position) {
    if (position == NULL) {
        return false;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *position = GetMousePosition();
        return true;
    }

    return false;
}

static inline int GameOverlay_FindClickedIndexedRect(Vector2 mouse, int count, GameOverlayIndexedRectGetter getRect) {
    int index;

    for (index = 0; index < count; ++index) {
        if (CheckCollisionPointRec(mouse, getRect(GetScreenWidth(), GetScreenHeight(), index))) {
            return index;
        }
    }

    return -1;
}

SCL_EXTERN_C_BEGIN

bool Game_UpdateFrontEndOverlayState(Game *game);
bool Game_UpdateGameplayOverlayState(Game *game);

SCL_EXTERN_C_END

#endif
