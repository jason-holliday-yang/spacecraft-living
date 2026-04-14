#include "ui_inventory_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cstdio>

void UI_DrawCraftOverlay(const AssetBundle *assets, const TaskSystem *tasks, const Player *player, int selectedRecipe, int screenWidth, int screenHeight) {
    float scale;
    Rectangle panel;
    Rectangle listPanel;
    Rectangle detailPanel;
    Rectangle buttonRect;
    int recipeCount;
    int recipeIndex;
    RecipeType recipe;
    RecipeVisualDef visual;
    char buffer[256];
    char statusBuffer[128];

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    listPanel = Rectangle{panel.x + 20.0f * scale, panel.y + 102.0f * scale, 630.0f * scale, panel.height - 134.0f * scale};
    detailPanel = Rectangle{panel.x + 674.0f * scale, panel.y + 102.0f * scale, panel.width - 706.0f * scale, panel.height - 134.0f * scale};
    buttonRect = UI_GetCraftActionButtonRect(screenWidth, screenHeight);
    recipeCount = Tasks_GetVisibleRecipeCount(tasks);
    selectedRecipe = UIInventory_NormalizeSelectedIndex(selectedRecipe, recipeCount);

    UIInventory_DrawOverlayShell(assets,
                                 panel,
                                 listPanel,
                                 detailPanel,
                                 scale,
                                 LOC_UI_CRAFT_TITLE,
                                 33.0f * scale,
                                 Loc_PickLiteral("Press F or Enter to craft. ESC closes.", "按 F 或 Enter 制作，按 ESC 关闭。"),
                                 16.0f * scale,
                                 Loc_PickLiteral("Select recipe and craft.", "选择配方并制作。"),
                                 15.5f * scale,
                                 17.0f * scale);

    for (recipeIndex = 0; recipeIndex < recipeCount; recipeIndex++) {
        Rectangle slotRect;
        bool selected;
        bool craftable;
        Color fill;
        Color outline;
        Color primary;
        Color secondary;
        char title[128];

        slotRect = UI_GetCraftSlotRect(screenWidth, screenHeight, recipeIndex);
        recipe = Tasks_GetVisibleRecipeAt(tasks, recipeIndex);
        visual = UIInventory_GetRecipeVisual(recipe);
        selected = recipeIndex == selectedRecipe;
        craftable = UIInventory_IsRecipeCraftable(tasks, player, recipe);
        fill = craftable ? Color{15, 33, 52, 230} : Color{12, 18, 28, 210};
        outline = selected ? Color{255, 214, 154, 220} : (craftable ? Color{109, 201, 234, 95} : Color{95, 108, 128, 60});
        primary = craftable ? visual.primary : Color{visual.primary.r, visual.primary.g, visual.primary.b, (unsigned char)((float)visual.primary.a * 0.42f)};
        secondary = craftable ? visual.secondary : Color{visual.secondary.r, visual.secondary.g, visual.secondary.b, (unsigned char)((float)visual.secondary.a * 0.38f)};

        UIRuntime_DrawPanel(slotRect, fill, outline);
        UIInventory_DrawRecipeIcon(assets, recipe, Rectangle{slotRect.x + 14.0f * scale, slotRect.y + 14.0f * scale, 52.0f * scale, 52.0f * scale}, primary, secondary);
        std::snprintf(title, sizeof(title), "%d. %s", recipeIndex + 1, Player_GetRecipeName(recipe));
        UIRuntime_DrawWrappedText(assets, title, Rectangle{slotRect.x + 78.0f * scale, slotRect.y + 12.0f * scale, slotRect.width - 160.0f * scale, 24.0f * scale}, 16.0f * scale, 17.0f * scale, WHITE);
        UIRuntime_DrawWrappedText(assets, Player_GetRecipeSummary(recipe), Rectangle{slotRect.x + 78.0f * scale, slotRect.y + 40.0f * scale, slotRect.width - 154.0f * scale, 40.0f * scale}, 13.0f * scale, 16.0f * scale, Color{208, 221, 232, 255});
        UIInventory_DrawStatusBadge(assets,
                                    Rectangle{slotRect.x + slotRect.width - 78.0f * scale, slotRect.y + 10.0f * scale, 62.0f * scale, 20.0f * scale},
                                    craftable ? Loc_PickLiteral("READY", "可做") : Loc_PickLiteral("LOCKED", "未就绪"),
                                    10.5f * scale,
                                    craftable ? Color{24, 60, 54, 235} : Color{34, 33, 40, 220},
                                    craftable ? Color{94, 230, 196, 120} : Color{112, 126, 148, 70},
                                    craftable ? Color{192, 255, 228, 255} : Color{168, 180, 196, 255});
    }

    if (recipeCount > 0) {
        const RecipeCatalogEntry *entry;

        recipe = Tasks_GetVisibleRecipeAt(tasks, selectedRecipe);
        entry = RecipeCatalog_Get(recipe);
        visual = UIInventory_GetRecipeVisual(recipe);
        UIInventory_GetRecipeStatusText(tasks, player, recipe, statusBuffer, sizeof(statusBuffer));

        UIInventory_DrawRecipeIcon(assets, recipe, Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 20.0f * scale, 94.0f * scale, 94.0f * scale}, visual.primary, visual.secondary);
        UIRuntime_DrawWrappedText(assets, Player_GetRecipeName(recipe), Rectangle{detailPanel.x + 126.0f * scale, detailPanel.y + 24.0f * scale, detailPanel.width - 144.0f * scale, 52.0f * scale}, 23.0f * scale, 24.0f * scale, WHITE);
        UIRuntime_DrawWrappedText(assets, statusBuffer, Rectangle{detailPanel.x + 126.0f * scale, detailPanel.y + 74.0f * scale, detailPanel.width - 144.0f * scale, 34.0f * scale}, 15.5f * scale, 17.0f * scale, Color{166, 255, 226, 255});
        UIRuntime_DrawWrappedText(assets,
                                  entry != NULL ? Loc_PickText(entry->detailDescription) : "",
                                  Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 136.0f * scale, detailPanel.width - 36.0f * scale, 104.0f * scale},
                                  15.5f * scale,
                                  18.0f * scale,
                                  Color{214, 226, 238, 255});
        std::snprintf(buffer,
                      sizeof(buffer),
                      "%s: %s",
                      Loc_PickLiteral("Needs", "需求"),
                      entry != NULL ? Loc_PickText(entry->ingredientText) : "");
        UIRuntime_DrawWrappedText(assets, buffer, Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 258.0f * scale, detailPanel.width - 36.0f * scale, 48.0f * scale}, 15.2f * scale, 18.0f * scale, Color{255, 220, 150, 255});
        if (entry != NULL && (entry->requiresWorkbench || entry->requiresLowStress)) {
            UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Advanced build: workbench + stable vitals required.", "高级制作：需要工作台并保持生命体征稳定。"), Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 318.0f * scale, detailPanel.width - 36.0f * scale, 54.0f * scale}, 14.0f * scale, 17.0f * scale, Color{194, 224, 255, 255});
        } else {
            UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Number keys for quick craft. F/Enter crafts selected.", "可用数字键快速制作，按 F/Enter 制作当前选中项。"), Rectangle{detailPanel.x + 18.0f * scale, detailPanel.y + 318.0f * scale, detailPanel.width - 36.0f * scale, 54.0f * scale}, 14.0f * scale, 17.0f * scale, Color{194, 224, 255, 255});
        }
        UIRuntime_DrawButton(assets, buttonRect, Loc_PickLiteral("Craft Selected", "制作当前配方"), true);
    } else {
        UIRuntime_DrawText(assets, Loc_PickLiteral("No recipes available.", "暂无可用配方。"), Vector2{detailPanel.x + 24.0f * scale, detailPanel.y + 26.0f * scale}, 24.0f * scale, Color{196, 214, 230, 255});
        UIRuntime_DrawWrappedText(assets, Loc_PickLiteral("Progress objectives to unlock more recipes.", "推进目标可解锁更多配方。"), Rectangle{detailPanel.x + 24.0f * scale, detailPanel.y + 62.0f * scale, detailPanel.width - 48.0f * scale, 84.0f * scale}, 15.5f * scale, 18.0f * scale, Color{188, 204, 220, 255});
    }
}
