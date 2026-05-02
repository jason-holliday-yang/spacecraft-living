#include "ui_inventory_internal.h"

#include "localization.h"
#include "task_runtime_internal.h"
#include "ui_runtime_internal.h"

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
    float titleX;
    float titleY;
    Vector2 hintSize;
    float hintX;
    float hintY;

    titleX = panel.x + 26.0f * scale;
    titleY = panel.y + 22.0f * scale;
    hintSize = UIRuntime_MeasureText(assets, closeHint, closeHintSize);
    hintX = panel.x + panel.width - hintSize.x - 26.0f * scale;
    hintY = panel.y + 30.0f * scale;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{5, 9, 16, 190});
    UIRuntime_DrawPanel(panel, Color{8, 18, 30, 245}, Color{153, 226, 255, 75});
    UIRuntime_DrawText(assets, title, Vector2{titleX, titleY}, titleSize, WHITE);
    UIRuntime_DrawText(assets, closeHint, Vector2{hintX, hintY}, closeHintSize, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets,
                              subtitle,
                              Rectangle{panel.x + 26.0f * scale, panel.y + 62.0f * scale, panel.width - 52.0f * scale, 28.0f * scale},
                              subtitleSize,
                              subtitleLineSpacing,
                              Color{194, 224, 255, 255});
    UIRuntime_DrawPanel(listPanel, Color{11, 20, 32, 230}, Color{255, 255, 255, 20});
    UIRuntime_DrawPanel(detailPanel, Color{14, 26, 42, 220}, Color{255, 255, 255, 22});
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
