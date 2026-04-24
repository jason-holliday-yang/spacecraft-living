#include "recipe_catalog.h"

#include <array>
#include <cstddef>

namespace {

#define LT(en, zh) LocalizedText{en, zh}

const std::array<RecipeCatalogEntry, RECIPE_COUNT> kRecipeCatalog = {{
    {
        RECIPE_GLOW_STICK,
        LT("Glow Stick", "荧光棒"),
        LT("1 Wood + 1 Glow Moss. A hand lamp for darkness and fog.", "1 木材 + 1 发光苔。做成能穿透黑暗与雾气的手持光源。"),
        LT("A sealed field lamp that keeps ship corridors, marsh nights, and low-visibility pockets from closing over you.", "密封式野外照明器，能让飞船走廊、沼泽夜路和低能见度区域不至于把你彻底吞没。"),
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
        LT("1 Wood + 2 Vines. Lash a safer line through swamp hazards.", "1 木材 + 2 异星藤蔓。绑出一条更安全的沼泽通路。"),
        LT("A rough traversal line for tying off unstable ground and converting marked swamp barriers into usable shortcuts.", "粗制但好用的穿越工具，可以固定不稳定地形，把特定沼泽阻隔改造成可用捷径。"),
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
        LT("1 Junk Metal + 2 Metal Scrap. Forge braced parts for advanced gear.", "1 废旧金属 + 2 金属残片。锻成用于高级装备的承压部件。"),
        LT("Workbench-forged reinforced parts that can survive real load and heat, clearing the way for the laser weapon build.", "必须在工作台上成形的强化构件，能扛住真正的冲击和热量，是激光武器装配前的关键步骤。"),
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
        LT("A rebuilt ranged weapon for high-threat zones, deep salvage work, and the guardian route where getting close stops being an option.", "重组后的远程武器，适合高威胁区域、深层打捞，以及那条一旦靠近就会出事的守卫路线。"),
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
        LT("1 Wood + 2 Vines + 1 Fiber. Patch together a filtered field suit.", "1 木材 + 2 异星藤蔓 + 1 防护纤维。拼出一套可过滤危险空气的野外防护服。"),
        LT("A patched survival suit that slows oxygen loss and cuts poison and leak risk in wetlands, ruins, and other hostile air pockets.", "拼补成型的生存防护服，可在沼地、遗迹和其他恶劣空气区域里减缓氧气流失，并降低中毒与漏氧风险。"),
        LT("1 Wood, 2 Vines, 1 Protective Fiber", "1 木材，2 异星藤蔓，1 防护纤维"),
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
        LT("3 Relic Fragments. Build the relay that calms the tower.", "3 遗迹碎片。做出那台能让塔楼安静下来的中继装置。"),
        LT("A precision relay assembled from mapped ruin fragments, used to stabilize the tower and complete the peaceful route without forcing the guardian fight.", "利用已解读的遗迹碎片组装而成的精密中继器，可稳定塔楼，在不强行触发守卫战的情况下完成和平路线。"),
        LT("3 Relic Fragments", "3 遗迹碎片"),
        0,
        true,
        true,
        true,
        1,
        {
            {RESOURCE_RELIC_FRAGMENT, 3}
        }
    },
    {
        RECIPE_FIELD_CAMP,
        LT("Field Camp", "野外营地"),
        LT("5 Wood. Build a fallback shelter for partial health/oxygen recovery and Camp Recovery.", "5 木材。搭一个可部分恢复生命/氧气并提供营地恢复的回撤据点。"),
        LT("A deployable outdoor shelter that restores part of your health and oxygen during long expeditions and gives you one safer place to regroup.", "可部署的户外据点，适合在长距离探索中恢复部分生命与氧气，让你在前线多一个能缓口气的地方。"),
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
