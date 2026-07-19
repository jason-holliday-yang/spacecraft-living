#include "ui_inventory_internal.h"
#include "task_crafting_internal.h"
#include "player_presentation.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

#include <cstdio>

int UIInventory_NormalizeSelectedIndex(int selectedIndex, int itemCount) {
    if (itemCount <= 0) {
        return 0;
    }

    if (selectedIndex < 0 || selectedIndex >= itemCount) {
        return 0;
    }

    return selectedIndex;
}

void UIInventory_DrawOverlayShell(const AssetBundle *assets,
                                  Rectangle panel,
                                  Rectangle listPanel,
                                  Rectangle detailPanel,
                                  float scale,
                                  const char *title,
                                  float titleSize,
                                  const char *closeHint,
                                  float closeHintSize,
                                  const char *subtitle,
                                  float subtitleSize,
                                  float subtitleLineSpacing) {
    const UITheme *theme = UITheme_Get();

    (void)titleSize;
    (void)closeHintSize;
    UIComponents_DrawScrim(GetScreenWidth(), GetScreenHeight(), 196);
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);
    UIComponents_DrawPanelHeader(assets,
                                 panel,
                                 Loc_PickLiteral("LOXI FIELD ARCHIVE // INVENTORY", "洛希现场档案 // 物资"),
                                 title,
                                 closeHint,
                                 scale,
                                 theme->signalCyan);
    UIRuntime_DrawWrappedText(assets,
                              subtitle,
                              Rectangle{panel.x + 30.0f * scale, panel.y + 84.0f * scale, panel.width - 60.0f * scale, 30.0f * scale},
                              subtitleSize,
                              subtitleLineSpacing,
                              theme->textSecondary);
    UIComponents_DrawCard(listPanel, false, true, scale, theme->signalBlue);
    UIComponents_DrawCard(detailPanel, true, true, scale, theme->signalCyan);
}

void UIInventory_DrawStatusBadge(const AssetBundle *assets,
                                 Rectangle rect,
                                 const char *label,
                                 float fontSize,
                                 Color fill,
                                 Color outline,
                                 Color textColor) {
    Vector2 textSize;
    Vector2 textPosition;

    UIRuntime_DrawPanel(rect, fill, outline);
    textSize = UIRuntime_MeasureText(assets, label, fontSize);
    textPosition = Vector2{
        rect.x + (rect.width - textSize.x) * 0.5f,
        rect.y + (rect.height - textSize.y) * 0.5f
    };
    UIRuntime_DrawText(assets, label, textPosition, fontSize, textColor);
}

bool UIInventory_IsRecipeCraftable(const TaskSystem *tasks, const Player *player, RecipeType recipe) {
    return Tasks_CanCraftRecipe(tasks, player, recipe);
}

void UIInventory_GetRecipeStatusText(const TaskSystem *tasks, const Player *player, RecipeType recipe, char *buffer, size_t bufferSize) {
    const RecipeCatalogEntry *entry;
    int ingredientIndex;

    if (!Tasks_GetVisibleRecipeCount(tasks)) {
        std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("No recipes available", "暂无可用配方"));
        return;
    }

    entry = RecipeCatalog_Get(recipe);
    if (entry != NULL && TasksRuntime_IsRecipeCompleted(tasks, NULL, player, recipe)) {
        std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Already ready in pack", "已在背包中就绪"));
        return;
    }
    if (recipe == RECIPE_REINFORCED_METAL && player != NULL && player->attackBonus >= 8.0f) {
        std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Frame service complete", "框架维护已完成"));
        return;
    }

    if (UIInventory_IsRecipeCraftable(tasks, player, recipe)) {
        std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Ready to craft", "可立即制作"));
        return;
    }

    if (entry != NULL && (entry->requiresWorkbench || entry->requiresLowStress)
        && (!Tasks_IsNearWorkbench(player) || !Player_CanCraftAdvanced(player))) {
        if (!Tasks_IsNearWorkbench(player)) {
            std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Move next to the workbench", "请靠近工作台"));
        } else {
            std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Stabilize health, oxygen, and anomalies", "请先稳定生命、氧气与异常状态"));
        }
        return;
    }

    if (entry != NULL && player != NULL) {
        for (ingredientIndex = 0; ingredientIndex < entry->ingredientCount; ingredientIndex++) {
            const RecipeIngredient *ingredient;

            ingredient = &entry->ingredients[ingredientIndex];
            if (!Player_HasResources(player, ingredient->resource, ingredient->amount)) {
                std::snprintf(buffer,
                              bufferSize,
                              "%s: %s %d/%d",
                              Loc_PickLiteral("Missing", "缺少"),
                              Player_GetResourceLabel(ingredient->resource),
                              player->resources[ingredient->resource],
                              ingredient->amount);
                return;
            }
        }
    }

    std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Missing materials", "材料不足"));
}
