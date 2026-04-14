#include "ui_system.h"

#include "ui_runtime_internal.h"

Rectangle UI_GetMainMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    float width;
    float height;
    float startY;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    width = 308.0f * scale;
    height = 56.0f * scale;
    startY = screenHeight * 0.50f;

    return Rectangle{
        screenWidth * 0.5f - width * 0.5f,
        startY + buttonIndex * (height + 18.0f * scale),
        width,
        height
    };
}

int UI_GetMainMenuButtonIndexAtPoint(int screenWidth, int screenHeight, Vector2 point) {
    int buttonIndex;

    for (buttonIndex = 0; buttonIndex < MAIN_MENU_BUTTON_COUNT; buttonIndex++) {
        if (CheckCollisionPointRec(point, UI_GetMainMenuButtonRect(screenWidth, screenHeight, buttonIndex))) {
            return buttonIndex;
        }
    }

    return -1;
}

Rectangle UI_GetMainMenuSwitchAccountRect(int screenWidth, int screenHeight) {
    float scale;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    return Rectangle{
        screenWidth * 0.5f + 38.0f * scale,
        122.0f * scale,
        200.0f * scale,
        44.0f * scale
    };
}

Rectangle UI_GetMainMenuDeleteAccountRect(int screenWidth, int screenHeight) {
    float scale;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    return Rectangle{
        screenWidth * 0.5f + 250.0f * scale,
        122.0f * scale,
        200.0f * scale,
        44.0f * scale
    };
}

Rectangle UI_GetAuthPanelRect(int screenWidth, int screenHeight) {
    float scale;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    return Rectangle{
        screenWidth * 0.5f - 440.0f * scale,
        screenHeight * 0.5f - 248.0f * scale,
        880.0f * scale,
        496.0f * scale
    };
}

Rectangle UI_GetAuthInputRect(int screenWidth, int screenHeight, int fieldIndex) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + 52.0f * scale,
        panel.y + 170.0f * scale + fieldIndex * 104.0f * scale,
        panel.width - 104.0f * scale,
        60.0f * scale
    };
}

Rectangle UI_GetAuthPasswordToggleRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle passwordRect;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    passwordRect = UI_GetAuthInputRect(screenWidth, screenHeight, 1);
    return Rectangle{
        passwordRect.x + passwordRect.width - 136.0f * scale,
        passwordRect.y + 11.0f * scale,
        118.0f * scale,
        38.0f * scale
    };
}

Rectangle UI_GetAuthDeleteAccountRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + panel.width * 0.5f - 114.0f * scale,
        panel.y + panel.height - 118.0f * scale,
        228.0f * scale,
        56.0f * scale
    };
}

Rectangle UI_GetAuthSubmitButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + 52.0f * scale,
        panel.y + panel.height - 118.0f * scale,
        228.0f * scale,
        56.0f * scale
    };
}

Rectangle UI_GetAuthSwitchModeRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + panel.width - 280.0f * scale,
        panel.y + panel.height - 118.0f * scale,
        228.0f * scale,
        56.0f * scale
    };
}

Rectangle UI_GetPauseMenuPanelRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle firstButton;
    Rectangle lastButton;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    firstButton = UI_GetPauseMenuButtonRect(screenWidth, screenHeight, 0);
    lastButton = UI_GetPauseMenuButtonRect(screenWidth, screenHeight, PAUSE_MENU_BUTTON_COUNT - 1);

    return Rectangle{
        firstButton.x - 34.0f * scale,
        firstButton.y - 34.0f * scale,
        firstButton.width + 68.0f * scale,
        (lastButton.y + lastButton.height - firstButton.y) + 68.0f * scale
    };
}

Rectangle UI_GetPauseMenuButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    float width;
    float height;
    float startY;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    width = 292.0f * scale;
    height = 52.0f * scale;
    startY = screenHeight * 0.5f - 116.0f * scale;

    return Rectangle{
        screenWidth * 0.5f - width * 0.5f,
        startY + buttonIndex * (height + 16.0f * scale),
        width,
        height
    };
}

Rectangle UI_GetStandardOverlayRect(int screenWidth, int screenHeight) {
    float scale;
    float width;
    float height;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    width = 1040.0f * scale;
    height = 620.0f * scale;

    return Rectangle{
        screenWidth * 0.5f - width * 0.5f,
        screenHeight * 0.5f - height * 0.5f,
        width,
        height
    };
}

Rectangle UI_GetDeathPopupButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    float width;
    float height;
    float startX;
    float y;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    width = 200.0f * scale;
    height = 52.0f * scale;
    startX = screenWidth * 0.5f - width - 12.0f * scale;
    y = screenHeight * 0.5f + 60.0f * scale;

    return Rectangle{
        startX + buttonIndex * (width + 24.0f * scale),
        y,
        width,
        height
    };
}

Rectangle UI_GetSaveSlotRect(int screenWidth, int screenHeight, int slotIndex) {
    float scale;
    float panelX;
    float panelY;
    float slotWidth;
    float slotHeight;
    float gap;
    int column;
    int row;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panelX = screenWidth * 0.5f - 520.0f * scale;
    panelY = screenHeight * 0.5f - 290.0f * scale;
    slotWidth = 162.0f * scale;
    slotHeight = 88.0f * scale;
    gap = 16.0f * scale;
    column = slotIndex % 4;
    row = slotIndex / 4;

    return Rectangle{
        panelX + 28.0f * scale + column * (slotWidth + gap),
        panelY + 108.0f * scale + row * (slotHeight + gap),
        slotWidth,
        slotHeight
    };
}

Rectangle UI_GetSaveDeleteButtonRect(int screenWidth, int screenHeight) {
    float scale;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    return Rectangle{
        screenWidth * 0.5f + 248.0f * scale,
        screenHeight * 0.5f + 206.0f * scale,
        220.0f * scale,
        50.0f * scale
    };
}

Rectangle UI_GetBackpackSlotRect(int screenWidth, int screenHeight, int itemIndex) {
    float scale;
    Rectangle panel;
    float slotWidth;
    float slotHeight;
    float gap;
    int column;
    int row;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    slotWidth = 149.0f * scale;
    slotHeight = 72.0f * scale;
    gap = 12.0f * scale;
    column = itemIndex % UI_BACKPACK_SLOT_COLUMNS;
    row = itemIndex / UI_BACKPACK_SLOT_COLUMNS;

    return Rectangle{
        panel.x + 32.0f * scale + column * (slotWidth + gap),
        panel.y + 118.0f * scale + row * (slotHeight + 10.0f * scale),
        slotWidth,
        slotHeight
    };
}

Rectangle UI_GetCraftSlotRect(int screenWidth, int screenHeight, int itemIndex) {
    float scale;
    Rectangle panel;
    float slotWidth;
    float slotHeight;
    float gap;
    int column;
    int row;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    slotWidth = 290.0f * scale;
    slotHeight = 96.0f * scale;
    gap = 14.0f * scale;
    column = itemIndex % UI_CRAFT_SLOT_COLUMNS;
    row = itemIndex / UI_CRAFT_SLOT_COLUMNS;

    return Rectangle{
        panel.x + 32.0f * scale + column * (slotWidth + gap),
        panel.y + 118.0f * scale + row * (slotHeight + 12.0f * scale),
        slotWidth,
        slotHeight
    };
}

Rectangle UI_GetCraftActionButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + panel.width - 322.0f * scale,
        panel.y + panel.height - 82.0f * scale,
        290.0f * scale,
        50.0f * scale
    };
}

Rectangle UI_GetLogEntryRect(int screenWidth, int screenHeight, int entryIndex) {
    float scale;
    Rectangle panel;
    float rowHeight;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    rowHeight = 56.0f * scale;

    return Rectangle{
        panel.x + 32.0f * scale,
        panel.y + 150.0f * scale + entryIndex * (rowHeight + 10.0f * scale),
        258.0f * scale,
        rowHeight
    };
}

Rectangle UI_GetSettingsSliderRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + 52.0f * scale,
        panel.y + 246.0f * scale,
        panel.width - 104.0f * scale,
        16.0f * scale
    };
}

Rectangle UI_GetSettingsDecreaseButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle slider;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    slider = UI_GetSettingsSliderRect(screenWidth, screenHeight);
    return Rectangle{
        slider.x,
        slider.y + 40.0f * scale,
        72.0f * scale,
        44.0f * scale
    };
}

Rectangle UI_GetSettingsIncreaseButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle slider;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    slider = UI_GetSettingsSliderRect(screenWidth, screenHeight);
    return Rectangle{
        slider.x + slider.width - 72.0f * scale,
        slider.y + 40.0f * scale,
        72.0f * scale,
        44.0f * scale
    };
}

Rectangle UI_GetSettingsCloseButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + panel.width - 186.0f * scale,
        panel.y + 24.0f * scale,
        154.0f * scale,
        44.0f * scale
    };
}

Rectangle UI_GetSettingsLanguageButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    Rectangle panel;
    float width;
    float gap;
    float x;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    width = 180.0f * scale;
    gap = 16.0f * scale;
    x = panel.x + panel.width - 34.0f * scale - width * 2.0f - gap;
    return Rectangle{
        x + buttonIndex * (width + gap),
        panel.y + 442.0f * scale,
        width,
        48.0f * scale
    };
}

Rectangle UI_GetHudShortcutRect(int screenWidth, int screenHeight, int shortcutIndex) {
    float scale;
    Rectangle shortcutsPanel;
    float width;
    float gap;
    float totalWidth;
    float startX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    shortcutsPanel = Rectangle{
        screenWidth - 334.0f * scale,
        screenHeight - 130.0f * scale,
        316.0f * scale,
        106.0f * scale
    };
    width = 128.0f * scale;
    gap = 16.0f * scale;
    totalWidth = width * HUD_SHORTCUT_COUNT + gap * (HUD_SHORTCUT_COUNT - 1);
    startX = shortcutsPanel.x + (shortcutsPanel.width - totalWidth) * 0.5f;

    return Rectangle{
        startX + shortcutIndex * (width + gap),
        shortcutsPanel.y + 12.0f * scale,
        width,
        shortcutsPanel.height - 24.0f * scale
    };
}

Rectangle UI_GetSettlementConfirmPanelRect(int screenWidth, int screenHeight) {
    float scale;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    return Rectangle{
        screenWidth * 0.5f - 330.0f * scale,
        screenHeight * 0.5f - 150.0f * scale,
        660.0f * scale,
        300.0f * scale
    };
}

Rectangle UI_GetSettlementConfirmButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    Rectangle panel;
    float width;
    float gap;
    float startX;
    float y;
    float totalWidth;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetSettlementConfirmPanelRect(screenWidth, screenHeight);
    width = 136.0f * scale;
    gap = 16.0f * scale;
    totalWidth = width * SETTLEMENT_CONFIRM_BUTTON_COUNT + gap * (SETTLEMENT_CONFIRM_BUTTON_COUNT - 1);
    startX = panel.x + panel.width * 0.5f - totalWidth * 0.5f;
    y = panel.y + panel.height - 78.0f * scale;

    return Rectangle{
        startX + buttonIndex * (width + gap),
        y,
        width,
        48.0f * scale
    };
}

Rectangle UI_GetAccountDeleteConfirmPanelRect(int screenWidth, int screenHeight) {
    float scale;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    return Rectangle{
        screenWidth * 0.5f - 340.0f * scale,
        screenHeight * 0.5f - 158.0f * scale,
        680.0f * scale,
        316.0f * scale
    };
}

Rectangle UI_GetAccountDeleteConfirmButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    Rectangle panel;
    float width;
    float gap;
    float startX;
    float y;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAccountDeleteConfirmPanelRect(screenWidth, screenHeight);
    width = 214.0f * scale;
    gap = 24.0f * scale;
    startX = panel.x + panel.width * 0.5f - width - gap * 0.5f;
    y = panel.y + panel.height - 76.0f * scale;

    return Rectangle{
        startX + buttonIndex * (width + gap),
        y,
        width,
        48.0f * scale
    };
}
