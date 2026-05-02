#ifndef RECIPE_CATALOG_H
#define RECIPE_CATALOG_H

#include <stdbool.h>

#include "c_compat.h"
#include "config.h"
#include "localization.h"
#include "player.h"

/* Public recipe catalog lookup helpers shared by gameplay and UI layers. */

SCL_EXTERN_C_BEGIN

#define RECIPE_MAX_INGREDIENTS 5

typedef struct RecipeIngredient {
    ResourceType resource;
    int amount;
} RecipeIngredient;

typedef struct RecipeCatalogEntry {
    RecipeType recipe;
    LocalizedText name;
    LocalizedText summary;
    LocalizedText detailDescription;
    LocalizedText ingredientText;
    int unlockStage;
    bool requiresWorkbench;
    bool requiresLowStress;
    bool requiresAmplifierUnlock;
    int ingredientCount;
    RecipeIngredient ingredients[RECIPE_MAX_INGREDIENTS];
} RecipeCatalogEntry;

const RecipeCatalogEntry *RecipeCatalog_Get(RecipeType recipe);
int RecipeCatalog_GetOrderedCount(void);
RecipeType RecipeCatalog_GetOrderedAt(int index);
bool RecipeCatalog_IsVisible(const RecipeCatalogEntry *entry, bool amplifierUnlocked, int stage);
bool RecipeCatalog_HasResources(const Player *player, RecipeType recipe);
bool RecipeCatalog_SpendResources(Player *player, RecipeType recipe);

SCL_EXTERN_C_END

#endif
