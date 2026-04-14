#include "game_overlay_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cstdio>

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
            std::snprintf(message, messageSize, "%s", Loc_PickLiteral("The selected item cannot be used directly from the backpack.", "当前选中的物品无法直接从背包使用。"));
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
            Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
            Game_PostMessage(game, message, 2.6f);
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
    GameOverlay_TryCloseOverlay(game, &game->communicatorOpen, KEY_N);
}

static void UpdateHelpOverlay(Game *game) {
    GameOverlay_TryCloseOverlay(game, &game->helpOpen, KEY_H);
}

static void UpdateLogReader(Game *game) {
    int collectedCount;
    int entryIndex;

    collectedCount = Tasks_GetCollectedLogCount(&game->tasks);
    if (collectedCount <= 0) {
        game->logReaderOpen = false;
        return;
    }

    if (game->selectedLogIndex < 0) {
        game->selectedLogIndex = 0;
    }
    if (game->selectedLogIndex >= collectedCount) {
        game->selectedLogIndex = collectedCount - 1;
    }

    if (GameOverlay_TryCloseOverlay(game, &game->logReaderOpen, KEY_L)) {
        return;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        if (game->selectedLogIndex > 0) {
            game->selectedLogIndex--;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
        if (game->selectedLogIndex < collectedCount - 1) {
            game->selectedLogIndex++;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    entryIndex = GameOverlay_FindClickedIndexedRect(GetMousePosition(), collectedCount, UI_GetLogEntryRect);
    if (entryIndex >= 0) {
        game->selectedLogIndex = entryIndex;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    }
}

static void UpdateMapOverlay(Game *game) {
    GameOverlay_TryCloseOverlay(game, &game->mapOpen, KEY_M);
}

static void UpdateDeathPopup(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    bool activateSelection;

    activateSelection = false;

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->deathPopupSelection = DEATH_POPUP_BUTTON_EXIT;
        activateSelection = true;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        game->deathPopupSelection = (game->deathPopupSelection + DEATH_POPUP_BUTTON_COUNT - 1) % DEATH_POPUP_BUTTON_COUNT;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
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
    } else {
        game->requestClose = true;
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

    if (game->logReaderOpen) {
        UpdateLogReader(game);
        return true;
    }

    return false;
}
