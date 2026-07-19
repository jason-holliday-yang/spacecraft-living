#include "game_overlay_frontend_internal.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

static float ClampFloatLocal(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void CloseSettingsOverlay(Game *game) {
    game->ui.settingsOpen = false;
    game->ui.settingsSliderDragging = false;
    game->ui.settingsSliderDragIndex = -1;
    Game_TrySaveSettings(game);
}

static void ApplyMasterVolume(Game *game, float volume) {
    float clamped;

    clamped = ClampFloatLocal(volume, 0.0f, 1.0f);
    if (std::fabs(clamped - game->services.settings.masterVolume) < 0.001f) {
        return;
    }

    game->services.settings.masterVolume = clamped;
    game->ui.settingsDirty = true;
    Audio_SetMasterVolumeSetting(&game->services.audio, clamped);
}

static void ApplyMusicVolume(Game *game, float volume) {
    float clamped;

    clamped = ClampFloatLocal(volume, 0.0f, 1.0f);
    if (std::fabs(clamped - game->services.settings.musicVolume) < 0.001f) {
        return;
    }

    game->services.settings.musicVolume = clamped;
    game->ui.settingsDirty = true;
    Audio_SetMusicVolumeSetting(&game->services.audio, clamped);
}

static void ApplySfxVolume(Game *game, float volume) {
    float clamped;

    clamped = ClampFloatLocal(volume, 0.0f, 1.0f);
    if (std::fabs(clamped - game->services.settings.sfxVolume) < 0.001f) {
        return;
    }

    game->services.settings.sfxVolume = clamped;
    game->services.settings.sfxEnabled = clamped > 0.001f;
    game->ui.settingsDirty = true;
    Audio_SetSfxVolumeSetting(&game->services.audio, clamped);
    Audio_SetSfxEnabled(&game->services.audio, game->services.settings.sfxEnabled);
}

static void ApplySettingsSliderValue(Game *game, int sliderIndex, float value) {
    switch (sliderIndex) {
        case 0:
            ApplyMasterVolume(game, value);
            return;
        case 1:
            ApplyMusicVolume(game, value);
            return;
        case 2:
            ApplySfxVolume(game, value);
            return;
        default:
            return;
    }
}

static float GetSettingsSliderValue(const Game *game, int sliderIndex) {
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

static void OpenAccountDeleteConfirm(Game *game) {
    if (game == NULL || !game->account.authenticated) {
        return;
    }

    game->account.accountDeleteConfirmOpen = true;
    game->account.accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->account.accountDeleteFromAuth = false;
    std::snprintf(game->account.accountDeleteTargetName, sizeof(game->account.accountDeleteTargetName), "%s", SaveSystem_GetActiveAccountName());
}

static bool AreSettingsAccountActionsEnabled(const Game *game) {
    return game != NULL && game->flow.state == GAME_STATE_INTRO && game->account.authenticated;
}

void GameOverlay_UpdateSettingsOverlay(Game *game) {
    Rectangle closeRect;
    Rectangle languageEnglishRect;
    Rectangle languageChineseRect;
    Rectangle accountSwitchRect;
    Rectangle accountDeleteRect;
    Vector2 mouse;
    float scale;
    float nextVolume;
    bool accountActionsEnabled;
    int sliderIndex;

    scale = UIRuntime_GetScale(GetScreenWidth(), GetScreenHeight());
    closeRect = UI_GetSettingsCloseButtonRect(GetScreenWidth(), GetScreenHeight());
    languageEnglishRect = UI_GetSettingsLanguageButtonRect(GetScreenWidth(), GetScreenHeight(), 0);
    languageChineseRect = UI_GetSettingsLanguageButtonRect(GetScreenWidth(), GetScreenHeight(), 1);
    accountSwitchRect = UI_GetSettingsAccountButtonRect(GetScreenWidth(), GetScreenHeight(), 0);
    accountDeleteRect = UI_GetSettingsAccountButtonRect(GetScreenWidth(), GetScreenHeight(), 1);
    accountActionsEnabled = AreSettingsAccountActionsEnabled(game);

    if (IsKeyPressed(KEY_ESCAPE)) {
        CloseSettingsOverlay(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        ApplySettingsSliderValue(game, 0, game->services.settings.masterVolume - 0.05f);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        ApplySettingsSliderValue(game, 0, game->services.settings.masterVolume + 0.05f);
    }
    mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, closeRect)) {
            CloseSettingsOverlay(game);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
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
        if (accountActionsEnabled && CheckCollisionPointRec(mouse, accountSwitchRect)) {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_LOGOUT_TO_AUTH, -1);
            return;
        }
        if (accountActionsEnabled && CheckCollisionPointRec(mouse, accountDeleteRect)) {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
            OpenAccountDeleteConfirm(game);
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
            handleCenterX = sliderRect.x + sliderRect.width * GetSettingsSliderValue(game, sliderIndex);
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
                ApplySettingsSliderValue(game, sliderIndex, GetSettingsSliderValue(game, sliderIndex) - 0.05f);
                return;
            }
            if (CheckCollisionPointRec(mouse, increaseRect)) {
                ApplySettingsSliderValue(game, sliderIndex, GetSettingsSliderValue(game, sliderIndex) + 0.05f);
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
    ApplySettingsSliderValue(game, sliderIndex, nextVolume);
}
