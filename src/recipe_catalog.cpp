#include "recipe_catalog.h"

#include <array>
#include <cstddef>

namespace {

#define LT(en, zh) LocalizedText{en, zh}

const std::array<RecipeCatalogEntry, RECIPE_COUNT> kRecipeCatalog = {{
    {
        RECIPE_GLOW_STICK,
        LT("Glow Stick", "荧光棒"),
        LT("1 Wood + 1 Glow Moss. Expands night vision.", "1 木材 + 1 发光苔。提升夜间视野。"),
        LT("Portable light tool for dark phases and cave-like low-visibility stretches.", "便携照明工具，适合夜晚与低能见度区域。"),
        LT("1 Wood, 1 Glow Moss", "1 木材，1 发光苔"),
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
        LT("Simple Rope", "简易绳索"),
        LT("1 Wood + 2 Vines. Crosses swamp and solves shortcuts.", "1 木材 + 2 藤蔓。穿越沼泽并开启捷径。"),
        LT("Traversal utility that converts marked swamp barriers into shortcuts.", "用于穿越地形，可把特定沼泽障碍变成捷径。"),
        LT("1 Wood, 2 Alien Vines", "1 木材，2 异星藤蔓"),
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
        LT("Reinforced Metal", "强化金属"),
        LT("1 Junk Metal + 2 Metal Scrap. Prepares laser parts.", "1 废旧金属 + 2 金属残片。用于准备激光部件。"),
        LT("Workbench-forged parts that prepare the laser-gun build path.", "在工作台打造的强化部件，是制作激光枪的前置材料。"),
        LT("1 Junk Metal, 2 Metal Scrap", "1 废旧金属，2 金属残片"),
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
        LT("Laser Gun", "激光枪"),
        LT("1 Metal Scrap + 2 Ore. Core ranged weapon.", "1 金属残片 + 2 矿石。核心远程武器。"),
        LT("Core ranged weapon for high-threat zones and the guardian route.", "用于高威胁区域与守卫路线的核心远程武器。"),
        LT("1 Metal Scrap, 2 Ore", "1 金属残片，2 矿石"),
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
        LT("Protection Suit", "防护服"),
        LT("1 Wood + 2 Vines + 1 Fiber. Grants filtered protection in dangerous zones.", "1 木材 + 2 藤蔓 + 1 防护纤维。可在危险区域提供过滤防护。"),
        LT("Protective equipment that cuts hazard oxygen drain and lowers poison and leak risk.", "降低危险区域的氧气消耗，并减少中毒与漏氧风险。"),
        LT("1 Wood, 2 Vines, 1 Protective Fiber", "1 木材，2 藤蔓，1 防护纤维"),
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
        LT("Signal Amplifier", "信号放大器"),
        LT("1 Energy Core + 3 Relic Fragments. Peaceful tower activation.", "1 能源核心 + 3 遗迹碎片。用于和平方式启动塔楼。"),
        LT("Final peaceful-route device for stabilizing the tower without the boss fight.", "和平路线的最终装置，可在不进行首领战的情况下稳定塔楼。"),
        LT("1 Energy Core, 3 Relic Fragments", "1 能源核心，3 遗迹碎片"),
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
        LT("Field Camp", "野外营地"),
        LT("5 Wood. Build an outdoor recovery stop with Camp Recovery support.", "5 木材。搭建户外恢复点，并获得营地恢复支持。"),
        LT("Deployable outdoor recovery point that restores part of health and oxygen during long expeditions.", "可部署的户外恢复点，适合长距离探索时回复部分生命与氧气。"),
        LT("5 Wood", "5 木材"),
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

#undef LT

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
