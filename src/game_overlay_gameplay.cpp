#include "game_overlay_internal.h"

#include "localization.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cstdio>

static float ClampUnit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

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

static int CountShownMainStoryScenes(const Game *game) {
    int count = 0;

    if (game == NULL) {
        return 0;
    }

    for (int scene = STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY;
         scene < STORY_SCENE_MAIN_AIR_FOR_ONE_MORE_DAY + STORY_MAIN_SCENE_COUNT;
         ++scene) {
        if (game->storySceneShown[scene]) {
            count++;
        }
    }

    return count;
}

static void EnsureCommunicatorStorySelectionVisible(Game *game, int shownCount) {
    int visibleCount;

    if (game == NULL) {
        return;
    }

    game->communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                             GetScreenHeight(),
                                                                                             game->communicatorFirstVisibleStorySceneIndex,
                                                                                             shownCount);
    if (shownCount <= 0) {
        return;
    }

    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    if (game->selectedStorySceneIndex < game->communicatorFirstVisibleStorySceneIndex) {
        game->communicatorFirstVisibleStorySceneIndex = game->selectedStorySceneIndex;
    } else if (game->selectedStorySceneIndex >= game->communicatorFirstVisibleStorySceneIndex + visibleCount) {
        game->communicatorFirstVisibleStorySceneIndex = game->selectedStorySceneIndex - visibleCount + 1;
    }

    game->communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                             GetScreenHeight(),
                                                                                             game->communicatorFirstVisibleStorySceneIndex,
                                                                                             shownCount);
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
    static constexpr float kCommunicatorDetailFadeDuration = 0.16f;
    Vector2 mouse;
    int collectedCount;
    int shownStoryCount;
    int tabIndex;
    int firstVisibleLog;
    int visibleCount;
    int drawCount;
    int visibleIndex;
    float wheelMove;
    float detailFadeStep;
    bool detailVisible;

    detailFadeStep = GetFrameTime() / kCommunicatorDetailFadeDuration;
    game->communicatorLogDetailVisibility = ClampUnit(
        game->communicatorLogDetailVisibility + (game->communicatorLogDetailOpen ? detailFadeStep : -detailFadeStep));
    detailVisible = game->communicatorLogDetailOpen || game->communicatorLogDetailVisibility > 0.001f;

    if (detailVisible && IsKeyPressed(KEY_ESCAPE)) {
        game->communicatorLogDetailOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_TryCloseOverlay(game, &game->communicatorOpen, KEY_N)) {
        game->communicatorLogDetailOpen = false;
        game->communicatorLogDetailVisibility = 0.0f;
        return;
    }

    collectedCount = Tasks_GetCollectedLogCount(&game->tasks);
    shownStoryCount = CountShownMainStoryScenes(game);
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
    if (game->selectedStorySceneIndex < 0) {
        game->selectedStorySceneIndex = 0;
    }
    if (shownStoryCount > 0 && game->selectedStorySceneIndex >= shownStoryCount) {
        game->selectedStorySceneIndex = shownStoryCount - 1;
    }
    game->communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                             GetScreenHeight(),
                                                                                             game->communicatorFirstVisibleStorySceneIndex,
                                                                                             shownStoryCount);

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
        game->communicatorLogDetailOpen = false;
        game->communicatorLogDetailVisibility = 0.0f;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (game->communicatorTab == COMMUNICATOR_TAB_TASKS) {
        if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
            tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
            if (tabIndex >= 0) {
                game->communicatorTab = (CommunicatorTab)tabIndex;
                game->communicatorLogDetailOpen = false;
                game->communicatorLogDetailVisibility = 0.0f;
                Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            }
        }
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (game->communicatorTab == COMMUNICATOR_TAB_LOGS && game->selectedLogIndex > 0) {
            game->selectedLogIndex--;
            game->communicatorLogDetailOpen = false;
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        } else if (game->communicatorTab == COMMUNICATOR_TAB_STORY && game->selectedStorySceneIndex > 0) {
            game->selectedStorySceneIndex--;
            game->communicatorLogDetailOpen = false;
            EnsureCommunicatorStorySelectionVisible(game, shownStoryCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (game->communicatorTab == COMMUNICATOR_TAB_LOGS && game->selectedLogIndex + 1 < collectedCount) {
            game->selectedLogIndex++;
            game->communicatorLogDetailOpen = false;
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        } else if (game->communicatorTab == COMMUNICATOR_TAB_STORY && game->selectedStorySceneIndex + 1 < shownStoryCount) {
            game->selectedStorySceneIndex++;
            game->communicatorLogDetailOpen = false;
            EnsureCommunicatorStorySelectionVisible(game, shownStoryCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    wheelMove = GetMouseWheelMove();
    if (wheelMove != 0.0f) {
        Vector2 mousePosition = GetMousePosition();

        if (CheckCollisionPointRec(mousePosition, UI_GetCommunicatorLogListRect(GetScreenWidth(), GetScreenHeight()))) {
            if (game->communicatorTab == COMMUNICATOR_TAB_LOGS) {
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

            if (game->communicatorTab == COMMUNICATOR_TAB_STORY) {
                int previousFirstVisibleStoryIndex = game->communicatorFirstVisibleStorySceneIndex;

                if (wheelMove > 0.0f) {
                    game->communicatorFirstVisibleStorySceneIndex--;
                } else if (wheelMove < 0.0f) {
                    game->communicatorFirstVisibleStorySceneIndex++;
                }

                game->communicatorFirstVisibleStorySceneIndex = UI_ClampCommunicatorFirstVisibleLogIndex(GetScreenWidth(),
                                                                                                         GetScreenHeight(),
                                                                                                         game->communicatorFirstVisibleStorySceneIndex,
                                                                                                         shownStoryCount);
                if (game->communicatorFirstVisibleStorySceneIndex != previousFirstVisibleStoryIndex) {
                    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
                }
                return;
            }
        }
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
    if (tabIndex >= 0) {
        game->communicatorTab = (CommunicatorTab)tabIndex;
        game->communicatorLogDetailOpen = false;
        game->communicatorLogDetailVisibility = 0.0f;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (game->communicatorTab == COMMUNICATOR_TAB_LOGS && collectedCount <= 0) {
        return;
    }
    if (game->communicatorTab == COMMUNICATOR_TAB_STORY && shownStoryCount <= 0) {
        return;
    }

    if (detailVisible) {
        game->communicatorLogDetailOpen = false;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetCommunicatorLogImageRect(GetScreenWidth(), GetScreenHeight()))) {
        game->communicatorLogDetailOpen = true;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }

    firstVisibleLog = game->communicatorTab == COMMUNICATOR_TAB_STORY
        ? game->communicatorFirstVisibleStorySceneIndex
        : game->communicatorFirstVisibleLogIndex;
    visibleCount = UI_GetCommunicatorVisibleLogCount(GetScreenWidth(), GetScreenHeight());
    drawCount = (game->communicatorTab == COMMUNICATOR_TAB_STORY ? shownStoryCount : collectedCount) - firstVisibleLog;
    if (drawCount > visibleCount) {
        drawCount = visibleCount;
    }

    for (visibleIndex = 0; visibleIndex < drawCount; ++visibleIndex) {
        if (CheckCollisionPointRec(mouse, UI_GetCommunicatorVisibleLogEntryRect(GetScreenWidth(), GetScreenHeight(), visibleIndex))) {
            if (game->communicatorTab == COMMUNICATOR_TAB_STORY) {
                game->selectedStorySceneIndex = firstVisibleLog + visibleIndex;
            } else {
                game->selectedLogIndex = firstVisibleLog + visibleIndex;
            }
            game->communicatorLogDetailOpen = false;
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
        if (game->deathPopupSelection == DEATH_POPUP_BUTTON_LOAD) {
            game->deathPopupSelection = DEATH_POPUP_BUTTON_RESTART;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->deathPopupSelection = DEATH_POPUP_BUTTON_MENU;
        activateSelection = true;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        do {
            game->deathPopupSelection = (game->deathPopupSelection + DEATH_POPUP_BUTTON_COUNT - 1) % DEATH_POPUP_BUTTON_COUNT;
        } while (!canLoad && game->deathPopupSelection == DEATH_POPUP_BUTTON_LOAD);
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
        do {
            game->deathPopupSelection = (game->deathPopupSelection + 1) % DEATH_POPUP_BUTTON_COUNT;
        } while (!canLoad && game->deathPopupSelection == DEATH_POPUP_BUTTON_LOAD);
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
    } else if (game->deathPopupSelection == DEATH_POPUP_BUTTON_LOAD && canLoad) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
    } else if (game->deathPopupSelection == DEATH_POPUP_BUTTON_MENU) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        Game_BeginScreenTransition(game, SCREEN_TRANSITION_RETURN_TO_MENU, -1);
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
