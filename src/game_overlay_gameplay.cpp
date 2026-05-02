#include "game_overlay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
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

static void ResetCommunicatorDetailState(Game *game) {
    if (game == NULL) {
        return;
    }

    game->communicatorLogDetailOpen = false;
    game->communicatorLogDetailVisibility = 0.0f;
    game->communicatorLogDetailScroll = 0.0f;
}

static void ScrollCommunicatorDetail(Game *game, float delta) {
    if (game == NULL) {
        return;
    }

    game->communicatorLogDetailScroll += delta;
    if (game->communicatorLogDetailScroll < 0.0f) {
        game->communicatorLogDetailScroll = 0.0f;
    }
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

static float ClampSettingsValue(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void ApplySettingsInfoSliderValue(Game *game, int sliderIndex, float value) {
    float clampedValue;

    if (game == NULL) {
        return;
    }

    clampedValue = ClampSettingsValue(value, 0.0f, 1.0f);
    switch (sliderIndex) {
        case 0:
            game->settings.masterVolume = clampedValue;
            game->settingsDirty = true;
            Audio_SetMasterVolumeSetting(&game->audio, clampedValue);
            return;
        case 1:
            game->settings.musicVolume = clampedValue;
            game->settingsDirty = true;
            Audio_SetMusicVolumeSetting(&game->audio, clampedValue);
            return;
        case 2:
            game->settings.sfxVolume = clampedValue;
            game->settings.sfxEnabled = clampedValue > 0.001f;
            game->settingsDirty = true;
            Audio_SetSfxVolumeSetting(&game->audio, clampedValue);
            Audio_SetSfxEnabled(&game->audio, game->settings.sfxEnabled);
            return;
        default:
            return;
    }
}

static float GetSettingsInfoSliderValue(const Game *game, int sliderIndex) {
    if (game == NULL) {
        return 0.0f;
    }

    switch (sliderIndex) {
        case 0:
            return game->settings.masterVolume;
        case 1:
            return game->settings.musicVolume;
        case 2:
            return game->settings.sfxVolume;
        default:
            return 0.0f;
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
    const BackpackEntryDef *entry;
    ResourceType resource;

    entry = UIInventory_GetBackpackEntry(game->selectedBackpackItem);
    if (entry == NULL) {
        if (message != NULL && messageSize > 0) {
            message[0] = '\0';
        }
        return false;
    }

    if (entry->kind == BACKPACK_ENTRY_GEAR) {
        const BackpackGearId gearId = (BackpackGearId)entry->itemId;

        if (!UIInventory_BackpackEntryIsOwned(&game->player, game->selectedBackpackItem)) {
            std::snprintf(message, messageSize, "%s", Loc_PickLiteral("That gear is not built yet.", "这件装备还没有制作出来。"));
            return false;
        }

        switch (gearId) {
            case BACKPACK_GEAR_ROPE: {
                const int targetX = game->player.gridX + game->player.facingX;
                const int targetY = game->player.gridY + game->player.facingY;

                if (Map_CanCrossWithRope(&game->map, game->player.gridX, game->player.gridY, targetX, targetY)) {
                    Map_CreateRopeBridge(&game->map, targetX, targetY);
                    std::snprintf(message,
                                  messageSize,
                                  "%s",
                                  Loc_PickLiteral("Rope deployed. The hazard is now tied into a usable shortcut.",
                                                  "绳索已部署。眼前的险地已经被固定成可用捷径。"));
                    return true;
                }

                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Rope readied. Face a marked swamp or deep barrier, then press F or use it here.",
                                              "绳索已准备好。面对标记出的沼泽或深水阻隔后，按 F 或在背包中使用它。"));
                return true;
            }
            case BACKPACK_GEAR_LASER_GUN:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Laser Gun is already equipped. Face a threat and press Space to fire.",
                                              "激光枪已经装备。面向威胁后按空格键发射。"));
                return false;
            case BACKPACK_GEAR_PROTECTION_SUIT:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Protection Suit works passively while carried. There is no manual backpack action for it.",
                                              "防护服在携带时被动生效，不需要在背包里手动使用。"));
                return false;
            case BACKPACK_GEAR_FIELD_CAMP:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Field Camp is ready. Stand beside the placed camp and press F to rest there.",
                                              "野外营地已就绪。站在已部署营地旁按 F 即可休整。"));
                return false;
            case BACKPACK_GEAR_SIGNAL_AMPLIFIER:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Signal Amplifier packed. Carry it to the tower route when you commit to the calmer ending.",
                                              "信号放大器已携带。选择更平稳的终局路线时，把它带到塔楼。"));
                return false;
            case BACKPACK_GEAR_GLOW_STICK:
            default:
                game->player.glowStickTimer = fmaxf(game->player.glowStickTimer, 60.0f);
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Glow Stick readied. Its light will cover the next dark stretch.",
                                              "荧光棒已准备好。它的光会撑过接下来那段黑暗路程。"));
                return true;
        }
    }

    switch (entry->itemId) {
        case RESOURCE_RECOVERY_RATION:
            resource = (ResourceType)entry->itemId;
            break;
        default:
            if (message != NULL && messageSize > 0) {
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("That raw material has to be processed at the workbench before it becomes usable.",
                                              "这种原始素材必须先在工作台加工，之后才能使用。"));
            }
            return false;
    }

    return Player_UseSelectedConsumable(&game->player, resource, message, (int)messageSize);
}

static InfoOverlayTab GetResolvedInfoOverlayTab(const Game *game) {
    if (game == NULL) {
        return INFO_OVERLAY_TAB_MAP;
    }

    if (game->infoOverlayTab == INFO_OVERLAY_TAB_LOXI && !Tasks_IsCommunicatorUnlocked(&game->tasks)) {
        return INFO_OVERLAY_TAB_MAP;
    }
    if (game->infoOverlayTab < INFO_OVERLAY_TAB_MAP || game->infoOverlayTab >= INFO_OVERLAY_TAB_COUNT) {
        return INFO_OVERLAY_TAB_MAP;
    }
    return game->infoOverlayTab;
}

static void CloseInfoOverlay(Game *game) {
    if (game == NULL) {
        return;
    }

    if (game->settingsDirty) {
        Game_TrySaveSettings(game);
    }
    game->infoOverlayOpen = false;
    game->settingsOpen = false;
    game->settingsSliderDragging = false;
    game->settingsSliderDragIndex = -1;
    ResetCommunicatorDetailState(game);
    Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
}

static bool TrySwitchInfoOverlayTab(Game *game, InfoOverlayTab tab, bool closeIfSelected) {
    const InfoOverlayTab currentTab = GetResolvedInfoOverlayTab(game);

    if (game == NULL) {
        return false;
    }

    if (tab == INFO_OVERLAY_TAB_LOXI && !Tasks_IsCommunicatorUnlocked(&game->tasks)) {
        Game_PostMessage(game,
                         Loc_PickLiteral("Loxi link is offline. Sync with the terminal bay uplink first, then press N.",
                                         "洛希链路尚未上线。请先在终端舱完成同步，再按 N。"),
                         2.8f);
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
        return true;
    }

    if (currentTab == tab) {
        if (closeIfSelected) {
            CloseInfoOverlay(game);
            return true;
        }
        return false;
    }

    if (currentTab == INFO_OVERLAY_TAB_SETTINGS && game->settingsDirty) {
        Game_TrySaveSettings(game);
    }
    game->infoOverlayOpen = true;
    game->infoOverlayTab = tab;
    game->settingsOpen = false;
    game->settingsSliderDragging = false;
    game->settingsSliderDragIndex = -1;
    ResetCommunicatorDetailState(game);
    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    return true;
}

static void UpdateBackpackOverlay(Game *game, bool *open, KeyboardKey alternateKey) {
    Vector2 mouse;
    int itemIndex;

    if (game->selectedBackpackItem < 0 || game->selectedBackpackItem >= BACKPACK_ENTRY_COUNT) {
        game->selectedBackpackItem = 0;
    }

    if (GameOverlay_TryCloseOverlay(game, open, alternateKey)) {
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

static void UpdateCommunicatorOverlay(Game *game, bool *open, KeyboardKey alternateKey) {
    static constexpr float kCommunicatorDetailFadeDuration = 0.16f;
    Vector2 mouse;
    Rectangle contentPanel;
    int collectedCount;
    int shownStoryCount;
    int tabIndex;
    int firstVisibleLog;
    int visibleCount;
    int drawCount;
    int visibleIndex;
    float wheelMove;
    float detailFadeStep;
    float detailScrollStep;
    bool detailVisible;

    detailFadeStep = GetFrameTime() / kCommunicatorDetailFadeDuration;
    detailScrollStep = 80.0f * UIRuntime_GetScale(GetScreenWidth(), GetScreenHeight());
    contentPanel = UI_GetCommunicatorLogContentRect(GetScreenWidth(), GetScreenHeight());
    game->communicatorLogDetailVisibility = ClampUnit(
        game->communicatorLogDetailVisibility + (game->communicatorLogDetailOpen ? detailFadeStep : -detailFadeStep));
    detailVisible = game->communicatorLogDetailOpen || game->communicatorLogDetailVisibility > 0.001f;

    if (detailVisible && (IsKeyPressed(KEY_ESCAPE) || GameOverlay_IsConfirmPressed())) {
        ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_TryCloseOverlay(game, open, alternateKey)) {
        ResetCommunicatorDetailState(game);
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
        || IsKeyPressed(KEY_LEFT)
        || IsKeyPressed(KEY_A)
        || IsKeyPressed(KEY_RIGHT)
        || IsKeyPressed(KEY_D)) {
        int nextTab = (int)game->communicatorTab;

        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            nextTab = (nextTab + COMMUNICATOR_TAB_COUNT - 1) % COMMUNICATOR_TAB_COUNT;
        } else {
            nextTab = (nextTab + 1) % COMMUNICATOR_TAB_COUNT;
        }

        game->communicatorTab = (CommunicatorTab)nextTab;
        ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        return;
    }

    if (game->communicatorTab == COMMUNICATOR_TAB_TASKS) {
        if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
            tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
            if (tabIndex >= 0) {
                game->communicatorTab = (CommunicatorTab)tabIndex;
                ResetCommunicatorDetailState(game);
                Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            }
        }
        return;
    }

    if (detailVisible) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            ScrollCommunicatorDetail(game, -detailScrollStep);
            return;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_SPACE)) {
            ScrollCommunicatorDetail(game, detailScrollStep);
            return;
        }
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (game->communicatorTab == COMMUNICATOR_TAB_LOGS && game->selectedLogIndex > 0) {
            game->selectedLogIndex--;
            ResetCommunicatorDetailState(game);
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        } else if (game->communicatorTab == COMMUNICATOR_TAB_STORY && game->selectedStorySceneIndex > 0) {
            game->selectedStorySceneIndex--;
            ResetCommunicatorDetailState(game);
            EnsureCommunicatorStorySelectionVisible(game, shownStoryCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (game->communicatorTab == COMMUNICATOR_TAB_LOGS && game->selectedLogIndex + 1 < collectedCount) {
            game->selectedLogIndex++;
            ResetCommunicatorDetailState(game);
            EnsureCommunicatorSelectionVisible(game, collectedCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        } else if (game->communicatorTab == COMMUNICATOR_TAB_STORY && game->selectedStorySceneIndex + 1 < shownStoryCount) {
            game->selectedStorySceneIndex++;
            ResetCommunicatorDetailState(game);
            EnsureCommunicatorStorySelectionVisible(game, shownStoryCount);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        }
        return;
    }

    wheelMove = GetMouseWheelMove();
    if (wheelMove != 0.0f) {
        Vector2 mousePosition = GetMousePosition();

        if (detailVisible && CheckCollisionPointRec(mousePosition, contentPanel)) {
            ScrollCommunicatorDetail(game, wheelMove > 0.0f ? -detailScrollStep : detailScrollStep);
            return;
        }

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

    if ((GameOverlay_IsConfirmPressed() || IsKeyPressed(KEY_SPACE)) && !detailVisible) {
        game->communicatorLogDetailOpen = true;
        game->communicatorLogDetailScroll = 0.0f;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    tabIndex = GameOverlay_FindClickedIndexedRect(mouse, COMMUNICATOR_TAB_COUNT, UI_GetCommunicatorTabRect);
    if (tabIndex >= 0) {
        game->communicatorTab = (CommunicatorTab)tabIndex;
        ResetCommunicatorDetailState(game);
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
        ResetCommunicatorDetailState(game);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (CheckCollisionPointRec(mouse, contentPanel)) {
        game->communicatorLogDetailOpen = true;
        game->communicatorLogDetailScroll = 0.0f;
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
            ResetCommunicatorDetailState(game);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            return;
        }
    }
}

static void UpdateHelpOverlay(Game *game) {
    GameOverlay_TryCloseOverlay(game, &game->helpOpen, KEY_H);
}

static void UpdateMapOverlay(Game *game, bool *open, KeyboardKey alternateKey) {
    GameOverlay_TryCloseOverlay(game, open, alternateKey);
}

static void UpdateSettingsInfoTab(Game *game) {
    Rectangle closeRect;
    Rectangle languageEnglishRect;
    Rectangle languageChineseRect;
    Vector2 mouse;
    float scale;
    float nextVolume;
    int sliderIndex;

    if (game == NULL) {
        return;
    }

    scale = UIRuntime_GetScale(GetScreenWidth(), GetScreenHeight());
    closeRect = UI_GetSettingsCloseButtonRect(GetScreenWidth(), GetScreenHeight());
    languageEnglishRect = UI_GetSettingsLanguageButtonRect(GetScreenWidth(), GetScreenHeight(), 0);
    languageChineseRect = UI_GetSettingsLanguageButtonRect(GetScreenWidth(), GetScreenHeight(), 1);

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        ApplySettingsInfoSliderValue(game, 0, game->settings.masterVolume - 0.05f);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        ApplySettingsInfoSliderValue(game, 0, game->settings.masterVolume + 0.05f);
    }
    mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, closeRect)) {
            CloseInfoOverlay(game);
            return;
        }
        if (CheckCollisionPointRec(mouse, languageEnglishRect)) {
            Game_BeginLanguageTransition(game, GAME_LANGUAGE_EN);
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        }
        if (CheckCollisionPointRec(mouse, languageChineseRect)) {
            Game_BeginLanguageTransition(game, GAME_LANGUAGE_ZH_CN);
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        }
        for (sliderIndex = 0; sliderIndex < 3; ++sliderIndex) {
            Rectangle sliderRect;
            Rectangle decreaseRect;
            Rectangle increaseRect;
            Rectangle handleRect;
            float handleCenterX;

            sliderRect = UI_GetSettingsSliderRect(GetScreenWidth(), GetScreenHeight(), sliderIndex);
            decreaseRect = UI_GetSettingsDecreaseButtonRect(GetScreenWidth(), GetScreenHeight(), sliderIndex);
            increaseRect = UI_GetSettingsIncreaseButtonRect(GetScreenWidth(), GetScreenHeight(), sliderIndex);
            handleCenterX = sliderRect.x + sliderRect.width * GetSettingsInfoSliderValue(game, sliderIndex);
            if (handleCenterX < sliderRect.x + 14.0f * scale) {
                handleCenterX = sliderRect.x + 14.0f * scale;
            }
            if (handleCenterX > sliderRect.x + sliderRect.width - 14.0f * scale) {
                handleCenterX = sliderRect.x + sliderRect.width - 14.0f * scale;
            }
            handleRect = Rectangle{
                handleCenterX - 14.0f * scale,
                sliderRect.y - 8.0f * scale,
                28.0f * scale,
                sliderRect.height + 16.0f * scale
            };

            if (CheckCollisionPointRec(mouse, decreaseRect)) {
                ApplySettingsInfoSliderValue(game, sliderIndex, GetSettingsInfoSliderValue(game, sliderIndex) - 0.05f);
                return;
            }
            if (CheckCollisionPointRec(mouse, increaseRect)) {
                ApplySettingsInfoSliderValue(game, sliderIndex, GetSettingsInfoSliderValue(game, sliderIndex) + 0.05f);
                return;
            }
            if (CheckCollisionPointRec(mouse, sliderRect) || CheckCollisionPointRec(mouse, handleRect)) {
                game->settingsSliderDragging = true;
                game->settingsSliderDragIndex = sliderIndex;
                break;
            }
        }
    }

    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        game->settingsSliderDragging = false;
        game->settingsSliderDragIndex = -1;
    }

    if (!game->settingsSliderDragging || game->settingsSliderDragIndex < 0 || game->settingsSliderDragIndex > 2) {
        return;
    }

    sliderIndex = game->settingsSliderDragIndex;
    {
        Rectangle sliderRect;

        sliderRect = UI_GetSettingsSliderRect(GetScreenWidth(), GetScreenHeight(), sliderIndex);
        nextVolume = (mouse.x - sliderRect.x) / sliderRect.width;
    }
    ApplySettingsInfoSliderValue(game, sliderIndex, nextVolume);
}

static void UpdateInfoOverlay(Game *game) {
    Vector2 mouse;
    int tabIndex;
    bool communicatorDetailVisible;

    if (game == NULL) {
        return;
    }

    game->infoOverlayOpen = true;
    game->settingsOpen = false;
    game->infoOverlayTab = GetResolvedInfoOverlayTab(game);
    communicatorDetailVisible = game->infoOverlayTab == INFO_OVERLAY_TAB_LOXI
        && (game->communicatorLogDetailOpen || game->communicatorLogDetailVisibility > 0.001f);

    if (IsKeyPressed(KEY_ESCAPE) && !communicatorDetailVisible) {
        CloseInfoOverlay(game);
        return;
    }
    if (IsKeyPressed(KEY_M) && TrySwitchInfoOverlayTab(game, INFO_OVERLAY_TAB_MAP, true)) {
        return;
    }
    if (IsKeyPressed(KEY_B) && TrySwitchInfoOverlayTab(game, INFO_OVERLAY_TAB_BACKPACK, true)) {
        return;
    }
    if (IsKeyPressed(KEY_N) && TrySwitchInfoOverlayTab(game, INFO_OVERLAY_TAB_LOXI, true)) {
        return;
    }
    if (IsKeyPressed(KEY_O) && TrySwitchInfoOverlayTab(game, INFO_OVERLAY_TAB_SETTINGS, true)) {
        return;
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        tabIndex = GameOverlay_FindClickedIndexedRect(mouse, INFO_OVERLAY_TAB_COUNT, UI_GetInfoOverlayTabRect);
        if (tabIndex >= 0) {
            TrySwitchInfoOverlayTab(game, (InfoOverlayTab)tabIndex, false);
            return;
        }
    }

    switch (game->infoOverlayTab) {
        case INFO_OVERLAY_TAB_BACKPACK:
            UpdateBackpackOverlay(game, &game->infoOverlayOpen, KEY_NULL);
            break;
        case INFO_OVERLAY_TAB_LOXI:
            UpdateCommunicatorOverlay(game, &game->infoOverlayOpen, KEY_NULL);
            break;
        case INFO_OVERLAY_TAB_SETTINGS:
            UpdateSettingsInfoTab(game);
            break;
        case INFO_OVERLAY_TAB_MAP:
        default:
            UpdateMapOverlay(game, &game->infoOverlayOpen, KEY_NULL);
            break;
    }
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

    if (game->infoOverlayOpen) {
        UpdateInfoOverlay(game);
        return true;
    }

    if (game->helpOpen) {
        UpdateHelpOverlay(game);
        return true;
    }

    return false;
}
