#include "ui_inventory_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

#include <cstdio>

void UI_DrawBackpackOverlay(const AssetBundle *assets, const Player *player, int selectedItem, int screenWidth, int screenHeight) {
    const UITheme *theme = UITheme_Get();
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
                                 Loc_PickLiteral("Backpack", "背包"),
                                 33.0f * scale,
                                 Loc_PickLiteral("Press B or ESC to seal the pack again.", "按 B 或 ESC 收起背包。"),
                                 16.5f * scale,
                                 Loc_PickLiteral("Check what is left, what can still save you, and what the next push will cost.", "看看还剩下什么、哪些东西还能救命，以及下一次推进会消耗掉什么。"),
                                 17.6f * scale,
                                 19.6f * scale);

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
        fill = owned ? theme->surfaceRaised : theme->surfaceDisabled;
        outline = selected ? theme->signalCyan : (owned ? theme->border : theme->borderSoft);
        primary = Color{entry->primary.r, entry->primary.g, entry->primary.b, (unsigned char)(owned ? entry->primary.a : (unsigned char)((float)entry->primary.a * 0.30f))};
        secondary = Color{entry->secondary.r, entry->secondary.g, entry->secondary.b, (unsigned char)(owned ? entry->secondary.a : (unsigned char)((float)entry->secondary.a * 0.26f))};
        titleRect = Rectangle{slotRect.x + 62.0f * scale, slotRect.y + 9.0f * scale, slotRect.width - 74.0f * scale, 31.0f * scale};
        badgeRect = Rectangle{slotRect.x + slotRect.width - 44.0f * scale, slotRect.y + slotRect.height - 26.0f * scale, 32.0f * scale, 16.0f * scale};

        (void)fill;
        UIComponents_DrawCard(slotRect, selected, true, scale, outline);
        UIInventory_DrawBackpackIcon(assets, itemIndex, Rectangle{slotRect.x + 10.0f * scale, slotRect.y + 8.0f * scale, 44.0f * scale, slotRect.height - 16.0f * scale}, primary, secondary);
        UIRuntime_DrawWrappedText(assets, Loc_PickText(entry->name), titleRect, 13.2f * scale, 14.6f * scale, owned ? theme->textPrimary : theme->textMuted);
        UIRuntime_DrawText(assets, UIInventory_BackpackEntryGetCategory(entry), Vector2{slotRect.x + 62.0f * scale, slotRect.y + slotRect.height - 22.0f * scale}, 11.6f * scale, owned ? theme->signalCyan : theme->textMuted);

        if (entry->kind == BACKPACK_ENTRY_RESOURCE) {
            std::snprintf(buffer, sizeof(buffer), "%d", count);
        } else {
            std::snprintf(buffer, sizeof(buffer), "%s", owned ? Loc_PickLiteral("OK", "已就绪") : "--");
        }

        UIInventory_DrawStatusBadge(assets,
                                    badgeRect,
                                    buffer,
                                    9.8f * scale,
                                    owned ? Color{13, 55, 82, 240} : theme->surfaceDisabled,
                                    primary,
                                    owned ? theme->iceWhite : theme->disabled);
    }

    if (selectedEntry != NULL) {
        Rectangle descriptionPanel;
        Rectangle actionPanel;
        char statusBuffer[96];

        descriptionPanel = Rectangle{detailPanel.x + 14.0f * scale, detailPanel.y + 138.0f * scale, detailPanel.width - 28.0f * scale, 170.0f * scale};
        actionPanel = Rectangle{detailPanel.x + 14.0f * scale, detailPanel.y + detailPanel.height - 122.0f * scale, detailPanel.width - 28.0f * scale, 96.0f * scale};

        UIInventory_DrawBackpackIcon(assets, selectedItem, Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 24.0f * scale, 92.0f * scale, 92.0f * scale}, selectedEntry->primary, selectedEntry->secondary);
        UIRuntime_DrawWrappedText(assets, Loc_PickText(selectedEntry->name), Rectangle{detailPanel.x + 128.0f * scale, detailPanel.y + 28.0f * scale, detailPanel.width - 146.0f * scale, 56.0f * scale}, 24.0f * scale, 25.0f * scale, theme->iceWhite);
        std::snprintf(buffer, sizeof(buffer), "%s", UIInventory_BackpackEntryGetCategory(selectedEntry));
        UIRuntime_DrawText(assets, buffer, Vector2{detailPanel.x + 128.0f * scale, detailPanel.y + 82.0f * scale}, 15.0f * scale, theme->signalCyan);
        UIInventory_GetBackpackEntryStatus(player, selectedItem, statusBuffer, sizeof(statusBuffer));
        UIRuntime_DrawWrappedText(assets, statusBuffer, Rectangle{detailPanel.x + 128.0f * scale, detailPanel.y + 104.0f * scale, detailPanel.width - 146.0f * scale, 36.0f * scale}, 16.0f * scale, 18.5f * scale, theme->archiveGold);
        UIComponents_DrawCard(descriptionPanel, false, true, scale, theme->signalBlue);
        UIComponents_DrawCard(actionPanel, false, true, scale, UIInventory_BackpackEntryCanUseDirectly(selectedItem) ? theme->positive : theme->border);
        UIRuntime_DrawText(assets, Loc_PickLiteral("Field Notes", "现场描述"), Vector2{descriptionPanel.x + 14.0f * scale, descriptionPanel.y + 14.0f * scale}, 18.8f * scale, theme->iceWhite);
        UIRuntime_DrawWrappedText(assets, Loc_PickText(selectedEntry->description), Rectangle{descriptionPanel.x + 14.0f * scale, descriptionPanel.y + 42.0f * scale, descriptionPanel.width - 28.0f * scale, descriptionPanel.height - 56.0f * scale}, 16.8f * scale, 21.0f * scale, theme->textPrimary);
        UIRuntime_DrawText(assets, Loc_PickLiteral("Use Case", "使用方式"), Vector2{actionPanel.x + 14.0f * scale, actionPanel.y + 14.0f * scale}, 18.0f * scale, theme->iceWhite);
        UIRuntime_DrawWrappedText(
            assets,
            UIInventory_BackpackEntryCanUseDirectly(selectedItem)
                ? Loc_PickLiteral("Press F or Enter to use the selected item now.", "按 F 或 Enter 立即使用当前选中物品。")
                : Loc_PickLiteral("This item is passive, crafted material, or used through a specific world interaction.", "该物品属于被动效果、制作材料，或需要通过特定场景交互使用。"),
            Rectangle{actionPanel.x + 14.0f * scale, actionPanel.y + 42.0f * scale, actionPanel.width - 28.0f * scale, actionPanel.height - 54.0f * scale},
            16.0f * scale,
            20.0f * scale,
            UIInventory_BackpackEntryCanUseDirectly(selectedItem) ? theme->positive : theme->textSecondary
        );
    }
}
