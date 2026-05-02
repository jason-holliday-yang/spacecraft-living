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
        LT("Weapon Frame Tuning", "武器框架调校"),
        LT("1 Junk Metal + 2 Metal Scrap. Service the weapon frame at the bench.", "1 废旧金属 + 2 金属残片。在工作台对武器框架做一次维护调校。"),
        LT("A bench-side maintenance pass for the weapon frame. It restores alignment and keeps the ranged setup reliable, but it is not a permanent stat-growth route.", "在工作台完成的一次武器框架维护调校。它用于恢复对位、保证远程武器稳定运转，但不再是永久属性成长路线。"),
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
    },
    {
        RECIPE_RECOVERY_RATION,
        LT("Recovery Ration", "复苏口粮"),
        LT("Fruit + Shell Fruit + Fungus + Glow Moss + Calm Mushroom. One sealed ration for healing, detox, and oxygen recovery.", "植物果实 + 壳果 + 特殊菌株 + 发光苔 + 宁神蘑菇。封装成一份同时负责恢复、解毒与补氧的应急口粮。"),
        LT("A workbench-sealed survival ration that blends field fruit pulp, medicinal fungus, calming mushroom, and glow-moss vapor into one emergency pack. It is the only quick-use consumable now: one pack restores health, clears poison, stabilizes breathing, and pushes oxygen back up during a dangerous run.", "在工作台完成密封的综合生存口粮，把野外果浆、药用菌株、宁神蘑菇与发光苔气雾压进同一份应急包里。它现在就是唯一的快捷消耗品：一包下去，会同时恢复生命、清除中毒、稳定呼吸，并把氧气重新抬上来。"),
        LT("1 Plant Fruit, 1 Shell Fruit, 1 Special Fungus, 1 Glow Moss, 1 Calm Mushroom", "1 植物果实，1 壳果，1 特殊菌株，1 发光苔，1 宁神蘑菇"),
        BASE_CAMP_UNLOCK_STAGE,
        true,
        false,
        false,
        5,
        {
            {RESOURCE_FRUIT, 1},
            {RESOURCE_SHELL_FRUIT, 1},
            {RESOURCE_SPECIAL_FUNGUS, 1},
            {RESOURCE_GLOW_MOSS, 1},
            {RESOURCE_CALM_MUSHROOM, 1}
        }
    }
}};

const std::array<RecipeType, 8> kRecipeOrder = {{
    RECIPE_GLOW_STICK,
    RECIPE_ROPE,
    RECIPE_RECOVERY_RATION,
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
