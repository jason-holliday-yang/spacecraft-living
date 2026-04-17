#include "game_overlay_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cstdio>

static void EnsureCommunicatorSelectionVisible(Game *game, int collectedCount) {
    int visibleCount;

    if (game == NULL) {
        return;
    }

    game->communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                      GetScreenHeight(),
                                                                                      game->communicatorFirstVisibleLogIndex,
                                                                                      collectedCount);
    if (collectedCount <= 0) {
        return;
    }

    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    if (game->selectedLogIndex < game->communicatorFirstVisibleLogIndex) {
        game->communicatorFirstVisibleLogIndex = game->selectedLogIndex;
    } else if (game->selectedLogIndex >= game->communicatorFirstVisibleLogIndex + visibleCount) {
        game->communicatorFirstVisibleLogIndex = game->selectedLogIndex - visibleCount + 1;
    }

    game->communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                      GetScreenHeight(),
                                                                                      game->communicatorFirstVisibleLogIndex,
                                                                                      collectedCount);
}

static void TryCraftSelectedRecipe(Game *game) {
    char message[256];
    RecipeType recipe;

    recipe = Tasks_GetVisibleRecipeAt(&game->tasks, game->selectedCraftIndex);
    if (Tasks_TryCraft(&game->tasks, &game->map, &game->player, recipe, message, sizeof(message))) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CRAFT);
        Game_PostMessage(game, message, 3.6f);
    } else {
        Game_PostMessage(game, message, 3.0f);
    }
}

static void UpdateCraftOverlay(Game *game) {
    Vector2 mouse;
    int recipeCount;
    int recipeIndex;

    recipeCount = Tasks_GetVisibleRecipeCount(&game->tasks);
    if (recipeCount <= 0) {
        GameOverlay_TryCloseOverlay(game, &game->craftOpen, KEY_NULL);
        return;
    }

    if (game->selectedCraftIndex < 0 || game->selectedCraftIndex >= recipeCount) {
        game->selectedCraftIndex = 0;
    }

    if (GameOverlay_TryCloseOverlay(game, &game->craftOpen, KEY_NULL)) {
        return;
    }

    for (recipeIndex = 0; recipeIndex < 9; recipeIndex++) {
        KeyboardKey key;

        key = (KeyboardKey)(KEY_ONE + recipeIndex);
        if (IsKeyPressed(key) && recipeIndex < recipeCount) {
            game->selectedCraftIndex = recipeIndex;
            TryCraftSelectedRecipe(game);
            return;
        }
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        if (game->selectedCraftIndex > 0) {
            game->selectedCraftIndex--;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        if (game->selectedCraftIndex < recipeCount - 1) {
            game->selectedCraftIndex++;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (game->selectedCraftIndex - UI_CRAFT_SLOT_COLUMNS >= 0) {
            game->selectedCraftIndex -= UI_CRAFT_SLOT_COLUMNS;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (game->selectedCraftIndex + UI_CRAFT_SLOT_COLUMNS < recipeCount) {
            game->selectedCraftIndex += UI_CRAFT_SLOT_COLUMNS;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (GameOverlay_IsConfirmPressed() || IsKeyPressed(KEY_F)) {
        TryCraftSelectedRecipe(game);
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    recipeIndex = GameOverlay_FindClickedIndexedRect(mouse, recipeCount, UI_GetCraftSlotRect);
    if (recipeIndex >= 0) {
        game->selectedCraftIndex = recipeIndex;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetCraftActionButtonRect(GetScreenWidth(), GetScreenHeight()))) {
        TryCraftSelectedRecipe(game);
    }
}

static bool TryUseSelectedBackpackItem(Game *game, char *message, size_t messageSize) {
    ResourceType resource;

    switch (game->selectedBackpackItem) {
        case 3:
            resource = RESOURCE_FRUIT;
            break;
        case 4:
            resource = RESOURCE_SPECIAL_FUNGUS;
            break;
        case 6:
            resource = RESOURCE_GLOW_MOSS;
            break;
        case 8:
            resource = RESOURCE_SHELL_FRUIT;
            break;
        case 11:
            resource = RESOURCE_CALM_MUSHROOM;
            break;
        default:
            if (message != NULL && messageSize > 0) {
                message[0] = '\0';
            }
            return false;
    }

    return Player_UseSelectedConsumable(&game->player, resource, message, (int)messageSize);
}

static void UpdateBackpackOverlay(Game *game) {
    Vector2 mouse;
    int itemIndex;

    if (game->selectedBackpackItem < 0 || game->selectedBackpackItem >= BACKPACK_ENTRY_COUNT) {
        game->selectedBackpackItem = 0;
    }

    if (GameOverlay_TryCloseOverlay(game, &game->backpackOpen, KEY_B)) {
        return;
    }

    if (IsKeyPressed(KEY_F) || GameOverlay_IsConfirmPressed()) {
        char message[256];

        if (TryUseSelectedBackpackItem(game, message, sizeof(message))) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            Game_PostMessage(game, message, 2.8f);
        } else {
            if (message[0] != '\0') {
                Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
                Game_PostMessage(game, message, 2.6f);
            }
        }
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    itemIndex = GameOverlay_FindClickedIndexedRect(mouse, BACKPACK_ENTRY_COUNT, UI_GetBackpackSlotRect);
    if (itemIndex >= 0) {
        game->selectedBackpackItem = itemIndex;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }
}

static void UpdateCommunicatorOverlay(Game *game) {
    Vector2 mouse;
    int collectedCount;
    int tabIndex;
    int firstVisibleLog;
    int visibleCount;
    int drawCount;
    int visibleIndex;
    float wheelMove;

    if (GameOverlay_TryCloseOverlay(game, &game->communicatorOpen, KEY_N)) {
        return;
    }

    collectedCount = Tasks_GetCollectedLogCount(&game->tasks);
    if (game->communicatorTab < COMMUNICATOR_TAB_TASKS || game->communicatorTab >= COMMUNICATOR_TAB_COUNT) {
        game->communicatorTab = COMMUNICATOR_TAB_TASKS;
    }
    if (game->selectedLogIndex < 0) {
        game->selectedLogIndex = 0;
    }
    if (collectedCount > 0 && game->selectedLogIndex >= collectedCount) {
        game->selectedLogIndex = collectedCount - 1;
    }
    game->communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                      GetScreenHeight(),
                                                                                      game->communicatorFirstVisibleLogIndex,
                                                                                      collectedCount);

    if (IsKeyPressed(KEY_TAB)
        || IsKeyPressed(KEY_Q)
        || IsKeyPressed(KEY_E)
        || IsKeyPressed(KEY_LEFT)
        || IsKeyPressed(KEY_A)
        || IsKeyPressed(KEY_RIGHT)
        || IsKeyPressed(KEY_D)) {
        int nextTab = (int)game->communicatorTab;

        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_Q)) {
            nextTab = (nextTab + COMMUNICATOR_TAB_COUNT - 1) % COMMUNICATOR_TAB_COUNT;
        } else {
            nextTab = (nextTab + 1) % COMMUNICATOR_TAB_COUNT;
        }

        game->communicatorTab = (CommunicatorTab)nextTab;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (game->communicatorTab != COMMUNICATOR_TAB_LOGS) {
        if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
            tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
            if (tabIndex >= 0) {
                game->communicatorTab = (CommunicatorTab)tabIndex;
                Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            }
        }
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (game->selectedLogIndex > 0) {
            game->selectedLogIndex--;
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (game->selectedLogIndex + 1 < collectedCount) {
            game->selectedLogIndex++;
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    wheelMove = GetMouseWheelMove();
    if (wheelMove != 0.0f) {
        Vector2 mousePosition = GetMousePosition();

        if (CheckCollisionPointRec(mousePosition, UI_GetCommunicatorLogListRect(GetScreenWidth(), GetScreenHeight()))) {
            int previousFirstVisibleLogIndex = game->communicatorFirstVisibleLogIndex;

            if (wheelMove > 0.0f) {
                game->communicatorFirstVisibleLogIndex--;
            } else if (wheelMove < 0.0f) {
                game->communicatorFirstVisibleLogIndex++;
            }

            game->communicatorFirstVisibleLogIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                              GetScreenHeight(),
                                                                                              game->communicatorFirstVisibleLogIndex,
                                                                                              collectedCount);
            if (game->communicatorFirstVisibleLogIndex != previousFirstVisibleLogIndex) {
                Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            }
            return;
        }
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
    if (tabIndex >= 0) {
        game->communicatorTab = (CommunicatorTab)tabIndex;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (collectedCount <= 0) {
        return;
    }

    firstVisibleLog = game->communicatorFirstVisibleLogIndex;
    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    drawCount = collectedCount - firstVisibleLog;
    if (drawCount > visibleCount) {
        drawCount = visibleCount;
    }

    for (visibleIndex = 0; visibleIndex < drawCount; ++visibleIndex) {
        if (CheckCollisionPointRec(mouse, UI_GetCommunicatorVisibleLogEntryRect(GetScreenWidth(), GetScreenHeight(), visibleIndex))) {
            game->selectedLogIndex = firstVisibleLog + visibleIndex;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            return;
        }
    }
}

static void UpdateHelpOverlay(Game *game) {
    GameOverlay_TryCloseOverlay(game, &game->helpOpen, KEY_H);
}

static void UpdateMapOverlay(Game *game) {
    GameOverlay_TryCloseOverlay(game, &game->mapOpen, KEY_M);
}

static void UpdateDeathPopup(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    bool activateSelection;
    const bool canLoad = game != NULL && game->hasSaveFile;

    activateSelection = false;
    if (!canLoad) {
        game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
        activateSelection = true;
    }

    if (canLoad && GameOverlay_IsBackwardNavigationPressed()) {
        game->deathPopupSelection = (game->deathPopupSelection + DEATH_POPUP_BUTTON_COUNT - 1) % DEATH_POPUP_BUTTON_COUNT;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (canLoad && GameOverlay_IsForwardNavigationPressed()) {
        game->deathPopupSelection = (game->deathPopupSelection + 1) % DEATH_POPUP_BUTTON_COUNT;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = GameOverlay_FindClickedIndexedRect(mouse, DEATH_POPUP_BUTTON_COUNT, UI_GetDeathPopupButtonRect);
        if (buttonIndex >= 0) {
            game->deathPopupSelection = buttonIndex;
            activateSelection = true;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        activateSelection = true;
    }

    if (!activateSelection) {
        return;
    }

    if (game->deathPopupSelection == DEATH_POPUP_BUTTON_RESTART) {
        Game_HandleDeathRecovery(game);
    } else if (canLoad) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
    }
}

bool Game_UpdateGameplayOverlayState(Game *game) {
    if (game->showDeathPopup) {
        UpdateDeathPopup(game);
        return true;
    }

    if (game->craftOpen) {
        UpdateCraftOverlay(game);
        return true;
    }

    if (game->mapOpen) {
        UpdateMapOverlay(game);
        return true;
    }

    if (game->backpackOpen) {
        UpdateBackpackOverlay(game);
        return true;
    }

    if (game->communicatorOpen) {
        UpdateCommunicatorOverlay(game);
        return true;
    }

    if (game->helpOpen) {
        UpdateHelpOverlay(game);
        return true;
    }

    return false;
}
