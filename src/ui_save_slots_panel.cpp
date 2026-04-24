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
    Rectangle primaryButton = UI_GetSavePrimaryButtonRect(screenWidth, screenHeight);
    Rectangle deleteButton = UI_GetSaveDeleteButtonRect(screenWidth, screenHeight);
    const SaveSlotInfo *selectedInfo = (selectedSlot >= 0 && selectedSlot < slotCount) ? &slots[selectedSlot] : nullptr;
    bool primaryEnabled = selectedInfo != nullptr
        && ((mode == SAVE_PANEL_MODE_LOAD && selectedInfo->exists)
            || (mode == SAVE_PANEL_MODE_SAVE && !selectedInfo->exists));
    bool deleteEnabled = selectedInfo != nullptr && selectedInfo->exists;
    const char *primaryLabel = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("Save", "保存")
        : Loc_PickLiteral("Load", "读取");
    const char *modeTitle = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("Save Slots", "保存栏位")
        : Loc_PickLiteral("Load Slots", "读取栏位");
    const char *modeHint = mode == SAVE_PANEL_MODE_SAVE
        ? Loc_PickLiteral("Select a slot first. Empty slots can be saved, occupied slots can be deleted.", "先选中栏位。空栏位可保存，已占用栏位可删除。")
        : Loc_PickLiteral("Select an occupied slot first, then use the buttons on the right to load or delete it.", "先选中已占用栏位，再使用右侧按钮读取或删除。");
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
            std::snprintf(buffer, sizeof(buffer), "%s %d", Loc_PickLiteral("Stage", "阶段"), slots[slotIndex].stage);
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
            std::snprintf(buffer, sizeof(buffer), "%s %.0f", Loc_PickLiteral("Oxygen", "氧气"), selectedInfo->oxygen);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 152.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "%s %.0f", Loc_PickLiteral("Health", "生命"), selectedInfo->health);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 178.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "%s %d", Loc_PickLiteral("Deaths", "死亡"), selectedInfo->deathCount);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 204.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
        } else {
            UIRuntime_DrawText(
                assets,
                mode == SAVE_PANEL_MODE_SAVE
                    ? Loc_PickLiteral("Use the save button below to write a manual save into this empty slot.", "使用下方保存按钮，把手动存档写入这个空栏位。")
                    : Loc_PickLiteral("This slot is empty. Choose another occupied slot, then use the load button below.", "这个栏位是空的。请选择其他已占用栏位，再使用下方读取按钮。"),
                Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 132.0f * scale},
                16.0f * scale,
                Color{196, 214, 230, 255}
            );
        }

        UIRuntime_DrawText(assets, Loc_PickLiteral("File Path", "文件路径"), Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 284.0f * scale}, 17.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets, selectedInfo->path, Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 312.0f * scale, detailsPanel.width - 36.0f * scale, 56.0f * scale}, 12.5f * scale, 14.0f * scale, Color{190, 207, 222, 255});
    }

    UIRuntime_DrawButton(assets, primaryButton, primaryLabel, primaryEnabled);
    UIRuntime_DrawButton(assets, deleteButton, Loc_PickLiteral("Delete", "删除"), deleteEnabled);
}
