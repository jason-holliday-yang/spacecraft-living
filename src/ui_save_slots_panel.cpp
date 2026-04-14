#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cstdio>

void UI_DrawSaveSlotsOverlay(const AssetBundle *assets,
                             const SaveSlotInfo *slots,
                             int slotCount,
                             SavePanelMode mode,
                             int selectedSlot,
                             const char *accountName,
                             int screenWidth,
                             int screenHeight) {
    float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    Rectangle panel{
        screenWidth * 0.5f - 520.0f * scale,
        screenHeight * 0.5f - 290.0f * scale,
        1040.0f * scale,
        580.0f * scale,
    };
    Rectangle detailsPanel{
        panel.x + 724.0f * scale,
        panel.y + 108.0f * scale,
        286.0f * scale,
        386.0f * scale,
    };
    Rectangle deleteButton = UI_GetSaveDeleteButtonRect(screenWidth, screenHeight);
    const SaveSlotInfo *selectedInfo = (selectedSlot >= 0 && selectedSlot < slotCount) ? &slots[selectedSlot] : nullptr;
    const char *modeTitle = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("Save Slots", "保存栏位")
        : Loc_PickLiteral("Load Slots", "读取栏位");
    const char *modeHint = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("Click an empty slot to save. Occupied slots can be deleted from the panel.", "点击空栏位即可保存，已占用栏位也可在此面板删除。")
        : Loc_PickLiteral("Click an occupied slot to load. You can also delete a selected save.", "点击已占用栏位即可读取，也可以删除当前选中的存档。");
    char buffer[256];
    char accountBuffer[96];

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 208});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{153, 226, 255, 75});
    UIRuntime_DrawText(assets, modeTitle, Vector2{panel.x + 28.0f * scale, panel.y + 24.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawText(
        assets,
        LOC_UI_PRESS_ESC_RETURN,
        Vector2{
            panel.x + panel.width - UIRuntime_MeasureText(assets, LOC_UI_PRESS_ESC_RETURN, 17.5f * scale).x - 28.0f * scale,
            panel.y + 30.0f * scale,
        },
        17.5f * scale,
        Color{182, 199, 214, 255}
    );
    std::snprintf(accountBuffer,
                  sizeof(accountBuffer),
                  "%s: %s",
                  Loc_PickLiteral("Account", "账号"),
                  (accountName != nullptr && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知"));
    UIRuntime_DrawText(assets, accountBuffer, Vector2{panel.x + 28.0f * scale, panel.y + 60.0f * scale}, 15.5f * scale, Color{166, 255, 226, 255});
    UIRuntime_DrawWrappedText(assets, modeHint, Rectangle{panel.x + 28.0f * scale, panel.y + 84.0f * scale, panel.width - 56.0f * scale, 34.0f * scale}, 15.5f * scale, 16.0f * scale, Color{196, 214, 230, 255});

    for (int slotIndex = 0; slotIndex < slotCount; ++slotIndex) {
        Rectangle slotRect = UI_GetSaveSlotRect(screenWidth, screenHeight, slotIndex);
        bool occupied = slots[slotIndex].exists;
        bool isSelected = slotIndex == selectedSlot;
        Color fill = occupied ? Color{16, 34, 52, 235} : Color{14, 20, 32, 215};
        Color outline = isSelected ? Color{255, 214, 154, 210}
                                   : (occupied ? Color{104, 196, 222, 90} : Color{112, 126, 148, 60});

        UIRuntime_DrawPanel(slotRect, fill, outline);
        std::snprintf(buffer, sizeof(buffer), "%s %02d", Loc_PickLiteral("Slot", "栏位"), slotIndex + 1);
        UIRuntime_DrawText(assets, buffer, Vector2{slotRect.x + 12.0f * scale, slotRect.y + 10.0f * scale}, 18.0f * scale, WHITE);

        if (occupied) {
            std::snprintf(buffer, sizeof(buffer), "%s %d  %s %d", Loc_PickLiteral("Stage", "阶段"), slots[slotIndex].stage, Loc_PickLiteral("Day", "天数"), slots[slotIndex].dayCount + 1);
            UIRuntime_DrawText(assets, buffer, Vector2{slotRect.x + 12.0f * scale, slotRect.y + 34.0f * scale}, 15.0f * scale, Color{174, 240, 213, 255});
            std::snprintf(buffer, sizeof(buffer), "HP %.0f  O2 %.0f  %s %d", slots[slotIndex].health, slots[slotIndex].oxygen, Loc_PickLiteral("Deaths", "死亡"), slots[slotIndex].deathCount);
            UIRuntime_DrawText(assets, buffer, Vector2{slotRect.x + 12.0f * scale, slotRect.y + 55.0f * scale}, 13.0f * scale, Color{204, 219, 231, 255});
        } else {
            UIRuntime_DrawText(assets, Loc_PickLiteral("Empty", "空"), Vector2{slotRect.x + 12.0f * scale, slotRect.y + 39.0f * scale}, 18.0f * scale, Color{162, 174, 191, 255});
            UIRuntime_DrawText(
                assets,
                mode == SAVE_PANEL_MODE_SAVE ? Loc_PickLiteral("Ready for manual save", "可用于手动保存") : Loc_PickLiteral("No data to load", "没有可读取数据"),
                Vector2{slotRect.x + 12.0f * scale, slotRect.y + 61.0f * scale},
                13.0f * scale,
                Color{137, 152, 171, 255}
            );
        }
    }

    UIRuntime_DrawPanel(detailsPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
    UIRuntime_DrawText(assets, Loc_PickLiteral("Selected Slot", "当前栏位"), Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 18.0f * scale}, 24.0f * scale, WHITE);

    if (selectedInfo != nullptr) {
        std::snprintf(buffer, sizeof(buffer), "%s %02d", Loc_PickLiteral("Slot", "栏位"), selectedInfo->slotIndex + 1);
        UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 58.0f * scale}, 20.0f * scale, Color{166, 255, 226, 255});
        UIRuntime_DrawText(
            assets,
            selectedInfo->exists ? Loc_PickLiteral("Occupied", "已占用") : Loc_PickLiteral("Empty", "空"),
            Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 88.0f * scale},
            17.0f * scale,
            selectedInfo->exists ? Color{174, 240, 213, 255} : Color{183, 195, 208, 255}
        );

        if (selectedInfo->exists) {
            std::snprintf(buffer, sizeof(buffer), "%s %d", Loc_PickLiteral("Stage", "阶段"), selectedInfo->stage);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 126.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "%s %d", Loc_PickLiteral("Day", "天数"), selectedInfo->dayCount + 1);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 152.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "%s %.0f", Loc_PickLiteral("Oxygen", "氧气"), selectedInfo->oxygen);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 178.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "%s %.0f", Loc_PickLiteral("Health", "生命"), selectedInfo->health);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 204.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "%s %d", Loc_PickLiteral("Deaths", "死亡"), selectedInfo->deathCount);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 230.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
        } else {
            UIRuntime_DrawText(
                assets,
                mode == SAVE_PANEL_MODE_SAVE
                    ? Loc_PickLiteral("Click this empty slot to write a manual save.", "点击这个空栏位写入手动存档。")
                    : Loc_PickLiteral("Choose another slot to load existing data.", "请选择其他栏位读取已有数据。"),
                Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 132.0f * scale},
                16.0f * scale,
                Color{196, 214, 230, 255}
            );
        }

        UIRuntime_DrawText(assets, Loc_PickLiteral("File Path", "文件路径"), Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 284.0f * scale}, 17.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets, selectedInfo->path, Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 312.0f * scale, detailsPanel.width - 36.0f * scale, 56.0f * scale}, 12.5f * scale, 14.0f * scale, Color{190, 207, 222, 255});
    }

    UIRuntime_DrawButton(assets, deleteButton, Loc_PickLiteral("Delete Selected Save", "删除当前存档"), selectedInfo != nullptr && selectedInfo->exists);
}
