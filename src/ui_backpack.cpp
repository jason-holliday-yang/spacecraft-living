#include "ui_inventory_internal.h"

#include "ui_runtime_internal.h"

#include <cstdio>

void UI_DrawBackpackOverlay(const AssetBundle *assets, const Player *player, int selectedItem, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle gridPanel;
    Rectangle detailPanel;
    char buffer[256];
    const BackpackEntryDef *selectedEntry;
    int itemIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    gridPanel = Rectangle{panel.x + 20.0f * scale, panel.y + 102.0f * scale, 646.0f * scale, panel.height - 134.0f * scale};
    detailPanel = Rectangle{panel.x + 690.0f * scale, panel.y + 102.0f * scale, panel.width - 722.0f * scale, panel.height - 134.0f * scale};
    selectedItem = UIInventory_NormalizeSelectedIndex(selectedItem, BACKPACK_ENTRY_COUNT);
    selectedEntry = UIInventory_GetBackpackEntry(selectedItem);

    UIInventory_DrawOverlayShell(assets,
                                 panel,
                                 gridPanel,
                                 detailPanel,
                                 scale,
                                 "Backpack",
                                 33.0f * scale,
                                 "Press B or ESC to close.",
                                 16.5f * scale,
                                 "Inspect supplies, check counts, and use field items from one cleaner layout.",
                                 16.0f * scale,
                                 17.0f * scale);

    for (itemIndex = 0; itemIndex < BACKPACK_ENTRY_COUNT; itemIndex++) {
        const BackpackEntryDef *entry;
        Rectangle slotRect;
        Rectangle titleRect;
        Rectangle badgeRect;
        bool owned;
        bool selected;
        int count;
        Color fill;
        Color outline;
        Color primary;
        Color secondary;

        entry = UIInventory_GetBackpackEntry(itemIndex);
        slotRect = UI_GetBackpackSlotRect(screenWidth, screenHeight, itemIndex);
        owned = UIInventory_BackpackEntryIsOwned(player, itemIndex);
        selected = itemIndex == selectedItem;
        count = UIInventory_BackpackEntryGetCount(player, itemIndex);
        fill = owned ? Color{15, 33, 52, 230} : Color{12, 18, 28, 210};
        outline = selected ? Color{255, 214, 154, 220} : (owned ? Color{109, 201, 234, 95} : Color{95, 108, 128, 60});
        primary = Color{entry->primary.r, entry->primary.g, entry->primary.b, (unsigned char)(owned ? entry->primary.a : (unsigned char)((float)entry->primary.a * 0.30f))};
        secondary = Color{entry->secondary.r, entry->secondary.g, entry->secondary.b, (unsigned char)(owned ? entry->secondary.a : (unsigned char)((float)entry->secondary.a * 0.26f))};
        titleRect = Rectangle{slotRect.x + 62.0f * scale, slotRect.y + 9.0f * scale, slotRect.width - 74.0f * scale, 31.0f * scale};
        badgeRect = Rectangle{slotRect.x + slotRect.width - 44.0f * scale, slotRect.y + slotRect.height - 26.0f * scale, 32.0f * scale, 16.0f * scale};

        UIRuntime_DrawPanel(slotRect, fill, outline);
        UIInventory_DrawBackpackIcon(assets, itemIndex, Rectangle{slotRect.x + 10.0f * scale, slotRect.y + 8.0f * scale, 44.0f * scale, slotRect.height - 16.0f * scale}, primary, secondary);
        UIRuntime_DrawWrappedText(assets, entry->name, titleRect, 12.0f * scale, 13.0f * scale, owned ? Color{229, 238, 246, 255} : Color{134, 145, 160, 255});
        UIRuntime_DrawText(assets, UIInventory_BackpackEntryGetCategory(entry), Vector2{slotRect.x + 62.0f * scale, slotRect.y + slotRect.height - 22.0f * scale}, 10.5f * scale, owned ? Color{166, 255, 226, 255} : Color{120, 133, 150, 255});

        if (entry->kind == BACKPACK_ENTRY_RESOURCE) {
            std::snprintf(buffer, sizeof(buffer), "%d", count);
        } else {
            std::snprintf(buffer, sizeof(buffer), "%s", owned ? "OK" : "--");
        }

        UIInventory_DrawStatusBadge(assets,
                                    badgeRect,
                                    buffer,
                                    9.8f * scale,
                                    owned ? Color{27, 52, 79, 235} : Color{26, 29, 39, 220},
                                    primary,
                                    owned ? WHITE : Color{152, 162, 178, 255});
    }

    if (selectedEntry != NULL) {
        Rectangle descriptionPanel;
        Rectangle actionPanel;
        char statusBuffer[96];

        descriptionPanel = Rectangle{detailPanel.x + 14.0f * scale, detailPanel.y + 138.0f * scale, detailPanel.width - 28.0f * scale, 170.0f * scale};
        actionPanel = Rectangle{detailPanel.x + 14.0f * scale, detailPanel.y + detailPanel.height - 122.0f * scale, detailPanel.width - 28.0f * scale, 96.0f * scale};

        UIInventory_DrawBackpackIcon(assets, selectedItem, Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 24.0f * scale, 92.0f * scale, 92.0f * scale}, selectedEntry->primary, selectedEntry->secondary);
        UIRuntime_DrawWrappedText(assets, selectedEntry->name, Rectangle{detailPanel.x + 128.0f * scale, detailPanel.y + 28.0f * scale, detailPanel.width - 146.0f * scale, 56.0f * scale}, 24.0f * scale, 25.0f * scale, WHITE);
        std::snprintf(buffer, sizeof(buffer), "%s", UIInventory_BackpackEntryGetCategory(selectedEntry));
        UIRuntime_DrawText(assets, buffer, Vector2{detailPanel.x + 128.0f * scale, detailPanel.y + 82.0f * scale}, 15.0f * scale, Color{166, 255, 226, 255});
        UIInventory_GetBackpackEntryStatus(player, selectedItem, statusBuffer, sizeof(statusBuffer));
        UIRuntime_DrawWrappedText(assets, statusBuffer, Rectangle{detailPanel.x + 128.0f * scale, detailPanel.y + 104.0f * scale, detailPanel.width - 146.0f * scale, 36.0f * scale}, 14.5f * scale, 16.0f * scale, Color{255, 214, 154, 255});
        UIRuntime_DrawPanel(descriptionPanel, Color{12, 24, 39, 220}, Color{255, 255, 255, 18});
        UIRuntime_DrawPanel(actionPanel, Color{12, 24, 39, 220}, Color{255, 255, 255, 18});
        UIRuntime_DrawText(assets, "Item Brief", Vector2{descriptionPanel.x + 14.0f * scale, descriptionPanel.y + 14.0f * scale}, 17.0f * scale, WHITE);
        UIRuntime_DrawWrappedText(assets, selectedEntry->description, Rectangle{descriptionPanel.x + 14.0f * scale, descriptionPanel.y + 42.0f * scale, descriptionPanel.width - 28.0f * scale, descriptionPanel.height - 56.0f * scale}, 14.8f * scale, 18.0f * scale, Color{214, 226, 238, 255});
        UIRuntime_DrawText(assets, "Field Action", Vector2{actionPanel.x + 14.0f * scale, actionPanel.y + 14.0f * scale}, 18.0f * scale, WHITE);
        UIRuntime_DrawWrappedText(
            assets,
            UIInventory_BackpackEntryCanUseDirectly(selectedItem)
                ? "Press F or Enter to use the selected field item now."
                : "This selection stays in storage for crafting, route prep, or passive use.",
            Rectangle{actionPanel.x + 14.0f * scale, actionPanel.y + 42.0f * scale, actionPanel.width - 28.0f * scale, actionPanel.height - 54.0f * scale},
            14.5f * scale,
            18.0f * scale,
            UIInventory_BackpackEntryCanUseDirectly(selectedItem) ? Color{166, 255, 226, 255} : Color{190, 207, 222, 255}
        );
    }
}
