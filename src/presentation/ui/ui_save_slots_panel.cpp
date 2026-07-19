#include "ui_system.h"

#include "localization.h"
#include "ui_components_internal.h"
#include "ui_runtime_internal.h"

#include <algorithm>
#include <cstdio>

namespace {

void DrawSaveMetric(const AssetBundle *assets,
                    Rectangle rect,
                    const char *label,
                    float value,
                    float maxValue,
                    Color accent,
                    float scale) {
    const UITheme *theme = UITheme_Get();
    const float ratio = maxValue > 0.0f ? std::max(0.0f, std::min(1.0f, value / maxValue)) : 0.0f;
    char valueBuffer[32];

    UIRuntime_DrawText(assets, label, Vector2{rect.x, rect.y}, 13.0f * scale, theme->textSecondary);
    std::snprintf(valueBuffer, sizeof(valueBuffer), "%.0f", value);
    UIRuntime_DrawText(assets,
                       valueBuffer,
                       Vector2{rect.x + rect.width - UIRuntime_MeasureText(assets, valueBuffer, 13.0f * scale).x, rect.y},
                       13.0f * scale,
                       theme->iceWhite);
    DrawRectangleRounded(Rectangle{rect.x, rect.y + 20.0f * scale, rect.width, 7.0f * scale},
                         0.45f,
                         8,
                         Color{5, 18, 30, 235});
    DrawRectangleRounded(Rectangle{rect.x, rect.y + 20.0f * scale, rect.width * ratio, 7.0f * scale},
                         0.45f,
                         8,
                         accent);
}

}  // namespace

void UI_DrawSaveSlotsOverlay(const AssetBundle *assets,
                             const SaveSlotInfo *slots,
                             int slotCount,
                             SavePanelMode mode,
                             int selectedSlot,
                             const char *accountName,
                             int screenWidth,
                             int screenHeight) {
    const UITheme *theme = UITheme_Get();
    const float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    const float slotGap = 16.0f * scale;
    const float detailsGap = 24.0f * scale;
    const float detailsRightInset = 24.0f * scale;
    const Rectangle lastSlotRect = UI_GetSaveSlotRect(screenWidth, screenHeight, 3);
    const Rectangle panel{
        screenWidth * 0.5f - 520.0f * scale,
        screenHeight * 0.5f - 290.0f * scale,
        1040.0f * scale,
        580.0f * scale,
    };
    Rectangle detailsPanel{
        lastSlotRect.x + lastSlotRect.width + detailsGap,
        panel.y + 108.0f * scale,
        panel.x + panel.width - detailsRightInset - (lastSlotRect.x + lastSlotRect.width + detailsGap),
        386.0f * scale,
    };
    const Rectangle primaryButton = UI_GetSavePrimaryButtonRect(screenWidth, screenHeight);
    const Rectangle deleteButton = UI_GetSaveDeleteButtonRect(screenWidth, screenHeight);
    const SaveSlotInfo *selectedInfo = (selectedSlot >= 0 && selectedSlot < slotCount) ? &slots[selectedSlot] : nullptr;
    const bool primaryEnabled = selectedInfo != nullptr
        && ((mode == SAVE_PANEL_MODE_LOAD && selectedInfo->exists)
            || (mode == SAVE_PANEL_MODE_SAVE && !selectedInfo->exists));
    const bool deleteEnabled = selectedInfo != nullptr && selectedInfo->exists;
    const char *primaryLabel = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("WRITE ARCHIVE", "写入存档")
        : Loc_PickLiteral("RESTORE RUN", "读取远征");
    const char *modeTitle = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("EXPEDITION ARCHIVE", "远征存档柜")
        : Loc_PickLiteral("RESTORE EXPEDITION", "恢复远征记录");
    const char *modeEyebrow = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("LOXI VAULT // MANUAL RECORD", "洛希档案 // 手动记录")
        : Loc_PickLiteral("LOXI VAULT // RECOVERY", "洛希档案 // 记录恢复");
    const char *modeHint = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("Choose an empty archive cell. Occupied cells can be removed from the detail panel.", "选择一个空档案格写入记录；已占用档案可在右侧详情区删除。")
        : Loc_PickLiteral("Choose an occupied archive cell, review its survival data, then restore the expedition.", "选择已占用档案，确认生存数据后恢复远征。 ");
    char buffer[256];
    char accountBuffer[96];

    UIComponents_DrawScrim(screenWidth, screenHeight, 210);
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);
    UIComponents_DrawPanelHeader(assets,
                                 panel,
                                 modeEyebrow,
                                 modeTitle,
                                 LOC_UI_PRESS_ESC_RETURN,
                                 scale,
                                 theme->signalCyan);

    std::snprintf(accountBuffer,
                  sizeof(accountBuffer),
                  "%s  //  %s",
                  Loc_PickLiteral("ACTIVE ID", "当前身份"),
                  (accountName != nullptr && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知"));
    UIComponents_DrawBadge(assets,
                           Rectangle{panel.x + 370.0f * scale, panel.y + 29.0f * scale, 220.0f * scale, 26.0f * scale},
                           accountBuffer,
                           theme->signalBlue,
                           true,
                           scale);
    UIRuntime_DrawWrappedText(assets,
                              modeHint,
                              Rectangle{panel.x + 30.0f * scale, panel.y + 83.0f * scale, panel.width - 60.0f * scale, 30.0f * scale},
                              14.5f * scale,
                              16.0f * scale,
                              theme->textSecondary);

    for (int slotIndex = 0; slotIndex < slotCount; ++slotIndex) {
        const Rectangle slotRect = UI_GetSaveSlotRect(screenWidth, screenHeight, slotIndex);
        const bool occupied = slots[slotIndex].exists;
        const bool isSelected = slotIndex == selectedSlot;
        const Color accent = occupied ? theme->signalCyan : theme->border;
        Rectangle numberBadge{slotRect.x + 10.0f * scale, slotRect.y + 9.0f * scale, 38.0f * scale, 20.0f * scale};

        UIComponents_DrawCard(slotRect, isSelected, true, scale, accent);
        std::snprintf(buffer, sizeof(buffer), "%02d", slotIndex + 1);
        UIComponents_DrawBadge(assets, numberBadge, buffer, accent, true, scale);
        UIRuntime_DrawText(assets,
                           occupied ? Loc_PickLiteral("ARCHIVE ONLINE", "档案在线") : Loc_PickLiteral("EMPTY CELL", "空档案格"),
                           Vector2{slotRect.x + 56.0f * scale, slotRect.y + 11.0f * scale},
                           11.5f * scale,
                           occupied ? theme->signalCyan : theme->textMuted);

        if (occupied) {
            std::snprintf(buffer,
                          sizeof(buffer),
                          "%s %d  ·  %s %d",
                          Loc_PickLiteral("STAGE", "阶段"),
                          slots[slotIndex].stage,
                          Loc_PickLiteral("DAY", "日"),
                          slots[slotIndex].dayCount);
            UIRuntime_DrawText(assets,
                               buffer,
                               Vector2{slotRect.x + 12.0f * scale, slotRect.y + 39.0f * scale},
                               14.0f * scale,
                               theme->textPrimary);
            std::snprintf(buffer,
                          sizeof(buffer),
                          "%s %.0f  ·  %s %.0f",
                          Loc_PickLiteral("O₂", "氧气"),
                          slots[slotIndex].oxygen,
                          Loc_PickLiteral("HP", "生命"),
                          slots[slotIndex].health);
            UIRuntime_DrawText(assets,
                               buffer,
                               Vector2{slotRect.x + 12.0f * scale, slotRect.y + 61.0f * scale},
                               12.0f * scale,
                               theme->textSecondary);
        } else {
            UIRuntime_DrawText(assets,
                               mode == SAVE_PANEL_MODE_SAVE
                                   ? Loc_PickLiteral("Ready to receive a manual record", "可写入一份手动记录")
                                   : Loc_PickLiteral("No expedition data", "没有远征数据"),
                               Vector2{slotRect.x + 12.0f * scale, slotRect.y + 47.0f * scale},
                               12.5f * scale,
                               theme->textMuted);
        }
    }

    if (detailsPanel.width < 250.0f * scale) {
        detailsPanel.x = lastSlotRect.x + lastSlotRect.width + slotGap;
        detailsPanel.width = panel.x + panel.width - detailsRightInset - detailsPanel.x;
    }

    UIComponents_DrawCard(detailsPanel, true, true, scale, theme->signalBlue);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("ARCHIVE INSPECTION", "档案检查"),
                       Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 18.0f * scale},
                       13.0f * scale,
                       theme->signalCyan);
    UIComponents_DrawDivider(Rectangle{detailsPanel.x + 18.0f * scale,
                                       detailsPanel.y + 43.0f * scale,
                                       detailsPanel.width - 36.0f * scale,
                                       1.0f * scale},
                             theme->signalBlue,
                             scale);

    if (selectedInfo != nullptr) {
        std::snprintf(buffer, sizeof(buffer), "%s %02d", Loc_PickLiteral("ARCHIVE", "档案"), selectedInfo->slotIndex + 1);
        UIRuntime_DrawText(assets,
                           buffer,
                           Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 57.0f * scale},
                           23.0f * scale,
                           theme->iceWhite);
        UIComponents_DrawBadge(assets,
                               Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 91.0f * scale, 132.0f * scale, 25.0f * scale},
                               selectedInfo->exists ? Loc_PickLiteral("OCCUPIED", "已占用") : Loc_PickLiteral("EMPTY", "空"),
                               selectedInfo->exists ? theme->positive : theme->disabled,
                               true,
                               scale);

        if (selectedInfo->exists) {
            std::snprintf(buffer,
                          sizeof(buffer),
                          "%s %d  ·  %s %d  ·  %s %d",
                          Loc_PickLiteral("Stage", "阶段"),
                          selectedInfo->stage,
                          Loc_PickLiteral("Day", "天数"),
                          selectedInfo->dayCount,
                          Loc_PickLiteral("Deaths", "死亡"),
                          selectedInfo->deathCount);
            UIRuntime_DrawText(assets,
                               buffer,
                               Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 130.0f * scale},
                               14.0f * scale,
                               theme->textSecondary);
            DrawSaveMetric(assets,
                           Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 164.0f * scale, detailsPanel.width - 36.0f * scale, 30.0f * scale},
                           Loc_PickLiteral("OXYGEN RESERVE", "氧气储备"),
                           selectedInfo->oxygen,
                           MAX_OXYGEN,
                           theme->signalCyan,
                           scale);
            DrawSaveMetric(assets,
                           Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 211.0f * scale, detailsPanel.width - 36.0f * scale, 30.0f * scale},
                           Loc_PickLiteral("BODY CONDITION", "生命状态"),
                           selectedInfo->health,
                           BASE_MAX_HEALTH,
                           selectedInfo->health < BASE_MAX_HEALTH * 0.30f ? theme->danger : theme->positive,
                           scale);
        } else {
            UIRuntime_DrawWrappedText(
                assets,
                mode == SAVE_PANEL_MODE_SAVE
                    ? Loc_PickLiteral("This cell is clear and ready for a manual expedition record.", "这个档案格为空，可以写入当前远征记录。")
                    : Loc_PickLiteral("This cell contains no expedition data. Select an occupied archive.", "这个档案格没有远征数据，请选择已占用档案。"),
                Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 138.0f * scale, detailsPanel.width - 36.0f * scale, 78.0f * scale},
                15.0f * scale,
                19.0f * scale,
                theme->textSecondary);
        }

        UIRuntime_DrawText(assets,
                           Loc_PickLiteral("LOCAL RECORD PATH", "本地记录路径"),
                           Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 276.0f * scale},
                           12.0f * scale,
                           theme->archiveGold);
        UIRuntime_DrawWrappedText(assets,
                                  selectedInfo->path,
                                  Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 299.0f * scale, detailsPanel.width - 36.0f * scale, 60.0f * scale},
                                  11.5f * scale,
                                  13.5f * scale,
                                  theme->textMuted);
    } else {
        UIRuntime_DrawWrappedText(assets,
                                  Loc_PickLiteral("Select an archive cell to inspect its survival record.", "选择一个档案格以检查其生存记录。"),
                                  Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 78.0f * scale, detailsPanel.width - 36.0f * scale, 90.0f * scale},
                                  16.0f * scale,
                                  20.0f * scale,
                                  theme->textSecondary);
    }

    UIComponents_DrawCompactButton(assets,
                                  primaryButton,
                                  primaryLabel,
                                  primaryEnabled,
                                  false,
                                  UI_ACTION_PRIMARY,
                                  scale);
    UIComponents_DrawCompactButton(assets,
                                  deleteButton,
                                  Loc_PickLiteral("DELETE", "删除"),
                                  deleteEnabled,
                                  false,
                                  UI_ACTION_DANGER,
                                  scale);
}
