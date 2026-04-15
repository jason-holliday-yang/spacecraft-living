#include "ui_system.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "ui_layout_smoke failed: %s\n", message);
    exit(1);
}

static bool RectContainsRect(Rectangle outer, Rectangle inner) {
    return inner.x >= outer.x
        && inner.y >= outer.y
        && inner.x + inner.width <= outer.x + outer.width
        && inner.y + inner.height <= outer.y + outer.height;
}

static float GetLayoutScale(int screenWidth, int screenHeight) {
    float widthScale;
    float heightScale;
    float scale;

    widthScale = (float)screenWidth / 1440.0f;
    heightScale = (float)screenHeight / 900.0f;
    scale = widthScale < heightScale ? widthScale : heightScale;
    if (scale < 0.84f) {
        return 0.84f;
    }
    if (scale > 1.25f) {
        return 1.25f;
    }
    return scale;
}

static void VerifySettingsLayout(int screenWidth, int screenHeight) {
    Rectangle overlay;
    Rectangle slider;
    Rectangle decreaseButton;
    Rectangle increaseButton;
    Rectangle closeButton;
    Rectangle languageEnglishButton;
    Rectangle languageChineseButton;
    Rectangle exitButton;

    overlay = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    slider = UI_GetSettingsSliderRect(screenWidth, screenHeight);
    decreaseButton = UI_GetSettingsDecreaseButtonRect(screenWidth, screenHeight);
    increaseButton = UI_GetSettingsIncreaseButtonRect(screenWidth, screenHeight);
    closeButton = UI_GetSettingsCloseButtonRect(screenWidth, screenHeight);
    languageEnglishButton = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 0);
    languageChineseButton = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 1);
    exitButton = UI_GetMainMenuButtonRect(screenWidth, screenHeight, MAIN_MENU_BUTTON_EXIT);

    Require(overlay.width > 0.0f && overlay.height > 0.0f, "settings overlay should have a positive size");
    Require(RectContainsRect(overlay, slider), "settings slider should stay inside the overlay");
    Require(RectContainsRect(overlay, decreaseButton), "settings decrease button should stay inside the overlay");
    Require(RectContainsRect(overlay, increaseButton), "settings increase button should stay inside the overlay");
    Require(RectContainsRect(overlay, closeButton), "settings close button should stay inside the overlay");
    Require(RectContainsRect(overlay, languageEnglishButton), "settings english language button should stay inside the overlay");
    Require(RectContainsRect(overlay, languageChineseButton), "settings chinese language button should stay inside the overlay");
    Require(!CheckCollisionRecs(closeButton, slider), "settings close button should not overlap the slider");
    Require(!CheckCollisionRecs(closeButton, decreaseButton), "settings close button should not overlap the decrease button");
    Require(!CheckCollisionRecs(closeButton, increaseButton), "settings close button should not overlap the increase button");
    Require(!CheckCollisionRecs(closeButton, languageEnglishButton), "settings close button should not overlap the english language button");
    Require(!CheckCollisionRecs(closeButton, languageChineseButton), "settings close button should not overlap the chinese language button");
    Require(!CheckCollisionRecs(closeButton, exitButton), "settings close button should not overlap the main menu exit button");
    Require(!CheckCollisionRecs(languageEnglishButton, languageChineseButton), "settings language buttons should not overlap");
}

static void VerifyPauseMenuLayout(int screenWidth, int screenHeight) {
    Rectangle panel;
    int buttonIndex;

    panel = UI_GetPauseMenuPanelRect(screenWidth, screenHeight);
    Require(panel.width > 0.0f && panel.height > 0.0f, "pause menu panel should have a positive size");

    for (buttonIndex = 0; buttonIndex < PAUSE_MENU_BUTTON_COUNT; ++buttonIndex) {
        Rectangle button;

        button = UI_GetPauseMenuButtonRect(screenWidth, screenHeight, buttonIndex);
        Require(button.width > 0.0f && button.height > 0.0f, "pause menu buttons should have a positive size");
        Require(RectContainsRect(panel, button), "pause menu buttons should stay inside the pause panel");
        if (buttonIndex > 0) {
            Rectangle previousButton;

            previousButton = UI_GetPauseMenuButtonRect(screenWidth, screenHeight, buttonIndex - 1);
            Require(!CheckCollisionRecs(previousButton, button), "pause menu buttons should not overlap");
        }
    }
}

static void VerifyHudShortcutLayout(int screenWidth, int screenHeight) {
    Rectangle first;
    Rectangle second;

    first = UI_GetHudShortcutRect(screenWidth, screenHeight, 0);
    second = UI_GetHudShortcutRect(screenWidth, screenHeight, 1);

    Require(first.width > 0.0f && first.height > 0.0f, "first hud shortcut should have a positive size");
    Require(second.width > 0.0f && second.height > 0.0f, "second hud shortcut should have a positive size");
    Require(!CheckCollisionRecs(first, second), "first and second hud shortcuts should not overlap");
}

static void VerifySettlementConfirmLayout(int screenWidth, int screenHeight) {
    Rectangle panel;
    Rectangle heroicButton;
    Rectangle peacefulButton;
    Rectangle settlementButton;
    Rectangle cancelButton;

    panel = UI_GetSettlementConfirmPanelRect(screenWidth, screenHeight);
    heroicButton = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, SETTLEMENT_CONFIRM_BUTTON_HEROIC);
    peacefulButton = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, SETTLEMENT_CONFIRM_BUTTON_PEACEFUL);
    settlementButton = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, SETTLEMENT_CONFIRM_BUTTON_SETTLEMENT);
    cancelButton = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, SETTLEMENT_CONFIRM_BUTTON_CANCEL);

    Require(panel.width > 0.0f && panel.height > 0.0f, "settlement confirm panel should have a positive size");
    Require(RectContainsRect(panel, heroicButton), "heroic button should stay inside the panel");
    Require(RectContainsRect(panel, peacefulButton), "peaceful button should stay inside the panel");
    Require(RectContainsRect(panel, settlementButton), "settlement button should stay inside the panel");
    Require(RectContainsRect(panel, cancelButton), "settlement cancel button should stay inside the panel");
    Require(!CheckCollisionRecs(heroicButton, peacefulButton), "ending choice buttons should not overlap");
    Require(!CheckCollisionRecs(heroicButton, settlementButton), "ending choice buttons should not overlap");
    Require(!CheckCollisionRecs(heroicButton, cancelButton), "ending choice buttons should not overlap");
    Require(!CheckCollisionRecs(peacefulButton, settlementButton), "ending choice buttons should not overlap");
    Require(!CheckCollisionRecs(peacefulButton, cancelButton), "ending choice buttons should not overlap");
    Require(!CheckCollisionRecs(settlementButton, cancelButton), "ending choice buttons should not overlap");
}

static void VerifyAccountDeleteConfirmLayout(int screenWidth, int screenHeight) {
    Rectangle panel;
    Rectangle cancelButton;
    Rectangle deleteButton;

    panel = UI_GetAccountDeleteConfirmPanelRect(screenWidth, screenHeight);
    cancelButton = UI_GetAccountDeleteConfirmButtonRect(screenWidth, screenHeight, ACCOUNT_DELETE_CONFIRM_BUTTON_CANCEL);
    deleteButton = UI_GetAccountDeleteConfirmButtonRect(screenWidth, screenHeight, ACCOUNT_DELETE_CONFIRM_BUTTON_DELETE);

    Require(panel.width > 0.0f && panel.height > 0.0f, "account delete confirm panel should have a positive size");
    Require(RectContainsRect(panel, cancelButton), "account delete cancel button should stay inside the panel");
    Require(RectContainsRect(panel, deleteButton), "account delete confirm button should stay inside the panel");
    Require(!CheckCollisionRecs(cancelButton, deleteButton), "account delete confirm buttons should not overlap");
}

static void VerifyAuthLayout(int screenWidth, int screenHeight) {
    Rectangle panel;
    Rectangle usernameField;
    Rectangle passwordField;
    Rectangle passwordToggle;
    Rectangle deleteButton;
    Rectangle submitButton;
    Rectangle switchButton;
    Rectangle switchAccountButton;
    Rectangle deleteAccountButton;

    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    usernameField = UI_GetAuthInputRect(screenWidth, screenHeight, 0);
    passwordField = UI_GetAuthInputRect(screenWidth, screenHeight, 1);
    passwordToggle = UI_GetAuthPasswordToggleRect(screenWidth, screenHeight);
    deleteButton = UI_GetAuthDeleteAccountRect(screenWidth, screenHeight);
    submitButton = UI_GetAuthSubmitButtonRect(screenWidth, screenHeight);
    switchButton = UI_GetAuthSwitchModeRect(screenWidth, screenHeight);
    switchAccountButton = UI_GetMainMenuSwitchAccountRect(screenWidth, screenHeight);
    deleteAccountButton = UI_GetMainMenuDeleteAccountRect(screenWidth, screenHeight);

    Require(panel.width > 0.0f && panel.height > 0.0f, "auth panel should have a positive size");
    Require(RectContainsRect(panel, usernameField), "username field should stay inside the auth panel");
    Require(RectContainsRect(panel, passwordField), "password field should stay inside the auth panel");
    Require(RectContainsRect(passwordField, passwordToggle), "password toggle should stay inside the password field");
    Require(RectContainsRect(panel, deleteButton), "delete account button should stay inside the auth panel");
    Require(RectContainsRect(panel, submitButton), "submit button should stay inside the auth panel");
    Require(RectContainsRect(panel, switchButton), "switch mode button should stay inside the auth panel");
    Require(!CheckCollisionRecs(usernameField, passwordField), "auth fields should not overlap");
    Require(!CheckCollisionRecs(submitButton, deleteButton), "submit and delete buttons should not overlap");
    Require(!CheckCollisionRecs(deleteButton, switchButton), "delete and switch mode buttons should not overlap");
    Require(!CheckCollisionRecs(submitButton, switchButton), "auth buttons should not overlap");
    Require(!CheckCollisionRecs(switchAccountButton, deleteAccountButton), "main menu account action buttons should not overlap");
}

static void VerifySavePanelLayout(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle detailsPanel;
    Rectangle primaryButton;
    Rectangle deleteButton;
    Rectangle firstSlot;
    Rectangle lastSlot;

    scale = GetLayoutScale(screenWidth, screenHeight);
    panel = (Rectangle){
        screenWidth * 0.5f - 520.0f * scale,
        screenHeight * 0.5f - 290.0f * scale,
        1040.0f * scale,
        580.0f * scale
    };
    detailsPanel = (Rectangle){
        panel.x + 724.0f * scale,
        panel.y + 108.0f * scale,
        286.0f * scale,
        386.0f * scale
    };
    primaryButton = UI_GetSavePrimaryButtonRect(screenWidth, screenHeight);
    deleteButton = UI_GetSaveDeleteButtonRect(screenWidth, screenHeight);
    firstSlot = UI_GetSaveSlotRect(screenWidth, screenHeight, 0);
    lastSlot = UI_GetSaveSlotRect(screenWidth, screenHeight, SAVE_SLOT_COUNT - 1);

    Require(panel.width > 0.0f && panel.height > 0.0f, "save panel should have a positive size");
    Require(RectContainsRect(panel, firstSlot), "first save slot should stay inside the save panel");
    Require(RectContainsRect(panel, lastSlot), "last save slot should stay inside the save panel");
    Require(RectContainsRect(panel, detailsPanel), "save details panel should stay inside the save panel");
    Require(RectContainsRect(panel, primaryButton), "save primary button should stay inside the save panel");
    Require(RectContainsRect(panel, deleteButton), "save delete button should stay inside the save panel");
    Require(!CheckCollisionRecs(primaryButton, deleteButton), "save panel action buttons should not overlap");
    Require(!CheckCollisionRecs(firstSlot, detailsPanel), "save list and details panel should not overlap");
}

int main(void) {
    const int screenWidth = 1440;
    const int screenHeight = 900;
    int buttonIndex;
    Rectangle previousRect;

    previousRect = UI_GetMainMenuButtonRect(screenWidth, screenHeight, 0);
    Require(previousRect.width > 0.0f && previousRect.height > 0.0f,
            "main menu button rect should have a positive size");
    Require(UI_GetMainMenuButtonIndexAtPoint(screenWidth,
                                             screenHeight,
                                             (Vector2){previousRect.x + previousRect.width * 0.5f, previousRect.y + previousRect.height * 0.5f}) == 0,
            "start button center should map back to the start button hit zone");

    for (buttonIndex = 1; buttonIndex < MAIN_MENU_BUTTON_COUNT; buttonIndex++) {
        Rectangle rect;
        Vector2 center;

        rect = UI_GetMainMenuButtonRect(screenWidth, screenHeight, buttonIndex);
        center = (Vector2){rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f};
        Require(rect.width > 0.0f && rect.height > 0.0f,
                "every main menu button rect should have a positive size");
        Require(rect.y >= previousRect.y + previousRect.height,
                "main menu buttons should not overlap vertically");
        Require(UI_GetMainMenuButtonIndexAtPoint(screenWidth, screenHeight, center) == buttonIndex,
                "each main menu button center should map back to its own hit zone");
        previousRect = rect;
    }

    Require(UI_GetMainMenuButtonIndexAtPoint(screenWidth, screenHeight, (Vector2){32.0f, 32.0f}) == -1,
            "points away from the menu should not match any main menu button");

    VerifySettingsLayout(1440, 900);
    VerifySettingsLayout(1280, 720);
    VerifySettingsLayout(1024, 768);
    VerifyPauseMenuLayout(1440, 900);
    VerifyPauseMenuLayout(1280, 720);
    VerifyPauseMenuLayout(1024, 768);
    VerifyHudShortcutLayout(1440, 900);
    VerifyHudShortcutLayout(1280, 720);
    VerifyHudShortcutLayout(1024, 768);
    VerifyAuthLayout(1440, 900);
    VerifyAuthLayout(1280, 720);
    VerifyAuthLayout(1024, 768);
    VerifySavePanelLayout(1440, 900);
    VerifySavePanelLayout(1280, 720);
    VerifySavePanelLayout(1024, 768);
    VerifyAccountDeleteConfirmLayout(1440, 900);
    VerifyAccountDeleteConfirmLayout(1280, 720);
    VerifyAccountDeleteConfirmLayout(1024, 768);
    VerifySettlementConfirmLayout(1440, 900);
    VerifySettlementConfirmLayout(1280, 720);
    VerifySettlementConfirmLayout(1024, 768);

    puts("ui_layout smoke ok");
    return 0;
}
