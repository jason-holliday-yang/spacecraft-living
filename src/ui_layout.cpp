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

Rectangle UI_GetAuthExitButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAuthPanelRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + panel.width - 184.0f * scale,
        panel.y + 34.0f * scale,
        132.0f * scale,
        44.0f * scale
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
    width = screenWidth - 72.0f * scale;
    height = screenHeight - 156.0f * scale;

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
    width = 180.0f * scale;
    height = 52.0f * scale;
    startX = screenWidth * 0.5f - (width * DEATH_POPUP_BUTTON_COUNT + 24.0f * scale * (DEATH_POPUP_BUTTON_COUNT - 1)) * 0.5f;
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
    Rectangle panel;
    float slotWidth;
    float slotHeight;
    float gap;
    float leftInset;
    float topInset;
    int column;
    int row;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = Rectangle{
        screenWidth * 0.5f - 520.0f * scale,
        screenHeight * 0.5f - 290.0f * scale,
        1040.0f * scale,
        580.0f * scale,
    };
    slotWidth = 162.0f * scale;
    slotHeight = 88.0f * scale;
    gap = 16.0f * scale;
    leftInset = 28.0f * scale;
    topInset = 108.0f * scale;
    column = slotIndex % 4;
    row = slotIndex / 4;

    return Rectangle{
        panel.x + leftInset + column * (slotWidth + gap),
        panel.y + topInset + row * (slotHeight + gap),
        slotWidth,
        slotHeight
    };
}

Rectangle UI_GetSavePrimaryButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    float detailInset;
    float detailWidth;
    float buttonWidth;
    float buttonGap;
    float buttonsLeft;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = Rectangle{
        screenWidth * 0.5f - 520.0f * scale,
        screenHeight * 0.5f - 290.0f * scale,
        1040.0f * scale,
        580.0f * scale,
    };
    detailInset = 24.0f * scale;
    detailWidth = 314.0f * scale;
    buttonWidth = 104.0f * scale;
    buttonGap = 12.0f * scale;
    buttonsLeft = panel.x + panel.width - detailInset - detailWidth + (detailWidth - (buttonWidth * 2.0f + buttonGap)) * 0.5f;

    return Rectangle{
        buttonsLeft,
        panel.y + panel.height - 74.0f * scale,
        buttonWidth,
        50.0f * scale
    };
}

Rectangle UI_GetSaveDeleteButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle primaryButton;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    primaryButton = UI_GetSavePrimaryButtonRect(screenWidth, screenHeight);
    return Rectangle{
        primaryButton.x + primaryButton.width + 12.0f * scale,
        primaryButton.y,
        primaryButton.width,
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
    float listWidth;
    float slotWidth;
    float slotHeight;
    float gap;
    float outerGap;
    float leftInset;
    float rightInset;
    int column;
    int row;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    outerGap = 24.0f * scale;
    leftInset = 32.0f * scale;
    rightInset = 20.0f * scale;
    listWidth = (panel.width - 40.0f * scale - outerGap) * (2.0f / 3.0f);
    slotHeight = 96.0f * scale;
    gap = 14.0f * scale;
    slotWidth = (listWidth - leftInset - rightInset - gap) / (float)UI_CRAFT_SLOT_COLUMNS;
    column = itemIndex % UI_CRAFT_SLOT_COLUMNS;
    row = itemIndex / UI_CRAFT_SLOT_COLUMNS;

    return Rectangle{
        panel.x + leftInset + column * (slotWidth + gap),
        panel.y + 118.0f * scale + row * (slotHeight + 12.0f * scale),
        slotWidth,
        slotHeight
    };
}

Rectangle UI_GetCraftActionButtonRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle detailPanel;
    float listWidth;
    float detailWidth;
    float outerGap;
    float buttonWidth;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    outerGap = 24.0f * scale;
    listWidth = (panel.width - 40.0f * scale - outerGap) * (2.0f / 3.0f);
    detailWidth = panel.width - 40.0f * scale - outerGap - listWidth;
    detailPanel = Rectangle{
        panel.x + 20.0f * scale + listWidth + outerGap,
        panel.y + 102.0f * scale,
        detailWidth,
        panel.height - 134.0f * scale
    };
    buttonWidth = detailPanel.width - 52.0f * scale;
    if (buttonWidth > 292.0f * scale) {
        buttonWidth = 292.0f * scale;
    }

    return Rectangle{
        detailPanel.x + detailPanel.width * 0.5f - buttonWidth * 0.5f,
        detailPanel.y + detailPanel.height - 64.0f * scale,
        buttonWidth,
        50.0f * scale
    };
}

Rectangle UI_GetCommunicatorOverlayRect(int screenWidth, int screenHeight) {
    return UI_GetStandardOverlayRect(screenWidth, screenHeight);
}

Rectangle UI_GetCommunicatorTabRect(int screenWidth, int screenHeight, int tabIndex) {
    float scale;
    Rectangle panel;
    float tabWidth;
    float tabHeight;
    float gap;
    const int communicatorTabCount = 3;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetCommunicatorOverlayRect(screenWidth, screenHeight);
    gap = 12.0f * scale;
    tabWidth = (panel.width - 40.0f * scale - gap * (communicatorTabCount - 1)) / (float)communicatorTabCount;
    tabHeight = 52.0f * scale;

    return Rectangle{
        panel.x + 20.0f * scale + tabIndex * (tabWidth + gap),
        panel.y + 78.0f * scale,
        tabWidth,
        tabHeight
    };
}

Rectangle UI_GetInfoOverlayTabRect(int screenWidth, int screenHeight, int tabIndex) {
    float scale;
    Rectangle panel;
    float tabWidth;
    float tabHeight;
    float gap;
    float totalWidth;
    float startX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    tabWidth = 150.0f * scale;
    tabHeight = 36.0f * scale;
    gap = 12.0f * scale;
    totalWidth = tabWidth * INFO_OVERLAY_TAB_COUNT + gap * (INFO_OVERLAY_TAB_COUNT - 1);
    startX = panel.x + panel.width * 0.5f - totalWidth * 0.5f;

    return Rectangle{
        startX + tabIndex * (tabWidth + gap),
        panel.y + 16.0f * scale,
        tabWidth,
        tabHeight
    };
}

Rectangle UI_GetCommunicatorLogListRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetCommunicatorOverlayRect(screenWidth, screenHeight);
    return Rectangle{
        panel.x + 20.0f * scale,
        panel.y + 146.0f * scale,
        258.0f * scale,
        panel.height - 178.0f * scale,
    };
}

Rectangle UI_GetCommunicatorLogContentRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle listPanel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetCommunicatorOverlayRect(screenWidth, screenHeight);
    listPanel = UI_GetCommunicatorLogListRect(screenWidth, screenHeight);
    return Rectangle{
        listPanel.x + listPanel.width + 18.0f * scale,
        listPanel.y,
        panel.x + panel.width - 22.0f * scale - (listPanel.x + listPanel.width + 18.0f * scale),
        listPanel.height,
    };
}

Rectangle UI_GetCommunicatorLogImagePanelRect(int screenWidth, int screenHeight) {
    Rectangle contentPanel;

    contentPanel = UI_GetCommunicatorLogContentRect(screenWidth, screenHeight);
    return contentPanel;
}

Rectangle UI_GetCommunicatorLogImageRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle imagePanel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    imagePanel = UI_GetCommunicatorLogImagePanelRect(screenWidth, screenHeight);
    return Rectangle{
        imagePanel.x + 6.0f * scale,
        imagePanel.y + 6.0f * scale,
        imagePanel.width - 12.0f * scale,
        imagePanel.height - 12.0f * scale
    };
}

Rectangle UI_GetCommunicatorLogDetailOverlayRect(int screenWidth, int screenHeight) {
    float scale;
    Rectangle imagePanel;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    imagePanel = UI_GetCommunicatorLogImagePanelRect(screenWidth, screenHeight);
    return Rectangle{
        imagePanel.x + 24.0f * scale,
        imagePanel.y + 24.0f * scale,
        imagePanel.width - 48.0f * scale,
        imagePanel.height - 48.0f * scale
    };
}

Rectangle UI_GetCommunicatorVisibleLogEntryRect(int screenWidth, int screenHeight, int visibleIndex) {
    float scale;
    Rectangle listPanel;
    float rowHeight;
    float gap;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    listPanel = UI_GetCommunicatorLogListRect(screenWidth, screenHeight);
    rowHeight = 60.0f * scale;
    gap = 9.0f * scale;

    return Rectangle{
        listPanel.x + 14.0f * scale,
        listPanel.y + 14.0f * scale + visibleIndex * (rowHeight + gap),
        listPanel.width - 28.0f * scale,
        rowHeight
    };
}

int UI_GetCommunicatorVisibleLogCount(int screenWidth, int screenHeight) {
    float scale;
    Rectangle listPanel;
    float availableHeight;
    float rowHeight;
    float gap;
    int visibleCount;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    listPanel = UI_GetCommunicatorLogListRect(screenWidth, screenHeight);
    rowHeight = 60.0f * scale;
    gap = 9.0f * scale;
    availableHeight = listPanel.height - 28.0f * scale;
    visibleCount = (int)((availableHeight + gap) / (rowHeight + gap));

    if (visibleCount < 1) {
        visibleCount = 1;
    }

    return visibleCount;
}

int UI_ClampCommunicatorFirstVisibleLogIndex(int screenWidth, int screenHeight, int firstVisibleLogIndex, int totalLogCount) {
    int visibleCount;
    int maxFirstIndex;

    if (totalLogCount <= 0) {
        return 0;
    }

    visibleCount = UI_GetCommunicatorVisibleLogCount(screenWidth, screenHeight);
    if (totalLogCount <= visibleCount) {
        return 0;
    }

    maxFirstIndex = totalLogCount - visibleCount;
    if (firstVisibleLogIndex < 0) {
        return 0;
    }
    if (firstVisibleLogIndex > maxFirstIndex) {
        return maxFirstIndex;
    }

    return firstVisibleLogIndex;
}

int UI_GetCommunicatorFirstVisibleLogIndex(int screenWidth, int screenHeight, int selectedLogIndex, int totalLogCount) {
    int visibleCount;
    int maxFirstIndex;
    int firstIndex;

    if (totalLogCount <= 0) {
        return 0;
    }

    visibleCount = UI_GetCommunicatorVisibleLogCount(screenWidth, screenHeight);
    if (totalLogCount <= visibleCount) {
        return 0;
    }

    maxFirstIndex = totalLogCount - visibleCount;
    firstIndex = selectedLogIndex - visibleCount / 2;
    if (firstIndex < 0) {
        firstIndex = 0;
    }
    if (firstIndex > maxFirstIndex) {
        firstIndex = maxFirstIndex;
    }

    return firstIndex;
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

Rectangle UI_GetSettingsRowRect(int screenWidth, int screenHeight, int rowIndex) {
    float scale;
    Rectangle panel;
    float rowHeight;
    float rowGap;
    float startY;
    float contentWidth;
    float startX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    rowHeight = 76.0f * scale;
    rowGap = 14.0f * scale;
    startY = panel.y + 108.0f * scale;
    contentWidth = panel.width * 0.74f;
    startX = panel.x + (panel.width - contentWidth) * 0.5f;

    return Rectangle{
        startX,
        startY + rowIndex * (rowHeight + rowGap),
        contentWidth,
        rowHeight
    };
}

Rectangle UI_GetSettingsSliderRect(int screenWidth, int screenHeight, int sliderIndex) {
    float scale;
    Rectangle rowRect;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, sliderIndex);
    return Rectangle{
        rowRect.x + 250.0f * scale,
        rowRect.y + 31.0f * scale,
        rowRect.width - 430.0f * scale,
        14.0f * scale
    };
}

Rectangle UI_GetSettingsDecreaseButtonRect(int screenWidth, int screenHeight, int sliderIndex) {
    float scale;
    Rectangle rowRect;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, sliderIndex);
    return Rectangle{
        rowRect.x + rowRect.width - 156.0f * scale,
        rowRect.y + 18.0f * scale,
        40.0f * scale,
        40.0f * scale
    };
}

Rectangle UI_GetSettingsIncreaseButtonRect(int screenWidth, int screenHeight, int sliderIndex) {
    float scale;
    Rectangle rowRect;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, sliderIndex);
    return Rectangle{
        rowRect.x + rowRect.width - 44.0f * scale,
        rowRect.y + 18.0f * scale,
        40.0f * scale,
        40.0f * scale
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
    Rectangle rowRect;
    float width;
    float gap;
    float startX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, 3);
    width = 154.0f * scale;
    gap = 14.0f * scale;
    startX = rowRect.x + rowRect.width - width * 2.0f - gap - 18.0f * scale;
    return Rectangle{
        startX + buttonIndex * (width + gap),
        rowRect.y + 14.0f * scale,
        width,
        48.0f * scale
    };
}

Rectangle UI_GetSettingsAccountButtonRect(int screenWidth, int screenHeight, int buttonIndex) {
    float scale;
    Rectangle rowRect;
    float width;
    float gap;
    float startX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    rowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, 4);
    width = 164.0f * scale;
    gap = 14.0f * scale;
    startX = rowRect.x + rowRect.width - width * 2.0f - gap - 18.0f * scale;
    return Rectangle{
        startX + buttonIndex * (width + gap),
        rowRect.y + 14.0f * scale,
        width,
        48.0f * scale
    };
}

Rectangle UI_GetHudShortcutRect(int screenWidth, int screenHeight, int shortcutIndex) {
    float scale;
    float edgeInset;
    float bottomInset;
    float size;
    float gap;
    float totalWidth;
    float startX;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    edgeInset = 22.0f * scale;
    bottomInset = 44.0f * scale;
    size = 68.0f * scale;
    gap = 16.0f * scale;
    totalWidth = size * HUD_SHORTCUT_COUNT + gap * (HUD_SHORTCUT_COUNT - 1);
    startX = screenWidth - edgeInset - totalWidth;

    return Rectangle{
        startX + shortcutIndex * (size + gap),
        screenHeight - bottomInset - size,
        size,
        size
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

Rectangle UI_GetSettlementConfirmButtonRect(int screenWidth, int screenHeight, int buttonIndex, int buttonCount) {
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
    if (buttonCount < 1) {
        buttonCount = 1;
    }
    totalWidth = width * buttonCount + gap * (buttonCount - 1);
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
