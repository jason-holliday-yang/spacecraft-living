#include "ui_system.h"

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
    const char *modeTitle = mode == SAVE_PANEL_MODE_SAVE ? "Save Slots" : "Load Slots";
    const char *modeHint = mode == SAVE_PANEL_MODE_SAVE
        ? "Click an empty slot to save. Occupied slots can be deleted from the panel."
        : "Click an occupied slot to load. You can also delete a selected save.";
    char buffer[256];
    char accountBuffer[96];

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 9, 16, 208});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{153, 226, 255, 75});
    UIRuntime_DrawText(assets, modeTitle, Vector2{panel.x + 28.0f * scale, panel.y + 24.0f * scale}, 34.0f * scale, WHITE);
    UIRuntime_DrawText(
        assets,
        "Press ESC to return",
        Vector2{
            panel.x + panel.width - UIRuntime_MeasureText(assets, "Press ESC to return", 17.5f * scale).x - 28.0f * scale,
            panel.y + 30.0f * scale,
        },
        17.5f * scale,
        Color{182, 199, 214, 255}
    );
    std::snprintf(accountBuffer, sizeof(accountBuffer), "Account: %s", (accountName != nullptr && accountName[0] != '\0') ? accountName : "Unknown");
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
        std::snprintf(buffer, sizeof(buffer), "Slot %02d", slotIndex + 1);
        UIRuntime_DrawText(assets, buffer, Vector2{slotRect.x + 12.0f * scale, slotRect.y + 10.0f * scale}, 18.0f * scale, WHITE);

        if (occupied) {
            std::snprintf(buffer, sizeof(buffer), "Stage %d  Day %d", slots[slotIndex].stage, slots[slotIndex].dayCount + 1);
            UIRuntime_DrawText(assets, buffer, Vector2{slotRect.x + 12.0f * scale, slotRect.y + 34.0f * scale}, 15.0f * scale, Color{174, 240, 213, 255});
            std::snprintf(buffer, sizeof(buffer), "HP %.0f  O2 %.0f  Deaths %d", slots[slotIndex].health, slots[slotIndex].oxygen, slots[slotIndex].deathCount);
            UIRuntime_DrawText(assets, buffer, Vector2{slotRect.x + 12.0f * scale, slotRect.y + 55.0f * scale}, 13.0f * scale, Color{204, 219, 231, 255});
        } else {
            UIRuntime_DrawText(assets, "Empty", Vector2{slotRect.x + 12.0f * scale, slotRect.y + 39.0f * scale}, 18.0f * scale, Color{162, 174, 191, 255});
            UIRuntime_DrawText(
                assets,
                mode == SAVE_PANEL_MODE_SAVE ? "Ready for manual save" : "No data to load",
                Vector2{slotRect.x + 12.0f * scale, slotRect.y + 61.0f * scale},
                13.0f * scale,
                Color{137, 152, 171, 255}
            );
        }
    }

    UIRuntime_DrawPanel(detailsPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
    UIRuntime_DrawText(assets, "Selected Slot", Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 18.0f * scale}, 24.0f * scale, WHITE);

    if (selectedInfo != nullptr) {
        std::snprintf(buffer, sizeof(buffer), "Slot %02d", selectedInfo->slotIndex + 1);
        UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 58.0f * scale}, 20.0f * scale, Color{166, 255, 226, 255});
        UIRuntime_DrawText(
            assets,
            selectedInfo->exists ? "Occupied" : "Empty",
            Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 88.0f * scale},
            17.0f * scale,
            selectedInfo->exists ? Color{174, 240, 213, 255} : Color{183, 195, 208, 255}
        );

        if (selectedInfo->exists) {
            std::snprintf(buffer, sizeof(buffer), "Stage %d", selectedInfo->stage);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 126.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "Day %d", selectedInfo->dayCount + 1);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 152.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "Oxygen %.0f", selectedInfo->oxygen);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 178.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "Health %.0f", selectedInfo->health);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 204.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
            std::snprintf(buffer, sizeof(buffer), "Deaths %d", selectedInfo->deathCount);
            UIRuntime_DrawText(assets, buffer, Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 230.0f * scale}, 17.0f * scale, Color{221, 233, 244, 255});
        } else {
            UIRuntime_DrawText(
                assets,
                mode == SAVE_PANEL_MODE_SAVE
                    ? "Click this empty slot to write a manual save."
                    : "Choose another slot to load existing data.",
                Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 132.0f * scale},
                16.0f * scale,
                Color{196, 214, 230, 255}
            );
        }

        UIRuntime_DrawText(assets, "File Path", Vector2{detailsPanel.x + 18.0f * scale, detailsPanel.y + 284.0f * scale}, 17.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawWrappedText(assets, selectedInfo->path, Rectangle{detailsPanel.x + 18.0f * scale, detailsPanel.y + 312.0f * scale, detailsPanel.width - 36.0f * scale, 56.0f * scale}, 12.5f * scale, 14.0f * scale, Color{190, 207, 222, 255});
    }

    UIRuntime_DrawButton(assets, deleteButton, "Delete Selected Save", selectedInfo != nullptr && selectedInfo->exists);
}
