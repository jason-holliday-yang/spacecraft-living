#include "game_overlay_gameplay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

static void TryCraftSelectedRecipe(Game *game) {
    char message[256];
    RecipeType recipe;

    recipe = Tasks_GetVisibleRecipeAt(&game->runtime.tasks, game->ui.selectedCraftIndex);
    if (Tasks_TryCraft(&game->runtime.tasks, &game->runtime.map, &game->runtime.player, recipe, message, sizeof(message))) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CRAFT);
        Game_PostMessage(game, message, 3.6f);
    } else {
        Game_PostMessage(game, message, 3.0f);
    }
}

void GameOverlay_UpdateCraft(Game *game) {
    Vector2 mouse;
    int recipeCount;
    int recipeIndex;

    recipeCount = Tasks_GetVisibleRecipeCount(&game->runtime.tasks);
    if (recipeCount <= 0) {
        GameOverlay_TryCloseOverlay(game, &game->ui.craftOpen, KEY_NULL);
        return;
    }

    if (game->ui.selectedCraftIndex < 0 || game->ui.selectedCraftIndex >= recipeCount) {
        game->ui.selectedCraftIndex = 0;
    }

    if (GameOverlay_TryCloseOverlay(game, &game->ui.craftOpen, KEY_NULL)) {
        return;
    }

    for (recipeIndex = 0; recipeIndex < 9; recipeIndex++) {
        KeyboardKey key;

        key = (KeyboardKey)(KEY_ONE + recipeIndex);
        if (IsKeyPressed(key) && recipeIndex < recipeCount) {
            game->ui.selectedCraftIndex = recipeIndex;
            TryCraftSelectedRecipe(game);
            return;
        }
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        if (game->ui.selectedCraftIndex > 0) {
            game->ui.selectedCraftIndex--;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        if (game->ui.selectedCraftIndex < recipeCount - 1) {
            game->ui.selectedCraftIndex++;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (game->ui.selectedCraftIndex - UI_CRAFT_SLOT_COLUMNS >= 0) {
            game->ui.selectedCraftIndex -= UI_CRAFT_SLOT_COLUMNS;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (game->ui.selectedCraftIndex + UI_CRAFT_SLOT_COLUMNS < recipeCount) {
            game->ui.selectedCraftIndex += UI_CRAFT_SLOT_COLUMNS;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
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
        game->ui.selectedCraftIndex = recipeIndex;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetCraftActionButtonRect(GetScreenWidth(), GetScreenHeight()))) {
        TryCraftSelectedRecipe(game);
    }
}
