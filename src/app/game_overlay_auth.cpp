#include "game_overlay_frontend_internal.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

static void ClearAuthMessage(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.authMessage[0] = '\0';
}

static void ClearAuthPassword(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.authPassword[0] = '\0';
}

static void RestoreRememberedAuthUsername(Game *game) {
    if (game == NULL) {
        return;
    }

    std::snprintf(game->account.authUsername, sizeof(game->account.authUsername), "%s", game->services.settings.lastUsername);
}

static int FindAuthAccountIndex(const Game *game, const char *accountName) {
    int accountIndex;

    if (game == NULL || accountName == NULL || accountName[0] == '\0') {
        return -1;
    }

    for (accountIndex = 0; accountIndex < game->account.authAccountCount; ++accountIndex) {
        if (std::strcmp(game->account.authAccountNames[accountIndex], accountName) == 0) {
            return accountIndex;
        }
    }

    return -1;
}

static void KeepAuthAccountPickerSelectionVisible(Game *game) {
    int visibleCount;
    int maxFirstVisible;

    if (game == NULL) {
        return;
    }

    visibleCount = UI_GetAuthAccountPickerVisibleRowCount();
    maxFirstVisible = game->account.authAccountCount - visibleCount;
    if (maxFirstVisible < 0) {
        maxFirstVisible = 0;
    }

    if (game->account.authAccountPickerSelection < game->account.authAccountPickerFirstVisible) {
        game->account.authAccountPickerFirstVisible = game->account.authAccountPickerSelection;
    } else if (game->account.authAccountPickerSelection >= game->account.authAccountPickerFirstVisible + visibleCount) {
        game->account.authAccountPickerFirstVisible = game->account.authAccountPickerSelection - visibleCount + 1;
    }

    if (game->account.authAccountPickerFirstVisible < 0) {
        game->account.authAccountPickerFirstVisible = 0;
    }
    if (game->account.authAccountPickerFirstVisible > maxFirstVisible) {
        game->account.authAccountPickerFirstVisible = maxFirstVisible;
    }
}

static void RefreshAuthAccountList(Game *game) {
    int selectedAccount;

    if (game == NULL) {
        return;
    }

    game->account.authAccountCount = SaveSystem_ListRegisteredAccounts(&game->account.authAccountNames[0][0],
                                                               sizeof(game->account.authAccountNames[0]),
                                                               SAVE_ACCOUNT_LIST_MAX);
    selectedAccount = FindAuthAccountIndex(game, game->account.authUsername);
    if (selectedAccount < 0 && game->account.authAccountCount > 0) {
        selectedAccount = 0;
    }

    game->account.authAccountPickerSelection = selectedAccount;
    game->account.authAccountPickerFirstVisible = 0;
    KeepAuthAccountPickerSelectionVisible(game);

    if (game->account.authMode == AUTH_SCREEN_MODE_LOGIN && selectedAccount >= 0) {
        std::snprintf(game->account.authUsername,
                      sizeof(game->account.authUsername),
                      "%s",
                      game->account.authAccountNames[selectedAccount]);
    }
}

static void CloseAuthAccountPicker(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.authAccountPickerOpen = false;
}

static void OpenAuthAccountPicker(Game *game) {
    if (game == NULL || game->account.authMode != AUTH_SCREEN_MODE_LOGIN) {
        return;
    }

    RefreshAuthAccountList(game);
    if (game->account.authAccountCount <= 0) {
        return;
    }

    game->account.authAccountPickerOpen = true;
    game->account.authSelectedField = AUTH_FIELD_USERNAME;
}

static void ChooseAuthAccount(Game *game, int accountIndex) {
    if (game == NULL || accountIndex < 0 || accountIndex >= game->account.authAccountCount) {
        return;
    }

    std::snprintf(game->account.authUsername,
                  sizeof(game->account.authUsername),
                  "%s",
                  game->account.authAccountNames[accountIndex]);
    game->account.authAccountPickerSelection = accountIndex;
    ClearAuthPassword(game);
    ClearAuthMessage(game);
    CloseAuthAccountPicker(game);
    game->account.authSelectedField = AUTH_FIELD_PASSWORD;
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
    CloseAuthAccountPicker(game);
    game->account.authSelectedField = AUTH_FIELD_USERNAME;
    game->account.authPasswordVisible = false;
    if (game->account.authMode == AUTH_SCREEN_MODE_LOGIN) {
        RefreshAuthAccountList(game);
    }
}

static void SetAuthenticatedState(Game *game, bool authenticated) {
    if (game == NULL) {
        return;
    }

    game->account.authenticated = authenticated;
    game->account.authHasAccounts = SaveSystem_HasRegisteredAccounts();
}

void Game_ToggleAuthMode(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.authMode = game->account.authMode == AUTH_SCREEN_MODE_LOGIN ? AUTH_SCREEN_MODE_REGISTER : AUTH_SCREEN_MODE_LOGIN;
    ResetAuthScreen(game, true);
}

void Game_LogoutToAuthScreen(Game *game) {
    if (game == NULL) {
        return;
    }

    SaveSystem_Logout();
    SetAuthenticatedState(game, false);
    game->account.authMode = game->account.authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    ResetAuthScreen(game, false);
    Game_CloseTransientOverlays(game);
    Game_RefreshSaveSlots(game);
}

void Game_CompleteAuthSuccess(Game *game) {
    if (game == NULL) {
        return;
    }

    SetAuthenticatedState(game, true);
    std::snprintf(game->services.settings.lastUsername, sizeof(game->services.settings.lastUsername), "%s", game->account.authUsername);
    game->ui.settingsDirty = true;
    Game_TrySaveSettings(game);
    ClearAuthPassword(game);
    CloseAuthAccountPicker(game);
    game->account.authSelectedField = AUTH_FIELD_USERNAME;
    game->account.authPasswordVisible = false;
    Game_RefreshSaveSlots(game);
}

static bool IsAuthDeleteEnabled(const Game *game) {
    return game != NULL && game->account.authMode == AUTH_SCREEN_MODE_LOGIN && game->account.authHasAccounts;
}

static void OpenAuthAccountDeleteConfirm(Game *game) {
    if (!IsAuthDeleteEnabled(game)) {
        return;
    }
    if (game->account.authUsername[0] == '\0' || game->account.authPassword[0] == '\0') {
        std::snprintf(game->account.authMessage,
                      sizeof(game->account.authMessage),
                      "%s",
                      Loc_PickLiteral("Enter that account's username and password before deleting it.",
                                      "删除账号前，请先输入该账号的用户名和密码。"));
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
        return;
    }

    game->account.accountDeleteConfirmOpen = true;
    game->account.accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->account.accountDeleteFromAuth = true;
    std::snprintf(game->account.accountDeleteTargetName, sizeof(game->account.accountDeleteTargetName), "%s", game->account.authUsername);
}

static void CloseAccountDeleteConfirm(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.accountDeleteConfirmOpen = false;
    game->account.accountDeleteConfirmSelection = ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL;
    game->account.accountDeleteFromAuth = false;
    game->account.accountDeleteTargetName[0] = '\0';
}

static void MoveAuthFieldSelection(Game *game, int delta) {
    int selection;
    const int authFieldCount = 7;

    if (game == NULL) {
        return;
    }

    selection = (int)game->account.authSelectedField + delta;
    for (;;) {
        while (selection < AUTH_FIELD_USERNAME) {
            selection += authFieldCount;
        }
        while (selection > AUTH_FIELD_EXIT_GAME) {
            selection -= authFieldCount;
        }

        if (selection != AUTH_FIELD_DELETE_ACCOUNT || IsAuthDeleteEnabled(game)) {
            game->account.authSelectedField = (AuthField)selection;
            return;
        }

        selection += delta >= 0 ? 1 : -1;
    }
}

static void TogglePasswordVisibility(Game *game) {
    if (game == NULL) {
        return;
    }

    game->account.authPasswordVisible = !game->account.authPasswordVisible;
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
    if (game->account.authSelectedField == AUTH_FIELD_USERNAME && game->account.authMode == AUTH_SCREEN_MODE_REGISTER) {
        buffer = game->account.authUsername;
        bufferSize = sizeof(game->account.authUsername);
    } else if (game->account.authSelectedField == AUTH_FIELD_PASSWORD) {
        buffer = game->account.authPassword;
        bufferSize = sizeof(game->account.authPassword);
    }

    if (buffer == NULL) {
        return;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        BackspaceAuthBuffer(buffer);
    }

    character = GetCharPressed();
    while (character > 0) {
        if (game->account.authSelectedField == AUTH_FIELD_USERNAME) {
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

    success = game->account.authMode == AUTH_SCREEN_MODE_REGISTER
        ? SaveSystem_Register(game->account.authUsername, game->account.authPassword, game->account.authMessage, sizeof(game->account.authMessage))
        : SaveSystem_Login(game->account.authUsername, game->account.authPassword, game->account.authMessage, sizeof(game->account.authMessage));
    if (!success) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
        SetAuthenticatedState(game, false);
        return;
    }

    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
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
    deleted = SaveSystem_DeleteActiveAccount(game->account.authMessage, sizeof(game->account.authMessage));
    if (!deleted) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
        CloseAccountDeleteConfirm(game);
        SetAuthenticatedState(game, SaveSystem_IsAccountAuthenticated());
        return;
    }

    if (std::strcmp(game->services.settings.lastUsername, deletedAccountName) == 0) {
        game->services.settings.lastUsername[0] = '\0';
        game->ui.settingsDirty = true;
        Game_TrySaveSettings(game);
    }

    std::snprintf(deleteMessage, sizeof(deleteMessage), "%s", game->account.authMessage);
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    CloseAccountDeleteConfirm(game);
    SetAuthenticatedState(game, false);
    game->account.authMode = game->account.authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    ResetAuthScreen(game, false);
    std::snprintf(game->account.authMessage, sizeof(game->account.authMessage), "%s", deleteMessage);
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

    std::snprintf(deletedAccountName, sizeof(deletedAccountName), "%s", game->account.authUsername);
    deleted = SaveSystem_DeleteAccount(game->account.authUsername, game->account.authPassword, game->account.authMessage, sizeof(game->account.authMessage));
    if (!deleted) {
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
        CloseAccountDeleteConfirm(game);
        SetAuthenticatedState(game, SaveSystem_IsAccountAuthenticated());
        return;
    }

    if (std::strcmp(game->services.settings.lastUsername, deletedAccountName) == 0) {
        game->services.settings.lastUsername[0] = '\0';
        game->ui.settingsDirty = true;
        Game_TrySaveSettings(game);
    }

    std::snprintf(deleteMessage, sizeof(deleteMessage), "%s", game->account.authMessage);
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    CloseAccountDeleteConfirm(game);
    SetAuthenticatedState(game, false);
    game->account.authMode = game->account.authHasAccounts ? AUTH_SCREEN_MODE_LOGIN : AUTH_SCREEN_MODE_REGISTER;
    ResetAuthScreen(game, false);
    std::snprintf(game->account.authMessage, sizeof(game->account.authMessage), "%s", deleteMessage);
    Game_RefreshSaveSlots(game);
}

void GameOverlay_UpdateAccountDeleteConfirm(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    bool activateSelection;

    activateSelection = false;

    if (IsKeyPressed(KEY_ESCAPE)) {
        CloseAccountDeleteConfirm(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        game->account.accountDeleteConfirmSelection = (game->account.accountDeleteConfirmSelection + ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT - 1) % ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }
    if (GameOverlay_IsForwardNavigationPressed()) {
        game->account.accountDeleteConfirmSelection = (game->account.accountDeleteConfirmSelection + 1) % ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = GameOverlay_FindClickedIndexedRect(mouse, ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT, UI_GetAccountDeleteConfirmButtonRect);
        if (buttonIndex >= 0) {
            game->account.accountDeleteConfirmSelection = buttonIndex;
            activateSelection = true;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        activateSelection = true;
    }

    if (!activateSelection) {
        return;
    }

    if (game->account.accountDeleteConfirmSelection == ACCOUNT_DELETE_CONFIRM_BUTTON_DELETE) {
        if (game->account.accountDeleteFromAuth) {
            DeleteAuthSelectedAccount(game);
        } else {
            DeleteActiveAccount(game);
        }
        return;
    }

    CloseAccountDeleteConfirm(game);
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
}

static void MoveAuthAccountPickerSelection(Game *game, int delta) {
    int selection;

    if (game == NULL || game->account.authAccountCount <= 0) {
        return;
    }

    selection = game->account.authAccountPickerSelection + delta;
    if (selection < 0) {
        selection = 0;
    }
    if (selection >= game->account.authAccountCount) {
        selection = game->account.authAccountCount - 1;
    }

    if (selection != game->account.authAccountPickerSelection) {
        game->account.authAccountPickerSelection = selection;
        KeepAuthAccountPickerSelectionVisible(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }
}

static void UpdateAuthAccountPicker(Game *game) {
    Vector2 mouse;
    int visibleIndex;
    int visibleCount;
    float wheelMove;

    if (game == NULL || !game->account.authAccountPickerOpen) {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        CloseAuthAccountPicker(game);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        MoveAuthAccountPickerSelection(game, -1);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        MoveAuthAccountPickerSelection(game, 1);
    }

    wheelMove = GetMouseWheelMove();
    if (wheelMove > 0.0f) {
        MoveAuthAccountPickerSelection(game, -1);
    } else if (wheelMove < 0.0f) {
        MoveAuthAccountPickerSelection(game, 1);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        visibleCount = UI_GetAuthAccountPickerVisibleRowCount();
        for (visibleIndex = 0; visibleIndex < visibleCount; ++visibleIndex) {
            const int accountIndex = game->account.authAccountPickerFirstVisible + visibleIndex;

            if (accountIndex >= game->account.authAccountCount) {
                break;
            }
            if (CheckCollisionPointRec(mouse, UI_GetAuthAccountPickerRowRect(GetScreenWidth(), GetScreenHeight(), visibleIndex))) {
                ChooseAuthAccount(game, accountIndex);
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
                return;
            }
        }

        if (!CheckCollisionPointRec(mouse, UI_GetAuthAccountPickerPanelRect(GetScreenWidth(), GetScreenHeight()))) {
            CloseAuthAccountPicker(game);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        }
        return;
    }

    if (GameOverlay_IsConfirmPressed()) {
        ChooseAuthAccount(game, game->account.authAccountPickerSelection);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
    }
}

void GameOverlay_UpdateAuthScreen(Game *game) {
    Vector2 mouse;
    Rectangle usernameRect;
    Rectangle passwordRect;
    Rectangle togglePasswordRect;
    Rectangle deleteRect;
    Rectangle submitRect;
    Rectangle switchRect;
    Rectangle exitRect;
    bool editingTextField;

    if (game->account.authMode == AUTH_SCREEN_MODE_LOGIN && game->account.authHasAccounts && game->account.authAccountCount <= 0) {
        RefreshAuthAccountList(game);
    }

    if (game->account.authAccountPickerOpen) {
        UpdateAuthAccountPicker(game);
        return;
    }

    usernameRect = UI_GetAuthInputRect(GetScreenWidth(), GetScreenHeight(), 0);
    passwordRect = UI_GetAuthInputRect(GetScreenWidth(), GetScreenHeight(), 1);
    togglePasswordRect = UI_GetAuthPasswordToggleRect(GetScreenWidth(), GetScreenHeight());
    deleteRect = UI_GetAuthDeleteAccountRect(GetScreenWidth(), GetScreenHeight());
    submitRect = UI_GetAuthSubmitButtonRect(GetScreenWidth(), GetScreenHeight());
    switchRect = UI_GetAuthSwitchModeRect(GetScreenWidth(), GetScreenHeight());
    exitRect = UI_GetAuthExitButtonRect(GetScreenWidth(), GetScreenHeight());
    editingTextField = game->account.authSelectedField == AUTH_FIELD_USERNAME || game->account.authSelectedField == AUTH_FIELD_PASSWORD;

    if (IsKeyPressed(KEY_TAB)) {
        MoveAuthFieldSelection(game, (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ? -1 : 1);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }
    if (IsKeyPressed(KEY_DOWN) || (!editingTextField && IsKeyPressed(KEY_S))) {
        MoveAuthFieldSelection(game, 1);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }
    if (IsKeyPressed(KEY_UP) || (!editingTextField && IsKeyPressed(KEY_W))) {
        MoveAuthFieldSelection(game, -1);
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    HandleAuthTextInput(game);

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        if (CheckCollisionPointRec(mouse, togglePasswordRect)) {
            game->account.authSelectedField = AUTH_FIELD_TOGGLE_PASSWORD;
            TogglePasswordVisibility(game);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        }
        if (CheckCollisionPointRec(mouse, usernameRect)) {
            game->account.authSelectedField = AUTH_FIELD_USERNAME;
            if (game->account.authMode == AUTH_SCREEN_MODE_LOGIN && game->account.authHasAccounts) {
                OpenAuthAccountPicker(game);
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            }
            return;
        }
        if (CheckCollisionPointRec(mouse, passwordRect)) {
            game->account.authSelectedField = AUTH_FIELD_PASSWORD;
            return;
        }
        if (CheckCollisionPointRec(mouse, deleteRect) && IsAuthDeleteEnabled(game)) {
            game->account.authSelectedField = AUTH_FIELD_DELETE_ACCOUNT;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
            OpenAuthAccountDeleteConfirm(game);
            return;
        }
        if (CheckCollisionPointRec(mouse, submitRect)) {
            game->account.authSelectedField = AUTH_FIELD_SUBMIT;
            SubmitAuth(game);
            return;
        }
        if (CheckCollisionPointRec(mouse, switchRect)) {
            game->account.authSelectedField = AUTH_FIELD_SWITCH_MODE;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_TOGGLE_AUTH_MODE, -1);
            return;
        }
        if (CheckCollisionPointRec(mouse, exitRect)) {
            game->account.authSelectedField = AUTH_FIELD_EXIT_GAME;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
            game->flow.requestClose = true;
            return;
        }
    }

    if (!GameOverlay_IsConfirmPressed()) {
        return;
    }

    switch (game->account.authSelectedField) {
        case AUTH_FIELD_USERNAME:
            if (game->account.authMode == AUTH_SCREEN_MODE_LOGIN && game->account.authHasAccounts) {
                OpenAuthAccountPicker(game);
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
                return;
            }
            game->account.authSelectedField = AUTH_FIELD_PASSWORD;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        case AUTH_FIELD_PASSWORD:
            game->account.authSelectedField = AUTH_FIELD_TOGGLE_PASSWORD;
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        case AUTH_FIELD_TOGGLE_PASSWORD:
            TogglePasswordVisibility(game);
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            return;
        case AUTH_FIELD_DELETE_ACCOUNT:
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
            OpenAuthAccountDeleteConfirm(game);
            return;
        case AUTH_FIELD_SUBMIT:
            SubmitAuth(game);
            return;
        case AUTH_FIELD_SWITCH_MODE:
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
            Game_BeginScreenTransition(game, SCREEN_TRANSITION_TOGGLE_AUTH_MODE, -1);
            return;
        case AUTH_FIELD_EXIT_GAME:
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
            game->flow.requestClose = true;
            return;
        default:
            break;
    }
}
