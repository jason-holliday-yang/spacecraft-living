#include "recipe_catalog.h"

#include <array>
#include <cstddef>

namespace {

const std::array<RecipeCatalogEntry, RECIPE_COUNT> kRecipeCatalog = {{
    {
        RECIPE_GLOW_STICK,
        "Glow Stick",
        "1 Wood + 1 Glow Moss. Expands night vision.",
        "Portable light tool for dark phases and cave-like low-visibility stretches.",
        "1 Wood, 1 Glow Moss",
        2,
        false,
        false,
        false,
        2,
        {
            {RESOURCE_WOOD, 1},
            {RESOURCE_GLOW_MOSS, 1}
        }
    },
    {
        RECIPE_ROPE,
        "Simple Rope",
        "1 Wood + 2 Vines. Crosses swamp and solves shortcuts.",
        "Traversal utility that converts marked swamp barriers into shortcuts.",
        "1 Wood, 2 Alien Vines",
        3,
        false,
        false,
        false,
        2,
        {
            {RESOURCE_WOOD, 1},
            {RESOURCE_ALIEN_VINE, 2}
        }
    },
    {
        RECIPE_REINFORCED_METAL,
        "Reinforced Metal",
        "1 Junk Metal + 2 Metal Scrap. Prepares laser parts.",
        "Workbench-forged parts that prepare the laser-gun build path.",
        "1 Junk Metal, 2 Metal Scrap",
        4,
        true,
        true,
        false,
        2,
        {
            {RESOURCE_JUNK_METAL, 1},
            {RESOURCE_METAL_SCRAP, 2}
        }
    },
    {
        RECIPE_LASER_GUN,
        "Laser Gun",
        "1 Metal Scrap + 2 Ore. Core ranged weapon.",
        "Core ranged weapon for high-threat zones and the guardian route.",
        "1 Metal Scrap, 2 Ore",
        4,
        true,
        true,
        false,
        2,
        {
            {RESOURCE_METAL_SCRAP, 1},
            {RESOURCE_ORE, 2}
        }
    },
    {
        RECIPE_PROTECTION_SUIT,
        "Protection Suit",
        "1 Wood + 2 Vines + 1 Fiber. Grants filtered protection in dangerous zones.",
        "Protective equipment that cuts hazard oxygen drain and lowers poison and leak risk.",
        "1 Wood, 2 Vines, 1 Protective Fiber",
        4,
        true,
        true,
        false,
        3,
        {
            {RESOURCE_WOOD, 1},
            {RESOURCE_ALIEN_VINE, 2},
            {RESOURCE_PROTECTIVE_FIBER, 1}
        }
    },
    {
        RECIPE_SIGNAL_AMPLIFIER,
        "Signal Amplifier",
        "1 Energy Core + 3 Relic Fragments. Peaceful tower activation.",
        "Final peaceful-route device for stabilizing the tower without the boss fight.",
        "1 Energy Core, 3 Relic Fragments",
        0,
        true,
        true,
        true,
        2,
        {
            {RESOURCE_ENERGY_CORE, 1},
            {RESOURCE_RELIC_FRAGMENT, 3}
        }
    },
    {
        RECIPE_FIELD_CAMP,
        "Field Camp",
        "5 Wood. Build an outdoor recovery stop with Camp Recovery support.",
        "Deployable outdoor recovery point that restores part of health and oxygen during long expeditions.",
        "5 Wood",
        BASE_CAMP_UNLOCK_STAGE,
        false,
        false,
        false,
        1,
        {
            {RESOURCE_WOOD, 5}
        }
    }
}};

const std::array<RecipeType, 7> kRecipeOrder = {{
    RECIPE_GLOW_STICK,
    RECIPE_ROPE,
    RECIPE_FIELD_CAMP,
    RECIPE_REINFORCED_METAL,
    RECIPE_LASER_GUN,
    RECIPE_PROTECTION_SUIT,
    RECIPE_SIGNAL_AMPLIFIER
}};

}  // namespace

const RecipeCatalogEntry *RecipeCatalog_Get(RecipeType recipe) {
    if (recipe < 0 || recipe >= RECIPE_COUNT) {
        return NULL;
    }

    return &kRecipeCatalog[recipe];
}

int RecipeCatalog_GetOrderedCount(void) {
    return static_cast<int>(kRecipeOrder.size());
}

RecipeType RecipeCatalog_GetOrderedAt(int index) {
    if (index < 0 || index >= RecipeCatalog_GetOrderedCount()) {
        return RECIPE_GLOW_STICK;
    }

    return kRecipeOrder[index];
}

bool RecipeCatalog_IsVisible(const RecipeCatalogEntry *entry, bool amplifierUnlocked, int stage) {
    if (entry == NULL) {
        return false;
    }

    if (entry->requiresAmplifierUnlock) {
        return amplifierUnlocked;
    }

    return stage >= entry->unlockStage;
}

bool RecipeCatalog_HasResources(const Player *player, RecipeType recipe) {
    const RecipeCatalogEntry *entry;
    int ingredientIndex;

    if (player == NULL) {
        return false;
    }

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
        return false;
    }

    for (ingredientIndex = 0; ingredientIndex < entry->ingredientCount; ingredientIndex++) {
        if (!Player_HasResources(player,
                                 entry->ingredients[ingredientIndex].resource,
                                 entry->ingredients[ingredientIndex].amount)) {
            return false;
        }
    }

    return true;
}

bool RecipeCatalog_SpendResources(Player *player, RecipeType recipe) {
    const RecipeCatalogEntry *entry;
    int ingredientIndex;

    if (player == NULL) {
        return false;
    }

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL || !RecipeCatalog_HasResources(player, recipe)) {
        return false;
    }

    for (ingredientIndex = 0; ingredientIndex < entry->ingredientCount; ingredientIndex++) {
        Player_SpendResource(player,
                             entry->ingredients[ingredientIndex].resource,
                             entry->ingredients[ingredientIndex].amount);
    }

    return true;
}
