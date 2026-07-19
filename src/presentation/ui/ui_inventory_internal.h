#ifndef UI_INVENTORY_INTERNAL_H
#define UI_INVENTORY_INTERNAL_H

#include "ui_system.h"

#include "localization.h"
#include "recipe_catalog.h"

typedef enum BackpackEntryKind {
    BACKPACK_ENTRY_RESOURCE = 0,
    BACKPACK_ENTRY_GEAR
} BackpackEntryKind;

typedef enum BackpackGearId {
    BACKPACK_GEAR_GLOW_STICK = 0,
    BACKPACK_GEAR_ROPE,
    BACKPACK_GEAR_LASER_GUN,
    BACKPACK_GEAR_PROTECTION_SUIT,
    BACKPACK_GEAR_SIGNAL_AMPLIFIER,
    BACKPACK_GEAR_FIELD_CAMP
} BackpackGearId;

typedef struct BackpackEntryDef {
    BackpackEntryKind kind;
    int itemId;
    LocalizedText name;
    LocalizedText description;
    Color primary;
    Color secondary;
} BackpackEntryDef;

typedef struct RecipeVisualDef {
    Color primary;
    Color secondary;
} RecipeVisualDef;

SCL_EXTERN_C_BEGIN

int UIInventory_NormalizeSelectedIndex(int selectedIndex, int itemCount);
const BackpackEntryDef *UIInventory_GetBackpackEntry(int itemIndex);
bool UIInventory_BackpackEntryIsOwned(const Player *player, int itemIndex);
int UIInventory_BackpackEntryGetCount(const Player *player, int itemIndex);
const char *UIInventory_BackpackEntryGetCategory(const BackpackEntryDef *entry);
bool UIInventory_BackpackEntryCanUseDirectly(int itemIndex);
void UIInventory_GetBackpackEntryStatus(const Player *player, int itemIndex, char *buffer, size_t bufferSize);
void UIInventory_DrawBackpackIcon(const AssetBundle *assets, int itemIndex, Rectangle rect, Color primary, Color secondary);
RecipeVisualDef UIInventory_GetRecipeVisual(RecipeType recipe);
bool UIInventory_IsRecipeCraftable(const TaskSystem *tasks, const Player *player, RecipeType recipe);
void UIInventory_GetRecipeStatusText(const TaskSystem *tasks, const Player *player, RecipeType recipe, char *buffer, size_t bufferSize);
void UIInventory_DrawRecipeIcon(const AssetBundle *assets, RecipeType recipe, Rectangle rect, Color primary, Color secondary);
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
                                  float subtitleLineSpacing);
void UIInventory_DrawStatusBadge(const AssetBundle *assets,
                                 Rectangle rect,
                                 const char *label,
                                 float fontSize,
                                 Color fill,
                                 Color outline,
                                 Color textColor);

SCL_EXTERN_C_END

#endif
