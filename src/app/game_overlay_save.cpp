#include "game_overlay_frontend_internal.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

void GameOverlay_UpdateSavePanel(Game *game) {
    Vector2 mouse;
    int slotIndex;

    if (GameOverlay_TryCloseOverlay(game, &game->ui.savePanelOpen, KEY_NULL)) {
        return;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        Game_MoveSavePanelSelection(game, -1);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        Game_MoveSavePanelSelection(game, 1);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        Game_MoveSavePanelSelection(game, -4);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        Game_MoveSavePanelSelection(game, 4);
    }

    if (GameOverlay_IsConfirmPressed()) {
        if (game->ui.savePanelMode == SAVE_PANEL_MODE_LOAD
            && game->ui.selectedSaveSlot >= 0
            && game->ui.selectedSaveSlot < SAVE_SLOT_COUNT
            && game->account.saveSlots[game->ui.selectedSaveSlot].exists) {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_LOAD_GAME, game->ui.selectedSaveSlot);
            return;
        }
        Game_ActivateSelectedSaveSlot(game);
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    slotIndex = GameOverlay_FindClickedIndexedRect(mouse, SAVE_SLOT_COUNT, UI_GetSaveSlotRect);
    if (slotIndex >= 0) {
        if (game->ui.selectedSaveSlot != slotIndex) {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
        }
        game->ui.selectedSaveSlot = slotIndex;
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetSavePrimaryButtonRect(GetScreenWidth(), GetScreenHeight()))) {
        if (game->ui.savePanelMode == SAVE_PANEL_MODE_LOAD
            && game->ui.selectedSaveSlot >= 0
            && game->ui.selectedSaveSlot < SAVE_SLOT_COUNT
            && game->account.saveSlots[game->ui.selectedSaveSlot].exists) {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_LOAD_GAME, game->ui.selectedSaveSlot);
            return;
        }
        Game_ActivateSelectedSaveSlot(game);
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetSaveDeleteButtonRect(GetScreenWidth(), GetScreenHeight()))) {
        Game_DeleteSelectedSaveSlot(game);
    }
}
