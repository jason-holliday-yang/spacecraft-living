#include "game_overlay_gameplay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

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
            game->services.settings.masterVolume = clampedValue;
            game->ui.settingsDirty = true;
            Audio_SetMasterVolumeSetting(&game->services.audio, clampedValue);
            return;
        case 1:
            game->services.settings.musicVolume = clampedValue;
            game->ui.settingsDirty = true;
            Audio_SetMusicVolumeSetting(&game->services.audio, clampedValue);
            return;
        case 2:
            game->services.settings.sfxVolume = clampedValue;
            game->services.settings.sfxEnabled = clampedValue > 0.001f;
            game->ui.settingsDirty = true;
            Audio_SetSfxVolumeSetting(&game->services.audio, clampedValue);
            Audio_SetSfxEnabled(&game->services.audio, game->services.settings.sfxEnabled);
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
            return game->services.settings.masterVolume;
        case 1:
            return game->services.settings.musicVolume;
        case 2:
            return game->services.settings.sfxVolume;
        default:
            return 0.0f;
    }
}

static InfoOverlayTab GetResolvedInfoOverlayTab(const Game *game) {
    if (game == NULL) {
        return INFO_OVERLAY_TAB_MAP;
    }

    if (game->ui.infoOverlayTab == INFO_OVERLAY_TAB_LOXI && !Tasks_IsCommunicatorUnlocked(&game->runtime.tasks)) {
        return INFO_OVERLAY_TAB_MAP;
    }
    if (game->ui.infoOverlayTab < INFO_OVERLAY_TAB_MAP || game->ui.infoOverlayTab >= INFO_OVERLAY_TAB_COUNT) {
        return INFO_OVERLAY_TAB_MAP;
    }
    return game->ui.infoOverlayTab;
}

static void CloseInfoOverlay(Game *game) {
    if (game == NULL) {
        return;
    }

    if (game->ui.settingsDirty) {
        Game_TrySaveSettings(game);
    }
    game->ui.infoOverlayOpen = false;
    game->ui.settingsOpen = false;
    game->ui.settingsSliderDragging = false;
    game->ui.settingsSliderDragIndex = -1;
    GameOverlay_ResetCommunicatorDetailState(game);
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
}

static bool TrySwitchInfoOverlayTab(Game *game, InfoOverlayTab tab, bool closeIfSelected) {
    const InfoOverlayTab currentTab = GetResolvedInfoOverlayTab(game);

    if (game == NULL) {
        return false;
    }

    if (tab == INFO_OVERLAY_TAB_LOXI && !Tasks_IsCommunicatorUnlocked(&game->runtime.tasks)) {
        Game_PostMessage(game,
                         Loc_PickLiteral("Loxi link is offline. Sync with the terminal bay uplink first, then press N.",
                                         "洛希链路尚未上线。请先在终端舱完成同步，再按 N。"),
                         2.8f);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
        return true;
    }

    if (currentTab == tab) {
        if (closeIfSelected) {
            CloseInfoOverlay(game);
            return true;
        }
        return false;
    }

    if (currentTab == INFO_OVERLAY_TAB_SETTINGS && game->ui.settingsDirty) {
        Game_TrySaveSettings(game);
    }
    game->ui.infoOverlayOpen = true;
    game->ui.infoOverlayTab = tab;
    game->ui.settingsOpen = false;
    game->ui.settingsSliderDragging = false;
    game->ui.settingsSliderDragIndex = -1;
    GameOverlay_ResetCommunicatorDetailState(game);
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    return true;
}

void GameOverlay_UpdateHelp(Game *game) {
    GameOverlay_TryCloseOverlay(game, &game->ui.helpOpen, KEY_H);
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
        ApplySettingsInfoSliderValue(game, 0, game->services.settings.masterVolume - 0.05f);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        ApplySettingsInfoSliderValue(game, 0, game->services.settings.masterVolume + 0.05f);
    }
    mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, closeRect)) {
            CloseInfoOverlay(game);
            return;
        }
        if (CheckCollisionPointRec(mouse, languageEnglishRect)) {
            Game_BeginLanguageTransition(game, GAME_LANGUAGE_EN);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        }
        if (CheckCollisionPointRec(mouse, languageChineseRect)) {
            Game_BeginLanguageTransition(game, GAME_LANGUAGE_ZH_CN);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
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
                game->ui.settingsSliderDragging = true;
                game->ui.settingsSliderDragIndex = sliderIndex;
                break;
            }
        }
    }

    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        game->ui.settingsSliderDragging = false;
        game->ui.settingsSliderDragIndex = -1;
    }

    if (!game->ui.settingsSliderDragging || game->ui.settingsSliderDragIndex < 0 || game->ui.settingsSliderDragIndex > 2) {
        return;
    }

    sliderIndex = game->ui.settingsSliderDragIndex;
    {
        Rectangle sliderRect;

        sliderRect = UI_GetSettingsSliderRect(GetScreenWidth(), GetScreenHeight(), sliderIndex);
        nextVolume = (mouse.x - sliderRect.x) / sliderRect.width;
    }
    ApplySettingsInfoSliderValue(game, sliderIndex, nextVolume);
}

void GameOverlay_UpdateInfo(Game *game) {
    Vector2 mouse;
    int tabIndex;
    bool communicatorDetailVisible;

    if (game == NULL) {
        return;
    }

    game->ui.infoOverlayOpen = true;
    game->ui.settingsOpen = false;
    game->ui.infoOverlayTab = GetResolvedInfoOverlayTab(game);
    communicatorDetailVisible = game->ui.infoOverlayTab == INFO_OVERLAY_TAB_LOXI
        && (game->ui.communicatorLogDetailOpen || game->ui.communicatorLogDetailVisibility > 0.001f);

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

    switch (game->ui.infoOverlayTab) {
        case INFO_OVERLAY_TAB_BACKPACK:
            GameOverlay_UpdateBackpack(game, &game->ui.infoOverlayOpen, KEY_NULL);
            break;
        case INFO_OVERLAY_TAB_LOXI:
            GameOverlay_UpdateCommunicator(game, &game->ui.infoOverlayOpen, KEY_NULL);
            break;
        case INFO_OVERLAY_TAB_SETTINGS:
            UpdateSettingsInfoTab(game);
            break;
        case INFO_OVERLAY_TAB_MAP:
        default:
            GameOverlay_UpdateMap(game, &game->ui.infoOverlayOpen, KEY_NULL);
            break;
    }
}
