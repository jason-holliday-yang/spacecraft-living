#include "game_overlay_internal.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

static int GetEndingChoicePopupButtonCount(const Game *game) {
    const int availableEndingCount = game != NULL ? Tasks_GetAvailableEndingCount(&game->tasks) : 0;

    return availableEndingCount > 1 ? availableEndingCount + 1 : 2;
}

static AudioCue GetRouteConfirmCue(GameEnding ending) {
    switch (ending) {
        case ENDING_PEACEFUL:
            return AUDIO_CUE_ENDING_PEACEFUL;
        case ENDING_SETTLEMENT:
            return AUDIO_CUE_ENDING_SETTLEMENT;
        case ENDING_HEROIC:
            return AUDIO_CUE_ENDING;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            return AUDIO_CUE_WARNING;
    }
}

static void ConfirmEndingChoice(Game *game, GameEnding ending) {
    char message[256];

    if (game == NULL) {
        return;
    }

    if (!Tasks_SelectEndingRoute(&game->tasks, ending)) {
        return;
    }

    Tasks_UpdateObjective(&game->tasks, &game->player);
    game->settlementConfirmOpen = false;
    game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
    switch (ending) {
        case ENDING_HEROIC:
            std::snprintf(message,
                          sizeof(message),
                          "%s",
                          Loc_PickLiteral("Heroic route locked. Hunt the guardian in the northwest ruins, then finish at the Signal Tower.",
                                          "强行救援路线已锁定。先去西北遗迹猎杀守卫，再到信号塔完成最后一步。"));
            break;
        case ENDING_PEACEFUL:
            std::snprintf(message,
                          sizeof(message),
                          "%s",
                          Loc_PickLiteral("Peaceful route locked. Prepare the Signal Amplifier, then carry it to the Signal Tower.",
                                          "和平救援路线已锁定。先准备信号放大器，再把它带到信号塔。"));
            break;
        case ENDING_SETTLEMENT:
            std::snprintf(message,
                          sizeof(message),
                          "%s",
                          Loc_PickLiteral("Settlement route locked. Return to Loxi once you are ready to confirm staying here for good.",
                                          "定居路线已锁定。等你准备好真正留下来时，再回到洛希那里确认。"));
            break;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            message[0] = '\0';
            break;
    }
    if (message[0] != '\0') {
        Game_PostMessage(game, message, 4.2f);
    }
    Audio_PlayCue(&game->audio, GetRouteConfirmCue(ending));
}

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
    game->settingsOpen = false;
    game->settingsSliderDragging = false;
    game->settingsSliderDragIndex = -1;
    Game_TrySaveSettings(game);
}

static void ApplyMasterVolume(Game *game, float volume) {
    float clamped;

    clamped = ClampFloatLocal(volume, 0.0f, 1.0f);
    if (std::fabs(clamped - game->settings.masterVolume) < 0.001f) {
        return;
    }

    game->settings.masterVolume = clamped;
    game->settingsDirty = true;
    Audio_SetMasterVolumeSetting(&game->audio, clamped);
}

static void ApplyMusicVolume(Game *game, float volume) {
    float clamped;

    clamped = ClampFloatLocal(volume, 0.0f, 1.0f);
    if (std::fabs(clamped - game->settings.musicVolume) < 0.001f) {
        return;
    }

    game->settings.musicVolume = clamped;
    game->settingsDirty = true;
    Audio_SetMusicVolumeSetting(&game->audio, clamped);
}

static void ApplySfxVolume(Game *game, float volume) {
    float clamped;

    clamped = ClampFloatLocal(volume, 0.0f, 1.0f);
    if (std::fabs(clamped - game->settings.sfxVolume) < 0.001f) {
        return;
    }

    game->settings.sfxVolume = clamped;
    game->settings.sfxEnabled = clamped > 0.001f;
    game->settingsDirty = true;
    Audio_SetSfxVolumeSetting(&game->audio, clamped);
    Audio_SetSfxEnabled(&game->audio, game->settings.sfxEnabled);
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
            return game->settings.masterVolume;
        case 1:
            return game->settings.musicVolume;
        case 2:
            return game->settings.sfxVolume;
        default:
            return 0.0f;
    }
}

static bool AreSettingsAccountActionsEnabled(const Game *game) {
    return game != NULL && game->state == GAME_STATE_INTRO && game->authenticated;
}

static void ClearAuthMessage(Game *game) {
    if (game == NULL) {
        return;
    }

    game->authMessage[0] = '\0';
}

static void ClearAuthPassword(Game *game) {
    if (game == NULL) {
        return;
    }

    game->authPassword[0] = '\0';
}

static void RestoreRememberedAuthUsername(Game *game) {
    if (game == NULL) {
        return;
    }

    std::snprintf(game->authUsername, sizeof(game->authUsername), "%s", game->settings.lastUsername);
}

static void ResetAuthScreen(Game *game, bool keepUsername) {
    if (game == NULL) {
        return;
    }

    if (!keepUsername) {
        RestoreRememberedAuthUsername(game);
    }
    ClearAuthPassword(game);
    ClearAuthMessage(game);
    game->authSelectedField = AUTH_FIELD_USERNAME;
    game->authPasswordVisible = false;
}

static void SetAuthenticatedState(Game *game, bool authenticated) {
    if (game == NULL) {
        return;
    }

    game->authenticated = authenticated;
    game->authHasAccounts = SaveSystem_HasRegisteredAccounts();
}

void Game_ToggleAuthMode(Game *game) {
    if (game == NULL) {
        return;
    }

    game->authMode = game->authMode == AUTH_SCREEN_MODE_LOGIN ? AUTH_SCREEN_MODE_REGISTER : AUTH_SCREEN_MODE_LOGIN;
    ResetAuthScreen(game, true);
}

void Game_LogoutToAuthScreen(Game *game) {
    if (game == NULL) {
        return;
    }

    SaveSystem_Logout();
    SetAuthenticatedState(game, false);
    game->authMode = game->authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    ResetAuthScreen(game, false);
    Game_CloseTransientOverlays(game);
    Game_RefreshSaveSlots(game);
}

void Game_CompleteAuthSuccess(Game *game) {
    if (game == NULL) {
        return;
    }

    SetAuthenticatedState(game, true);
    std::snprintf(game->settings.lastUsername, sizeof(game->settings.lastUsername), "%s", game->authUsername);
    game->settingsDirty = true;
    Game_TrySaveSettings(game);
    ClearAuthPassword(game);
    game->authSelectedField = AUTH_FIELD_USERNAME;
    game->authPasswordVisible = false;
    Game_RefreshSaveSlots(game);
}

static bool IsAuthDeleteEnabled(const Game *game) {
    return game != NULL && game->authMode == AUTH_SCREEN_MODE_LOGIN && game->authHasAccounts;
}

static void OpenAccountDeleteConfirm(Game *game) {
    if (game == NULL || !game->authenticated) {
        return;
    }

    game->accountDeleteConfirmOpen = true;
    game->accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->accountDeleteFromAuth = false;
    std::snprintf(game->accountDeleteTargetName, sizeof(game->accountDeleteTargetName), "%s", SaveSystem_GetActiveAccountName());
}

static void OpenAuthAccountDeleteConfirm(Game *game) {
    if (!IsAuthDeleteEnabled(game)) {
        return;
    }
    if (game->authUsername[0] == '\0' || game->authPassword[0] == '\0') {
        std::snprintf(game->authMessage,
                      sizeof(game->authMessage),
                      "%s",
                      Loc_PickLiteral("Enter that account's username and password before deleting it.",
                                      "删除账号前，请先输入该账号的用户名和密码。"));
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
        return;
    }

    game->accountDeleteConfirmOpen = true;
    game->accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->accountDeleteFromAuth = true;
    std::snprintf(game->accountDeleteTargetName, sizeof(game->accountDeleteTargetName), "%s", game->authUsername);
}

static void CloseAccountDeleteConfirm(Game *game) {
    if (game == NULL) {
        return;
    }

    game->accountDeleteConfirmOpen = false;
    game->accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->accountDeleteFromAuth = false;
    game->accountDeleteTargetName[0] = '\0';
}

static void MoveAuthFieldSelection(Game *game, int delta) {
    int selection;
    const int authFieldCount = 7;

    if (game == NULL) {
        return;
    }

    selection = (int)game->authSelectedField + delta;
    for (;;) {
        while (selection < AUTH_FIELD_USERNAME) {
            selection += authFieldCount;
        }
        while (selection > AUTH_FIELD_EXIT_GAME) {
            selection -= authFieldCount;
        }

        if (selection != AUTH_FIELD_DELETE_ACCOUNT || IsAuthDeleteEnabled(game)) {
            game->authSelectedField = (AuthField)selection;
            return;
        }

        selection += delta >= 0 ? 1 : -1;
    }
}

static void TogglePasswordVisibility(Game *game) {
    if (game == NULL) {
        return;
    }

    game->authPasswordVisible = !game->authPasswordVisible;
}

static void BackspaceAuthBuffer(char *buffer) {
    size_t length;

    if (buffer == NULL) {
        return;
    }

    length = std::strlen(buffer);
    if (length == 0) {
        return;
    }

    buffer[length - 1] = '\0';
}

static void AppendAuthCharacter(char *buffer, size_t bufferSize, int character) {
    size_t length;

    if (buffer == NULL || bufferSize == 0 || character <= 0 || character > 127) {
        return;
    }

    length = std::strlen(buffer);
    if (length + 1 >= bufferSize) {
        return;
    }

    buffer[length] = (char)character;
    buffer[length + 1] = '\0';
}

static void HandleAuthTextInput(Game *game) {
    char *buffer;
    size_t bufferSize;
    int character;

    if (game == NULL) {
        return;
    }

    buffer = NULL;
    bufferSize = 0;
    if (game->authSelectedField == AUTH_FIELD_USERNAME) {
        buffer = game->authUsername;
        bufferSize = sizeof(game->authUsername);
    } else if (game->authSelectedField == AUTH_FIELD_PASSWORD) {
        buffer = game->authPassword;
        bufferSize = sizeof(game->authPassword);
    }

    if (buffer == NULL) {
        return;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        BackspaceAuthBuffer(buffer);
    }

    character = GetCharPressed();
    while (character > 0) {
        if (game->authSelectedField == AUTH_FIELD_USERNAME) {
            if ((character >= '0' && character <= '9')
                || (character >= 'A' && character <= 'Z')
                || (character >= 'a' && character <= 'z')
                || character == '_'
                || character == '-') {
                AppendAuthCharacter(buffer, bufferSize, character);
            }
        } else if (character >= 32 && character <= 126) {
            AppendAuthCharacter(buffer, bufferSize, character);
        }

        character = GetCharPressed();
    }
}

static void SubmitAuth(Game *game) {
    bool success;

    if (game == NULL) {
        return;
    }

    success = game->authMode == AUTH_SCREEN_MODE_REGISTER
        ? SaveSystem_Register(game->authUsername, game->authPassword, game->authMessage, sizeof(game->authMessage))
        : SaveSystem_Login(game->authUsername, game->authPassword, game->authMessage, sizeof(game->authMessage));
    if (!success) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
        SetAuthenticatedState(game, false);
        return;
    }

    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    Game_BeginScreenTransition(game, SCREEN_TRANSITION_AUTH_SUCCESS, -1);
}

static void DeleteActiveAccount(Game *game) {
    char deletedAccountName[SAVE_ACCOUNT_NAME_MAX];
    char deleteMessage[SAVE_AUTH_MESSAGE_MAX];
    bool deleted;

    if (game == NULL) {
        return;
    }

    std::snprintf(deletedAccountName, sizeof(deletedAccountName), "%s", SaveSystem_GetActiveAccountName());
    deleted = SaveSystem_DeleteActiveAccount(game->authMessage, sizeof(game->authMessage));
    if (!deleted) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
        CloseAccountDeleteConfirm(game);
        SetAuthenticatedState(game, SaveSystem_IsAccountAuthenticated());
        return;
    }

    if (std::strcmp(game->settings.lastUsername, deletedAccountName) == 0) {
        game->settings.lastUsername[0] = '\0';
        game->settingsDirty = true;
        Game_TrySaveSettings(game);
    }

    std::snprintf(deleteMessage, sizeof(deleteMessage), "%s", game->authMessage);
    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    CloseAccountDeleteConfirm(game);
    SetAuthenticatedState(game, false);
    game->authMode = game->authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    ResetAuthScreen(game, false);
    std::snprintf(game->authMessage, sizeof(game->authMessage), "%s", deleteMessage);
    Game_CloseTransientOverlays(game);
    Game_RefreshSaveSlots(game);
}

static void DeleteAuthSelectedAccount(Game *game) {
    char deletedAccountName[SAVE_ACCOUNT_NAME_MAX];
    char deleteMessage[SAVE_AUTH_MESSAGE_MAX];
    bool deleted;

    if (game == NULL) {
        return;
    }

    std::snprintf(deletedAccountName, sizeof(deletedAccountName), "%s", game->authUsername);
    deleted = SaveSystem_DeleteAccount(game->authUsername, game->authPassword, game->authMessage, sizeof(game->authMessage));
    if (!deleted) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
        CloseAccountDeleteConfirm(game);
        SetAuthenticatedState(game, SaveSystem_IsAccountAuthenticated());
        return;
    }

    if (std::strcmp(game->settings.lastUsername, deletedAccountName) == 0) {
        game->settings.lastUsername[0] = '\0';
        game->settingsDirty = true;
        Game_TrySaveSettings(game);
    }

    std::snprintf(deleteMessage, sizeof(deleteMessage), "%s", game->authMessage);
    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    CloseAccountDeleteConfirm(game);
    SetAuthenticatedState(game, false);
    game->authMode = game->authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    ResetAuthScreen(game, false);
    std::snprintf(game->authMessage, sizeof(game->authMessage), "%s", deleteMessage);
    Game_RefreshSaveSlots(game);
}

static void UpdateAccountDeleteConfirm(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    bool activateSelection;

    activateSelection = false;

    if (IsKeyPressed(KEY_ESCAPE)) {
        CloseAccountDeleteConfirm(game);
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        game->accountDeleteConfirmSelection = (game->accountDeleteConfirmSelection + ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT - 1) % ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }
    if (GameOverlay_IsForwardNavigationPressed()) {
        game->accountDeleteConfirmSelection = (game->accountDeleteConfirmSelection + 1) % ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = GameOverlay_FindClickedIndexedRect(mouse, ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT, UI_GetAccountDeleteConfirmButtonRect);
        if (buttonIndex >= 0) {
            game->accountDeleteConfirmSelection = buttonIndex;
            activateSelection = true;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        activateSelection = true;
    }

    if (!activateSelection) {
        return;
    }

    if (game->accountDeleteConfirmSelection == ACCOUNT_DELETE_CONFIRM_BUTTON_DELETE) {
        if (game->accountDeleteFromAuth) {
            DeleteAuthSelectedAccount(game);
        } else {
            DeleteActiveAccount(game);
        }
        return;
    }

    CloseAccountDeleteConfirm(game);
    Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
}

static void UpdateAuthScreen(Game *game) {
    Vector2 mouse;
    Rectangle usernameRect;
    Rectangle passwordRect;
    Rectangle togglePasswordRect;
    Rectangle deleteRect;
    Rectangle submitRect;
    Rectangle switchRect;
    Rectangle exitRect;
    bool editingTextField;

    usernameRect = UI_GetAuthInputRect(GetScreenWidth(), GetScreenHeight(), 0);
    passwordRect = UI_GetAuthInputRect(GetScreenWidth(), GetScreenHeight(), 1);
    togglePasswordRect = UI_GetAuthPasswordToggleRect(GetScreenWidth(), GetScreenHeight());
    deleteRect = UI_GetAuthDeleteAccountRect(GetScreenWidth(), GetScreenHeight());
    submitRect = UI_GetAuthSubmitButtonRect(GetScreenWidth(), GetScreenHeight());
    switchRect = UI_GetAuthSwitchModeRect(GetScreenWidth(), GetScreenHeight());
    exitRect = UI_GetAuthExitButtonRect(GetScreenWidth(), GetScreenHeight());
    editingTextField = game->authSelectedField == AUTH_FIELD_USERNAME || game->authSelectedField == AUTH_FIELD_PASSWORD;

    if (IsKeyPressed(KEY_TAB)) {
        MoveAuthFieldSelection(game, (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ? -1 : 1);
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }
    if (IsKeyPressed(KEY_DOWN) || (!editingTextField && IsKeyPressed(KEY_S))) {
        MoveAuthFieldSelection(game, 1);
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }
    if (IsKeyPressed(KEY_UP) || (!editingTextField && IsKeyPressed(KEY_W))) {
        MoveAuthFieldSelection(game, -1);
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    HandleAuthTextInput(game);

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        if (CheckCollisionPointRec(mouse, togglePasswordRect)) {
            game->authSelectedField = AUTH_FIELD_TOGGLE_PASSWORD;
            TogglePasswordVisibility(game);
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        }
        if (CheckCollisionPointRec(mouse, usernameRect)) {
            game->authSelectedField = AUTH_FIELD_USERNAME;
            return;
        }
        if (CheckCollisionPointRec(mouse, passwordRect)) {
            game->authSelectedField = AUTH_FIELD_PASSWORD;
            return;
        }
        if (CheckCollisionPointRec(mouse, deleteRect) && IsAuthDeleteEnabled(game)) {
            game->authSelectedField = AUTH_FIELD_DELETE_ACCOUNT;
            Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
            OpenAuthAccountDeleteConfirm(game);
            return;
        }
        if (CheckCollisionPointRec(mouse, submitRect)) {
            game->authSelectedField = AUTH_FIELD_SUBMIT;
            SubmitAuth(game);
            return;
        }
        if (CheckCollisionPointRec(mouse, switchRect)) {
            game->authSelectedField = AUTH_FIELD_SWITCH_MODE;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_TOGGLE_AUTH_MODE, -1);
            return;
        }
        if (CheckCollisionPointRec(mouse, exitRect)) {
            game->authSelectedField = AUTH_FIELD_EXIT_GAME;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
            game->requestClose = true;
            return;
        }
    }

    if (!GameOverlay_IsConfirmPressed()) {
        return;
    }

    switch (game->authSelectedField) {
        case AUTH_FIELD_USERNAME:
            game->authSelectedField = AUTH_FIELD_PASSWORD;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        case AUTH_FIELD_PASSWORD:
            game->authSelectedField = AUTH_FIELD_TOGGLE_PASSWORD;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        case AUTH_FIELD_TOGGLE_PASSWORD:
            TogglePasswordVisibility(game);
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        case AUTH_FIELD_DELETE_ACCOUNT:
            Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
            OpenAuthAccountDeleteConfirm(game);
            return;
        case AUTH_FIELD_SUBMIT:
            SubmitAuth(game);
            return;
        case AUTH_FIELD_SWITCH_MODE:
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_TOGGLE_AUTH_MODE, -1);
            return;
        case AUTH_FIELD_EXIT_GAME:
            Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
            game->requestClose = true;
            return;
        default:
            break;
    }
}

static void UpdateMainMenu(Game *game) {
    Vector2 mouse;
    int buttonIndex;

    if (GameOverlay_IsConfirmPressed()) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        Game_StartNewGame(game);
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    buttonIndex = UI_GetMainMenuButtonIndexAtPoint(GetScreenWidth(), GetScreenHeight(), mouse);
    switch (buttonIndex) {
        case MAIN_MENU_BUTTON_START:
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            Game_StartNewGame(game);
            return;
        case MAIN_MENU_BUTTON_LOAD:
            if (game->hasSaveFile) {
                Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
                Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
            }
            return;
        case MAIN_MENU_BUTTON_SETTINGS:
            game->settingsOpen = true;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        case MAIN_MENU_BUTTON_EXIT:
            game->requestClose = true;
            return;
        default:
            break;
    }
}

static void UpdateOpeningCutscene(Game *game) {
    static constexpr float kOpeningInitialBlackHoldDuration = 1.5f;

    if (game->narrativeTransitionActive) {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        game->narrativeTransitionActive = true;
        game->narrativeTransitionElapsed = 0.0f;
        game->narrativeTransitionAction = NARRATIVE_TRANSITION_OPENING_COMPLETE;
        return;
    }

    if (!GameOverlay_IsCutsceneAdvancePressed()) {
        return;
    }

    if (game->openingAwaitingFirstAdvance) {
        if (game->openingCutsceneElapsed < kOpeningInitialBlackHoldDuration) {
            return;
        }

        game->openingAwaitingFirstAdvance = false;
        game->openingCutsceneElapsed = 0.0f;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        return;
    }

    if (game->openingSlideIndex + 1 >= INTRO_CUTSCENE_SLIDE_COUNT) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
        game->narrativeTransitionActive = true;
        game->narrativeTransitionElapsed = 0.0f;
        game->narrativeTransitionAction = NARRATIVE_TRANSITION_OPENING_COMPLETE;
        return;
    }

    Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    game->narrativeTransitionActive = true;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_OPENING_NEXT_SLIDE;
}

static void UpdateStoryScene(Game *game) {
    if (game->narrativeTransitionActive) {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        game->narrativeTransitionActive = true;
        game->narrativeTransitionElapsed = 0.0f;
        game->narrativeTransitionAction = NARRATIVE_TRANSITION_STORY_CLOSE;
        return;
    }

    if (!GameOverlay_IsCutsceneAdvancePressed()) {
        return;
    }

    Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
    game->narrativeTransitionActive = true;
    game->narrativeTransitionElapsed = 0.0f;
    game->narrativeTransitionAction = NARRATIVE_TRANSITION_STORY_CLOSE;
}

static void UpdatePauseMenu(Game *game) {
    Vector2 mouse;
    int buttonIndex;

    if (GameOverlay_TryCloseOverlay(game, &game->pauseMenuOpen, KEY_NULL)) {
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    buttonIndex = GameOverlay_FindClickedIndexedRect(mouse, PAUSE_MENU_BUTTON_COUNT, UI_GetPauseMenuButtonRect);
    if (buttonIndex < 0) {
        return;
    }

    switch (buttonIndex) {
        case PAUSE_MENU_BUTTON_CONTINUE:
            game->pauseMenuOpen = false;
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            return;
        case PAUSE_MENU_BUTTON_SAVE:
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            Game_OpenSavePanel(game, SAVE_PANEL_MODE_SAVE);
            return;
        case PAUSE_MENU_BUTTON_LOAD:
            if (game->hasSaveFile) {
                Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
                Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
            } else {
                Game_PostMessage(game, LOC_UI_SAVE_MISSING, 2.8f);
            }
            return;
        case PAUSE_MENU_BUTTON_SETTINGS:
            game->pauseMenuOpen = false;
            game->infoOverlayOpen = true;
            game->infoOverlayTab = INFO_OVERLAY_TAB_SETTINGS;
            game->settingsOpen = false;
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
            return;
        case PAUSE_MENU_BUTTON_MENU:
            Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_RETURN_TO_MENU, -1);
            return;
        default:
            break;
    }
}

static void UpdateSettingsOverlay(Game *game) {
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
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        ApplySettingsSliderValue(game, 0, game->settings.masterVolume - 0.05f);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        ApplySettingsSliderValue(game, 0, game->settings.masterVolume + 0.05f);
    }
    mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, closeRect)) {
            CloseSettingsOverlay(game);
            Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
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
        if (accountActionsEnabled && CheckCollisionPointRec(mouse, accountSwitchRect)) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_LOGOUT_TO_AUTH, -1);
            return;
        }
        if (accountActionsEnabled && CheckCollisionPointRec(mouse, accountDeleteRect)) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_WARNING);
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
    ApplySettingsSliderValue(game, sliderIndex, nextVolume);
}

static void UpdateSettlementConfirm(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    int buttonCount;
    int availableEndingCount;
    bool activateSelection;

    activateSelection = false;
    availableEndingCount = game != NULL ? Tasks_GetAvailableEndingCount(&game->tasks) : 0;
    buttonCount = GetEndingChoicePopupButtonCount(game);

    if (availableEndingCount <= 0) {
        game->settlementConfirmOpen = false;
        game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->settlementConfirmOpen = false;
        game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        game->settlementConfirmSelection = (game->settlementConfirmSelection + buttonCount - 1) % buttonCount;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
        game->settlementConfirmSelection = (game->settlementConfirmSelection + 1) % buttonCount;
        Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = -1;
        for (int index = 0; index < buttonCount; index++) {
            if (CheckCollisionPointRec(mouse,
                                       UI_GetSettlementConfirmButtonRect(GetScreenWidth(),
                                                                         GetScreenHeight(),
                                                                         index,
                                                                         buttonCount))) {
                buttonIndex = index;
                break;
            }
        }
        if (buttonIndex >= 0) {
            game->settlementConfirmSelection = buttonIndex;
            activateSelection = true;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        activateSelection = true;
    }

    if (!activateSelection) {
        return;
    }

    if (game->settlementConfirmSelection >= availableEndingCount) {
        game->settlementConfirmOpen = false;
        game->settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
        Audio_PlayCue(&game->audio, AUDIO_CUE_CLOSE);
        return;
    }

    ConfirmEndingChoice(game, Tasks_GetAvailableEndingAt(&game->tasks, game->settlementConfirmSelection));
}

static void UpdateSavePanel(Game *game) {
    Vector2 mouse;
    int slotIndex;

    if (GameOverlay_TryCloseOverlay(game, &game->savePanelOpen, KEY_NULL)) {
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
        if (game->savePanelMode == SAVE_PANEL_MODE_LOAD
            && game->selectedSaveSlot >= 0
            && game->selectedSaveSlot < SAVE_SLOT_COUNT
            && game->saveSlots[game->selectedSaveSlot].exists) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_LOAD_GAME, game->selectedSaveSlot);
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
        if (game->selectedSaveSlot != slotIndex) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_OPEN);
        }
        game->selectedSaveSlot = slotIndex;
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetSavePrimaryButtonRect(GetScreenWidth(), GetScreenHeight()))) {
        if (game->savePanelMode == SAVE_PANEL_MODE_LOAD
            && game->selectedSaveSlot >= 0
            && game->selectedSaveSlot < SAVE_SLOT_COUNT
            && game->saveSlots[game->selectedSaveSlot].exists) {
            Audio_PlayCue(&game->audio, AUDIO_CUE_CONFIRM);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_LOAD_GAME, game->selectedSaveSlot);
            return;
        }
        Game_ActivateSelectedSaveSlot(game);
        return;
    }

    if (CheckCollisionPointRec(mouse, UI_GetSaveDeleteButtonRect(GetScreenWidth(), GetScreenHeight()))) {
        Game_DeleteSelectedSaveSlot(game);
    }
}

bool Game_UpdateFrontEndOverlayState(Game *game) {
    if (game->state == GAME_STATE_INTRO) {
        if (game->accountDeleteConfirmOpen) {
            UpdateAccountDeleteConfirm(game);
        } else if (!game->authenticated) {
            UpdateAuthScreen(game);
        } else if (game->settingsOpen) {
            UpdateSettingsOverlay(game);
        } else if (game->savePanelOpen) {
            UpdateSavePanel(game);
        } else {
            UpdateMainMenu(game);
        }
        return true;
    }

    if (game->state == GAME_STATE_OPENING) {
        UpdateOpeningCutscene(game);
        return true;
    }

    if (game->storySceneOpen) {
        UpdateStoryScene(game);
        return true;
    }

    if (game->state == GAME_STATE_ENDING) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->requestClose = true;
        }
        return true;
    }

    if (game->settlementConfirmOpen) {
        UpdateSettlementConfirm(game);
        return true;
    }

    if (game->savePanelOpen) {
        UpdateSavePanel(game);
        return true;
    }

    if (game->pauseMenuOpen) {
        UpdatePauseMenu(game);
        return true;
    }

    return false;
}
