#include "ui_inventory_internal.h"

#include <cstdio>

#define LT(en, zh) LocalizedText{en, zh}

static const BackpackEntryDef kBackpackEntries[BACKPACK_ENTRY_COUNT] = {
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_WOOD, LT("Wood", "木材"), LT("Basic construction timber. Used in early repairs, rope work, glow sticks, and field camp setup.", "基础建材，用于前期修理、绳索制作、荧光棒和野外营地搭建。"), Color{166, 121, 84, 255}, Color{108, 74, 46, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ORE, LT("Ore", "矿石"), LT("Dense mineral chunks refined into advanced tools, especially the laser gun.", "高密度矿石，可精炼为高级工具材料，尤其用于激光枪。"), Color{145, 156, 170, 255}, Color{88, 103, 118, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_METAL_SCRAP, LT("Metal Scrap", "金属残片"), LT("Recovered ship fragments used for oxygen repairs and reinforced fabrication.", "从飞船残骸中回收的材料，用于氧气系统修理和强化制造。"), Color{158, 171, 184, 255}, Color{98, 113, 128, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_FRUIT, LT("Plant Fruit", "植物果实"), LT("Quick field food. Restores health and pairs well with shell fruit for a stronger ration combo.", "便于携带的野外食物，可恢复生命值，并能与壳果搭配成更强补给。"), Color{238, 127, 93, 255}, Color{142, 69, 49, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_SPECIAL_FUNGUS, LT("Special Fungus", "特殊菌株"), LT("Unstable fungal growth that eases poison buildup and can calm oxygen-leak anomalies.", "不稳定的菌类生长体，可缓解毒素累积，也能安抚漏氧异常。"), Color{164, 121, 220, 255}, Color{101, 70, 150, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ENERGY_CORE, LT("Energy Core", "能源核心"), LT("High-value power source needed for major system restoration and deep-route recovery work.", "高价值能源部件，是修复关键系统与深层路线恢复工作的核心材料。"), Color{255, 207, 96, 255}, Color{185, 126, 45, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_GLOW_MOSS, LT("Glow Moss", "发光苔"), LT("Bioluminescent material that restores oxygen, grants filtered breathing, and helps stabilize deep-swamp or ruins retreats.", "生物发光材料，可恢复氧气、提供过滤呼吸，并帮助稳定深沼或遗迹撤离。"), Color{119, 234, 170, 255}, Color{54, 144, 95, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ALIEN_VINE, LT("Alien Vine", "异星藤蔓"), LT("Flexible organic fiber used in rope, suit work, and traversal support.", "柔韧的有机纤维，用于绳索、防护服与移动支援。"), Color{98, 204, 132, 255}, Color{44, 122, 67, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_SHELL_FRUIT, LT("Shell Fruit", "壳果"), LT("Dense ration fruit that restores health, adds oxygen, and grants a reserve buffer for longer swamp or ruins pushes.", "高密度口粮果实，可恢复生命、补充氧气，并为长线探索提供额外缓冲。"), Color{244, 188, 112, 255}, Color{151, 107, 55, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_JUNK_METAL, LT("Junk Metal", "废旧金属"), LT("Heavier industrial salvage needed for reinforced components.", "较重的工业废料，用于制作强化组件。"), Color{132, 141, 150, 255}, Color{72, 82, 91, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ENERGY_CRYSTAL, LT("Energy Crystal", "能量晶体"), LT("Charged crystal from deeper zones. A strong power-material signal source.", "来自更深区域的带电晶体，是强力能源材料与信号源。"), Color{103, 220, 255, 255}, Color{48, 116, 164, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_CALM_MUSHROOM, LT("Calming Mushroom", "宁神蘑菇"), LT("Reliable field antidote that cuts poison and grants temporary filtered breathing.", "可靠的野外解毒物，可削减毒性并提供短暂过滤呼吸。"), Color{125, 178, 255, 255}, Color{71, 111, 188, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_PROTECTIVE_FIBER, LT("Protective Fiber", "防护纤维"), LT("Durable woven matter used to produce the protection suit.", "耐久的编织材料，用于制作防护服。"), Color{212, 233, 182, 255}, Color{128, 156, 90, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_RELIC_FRAGMENT, LT("Relic Fragment", "遗迹碎片"), LT("Ancient ruins material linked to the signal amplifier and final route prep.", "来自古老遗迹的材料，与信号放大器和最终路线准备密切相关。"), Color{190, 224, 232, 255}, Color{106, 141, 152, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_BOSS_SCALE, LT("Boss Scale", "守卫鳞片"), LT("Rare trophy from the guardian. Proof of combat success in the final area.", "守卫掉落的稀有战利品，是最终区域战斗胜利的证明。"), Color{222, 126, 102, 255}, Color{139, 70, 58, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ALIEN_SLIME, LT("Alien Slime", "异星黏液"), LT("Hazardous residue from deep creatures and late-zone encounters.", "来自深层生物与后期区域遭遇的危险残留物。"), Color{108, 233, 195, 255}, Color{42, 139, 112, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_GLOW_STICK, LT("Glow Stick", "荧光棒"), LT("Portable light tool that improves visibility during dark phases.", "可携带照明工具，在黑暗阶段提升视野。"), Color{110, 231, 184, 255}, Color{63, 142, 110, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_ROPE, LT("Simple Rope", "简易绳索"), LT("Traversal tool used to convert specific swamp barriers into passable shortcuts.", "可把特定沼泽障碍变为可通行捷径的移动工具。"), Color{201, 161, 102, 255}, Color{116, 80, 45, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_LASER_GUN, LT("Laser Gun", "激光枪"), LT("Primary ranged weapon for stronger monsters and the final guardian route.", "用于应对强敌与最终守卫路线的主力远程武器。"), Color{255, 117, 117, 255}, Color{142, 57, 57, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_PROTECTION_SUIT, LT("Protection Suit", "防护服"), LT("Filtered suit that reduces hazard oxygen drain plus poison and leak risk in dangerous regions.", "带过滤功能的防护服，可减少危险区域的氧气消耗，并降低中毒与漏氧风险。"), Color{122, 176, 255, 255}, Color{68, 97, 173, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_SIGNAL_AMPLIFIER, LT("Signal Amplifier", "信号放大器"), LT("Unlocks the peaceful tower route without defeating the final boss, though the final oxygen push still needs preparation.", "无需击败最终首领即可开启和平塔楼路线，但最后的氧气推进仍需充分准备。"), Color{255, 210, 104, 255}, Color{150, 104, 36, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_FIELD_CAMP, LT("Field Camp", "野外营地"), LT("Temporary outdoor recovery point that restores part of health and oxygen and grants Camp Recovery before the next route segment.", "临时户外恢复点，可回复部分生命与氧气，并在下一段路线前提供营地恢复效果。"), Color{205, 169, 129, 255}, Color{126, 89, 61, 255}}
};

static bool Player_HasGear(const Player *player, BackpackGearId gearId) {
    switch (gearId) {
        case BACKPACK_GEAR_GLOW_STICK:
            return player->hasGlowStick;
        case BACKPACK_GEAR_ROPE:
            return player->hasRope;
        case BACKPACK_GEAR_LASER_GUN:
            return player->hasLaserGun;
        case BACKPACK_GEAR_PROTECTION_SUIT:
            return player->hasProtectionSuit;
        case BACKPACK_GEAR_SIGNAL_AMPLIFIER:
            return player->hasSignalAmplifier;
        case BACKPACK_GEAR_FIELD_CAMP:
            return player->hasFieldCamp;
        default:
            return false;
    }
}

const BackpackEntryDef *UIInventory_GetBackpackEntry(int itemIndex) {
    if (itemIndex < 0 || itemIndex >= BACKPACK_ENTRY_COUNT) {
        return NULL;
    }

    return &kBackpackEntries[itemIndex];
}

bool UIInventory_BackpackEntryIsOwned(const Player *player, int itemIndex) {
    const BackpackEntryDef *entry;

    entry = UIInventory_GetBackpackEntry(itemIndex);
    if (entry == NULL) {
        return false;
    }

    if (entry->kind == BACKPACK_ENTRY_RESOURCE) {
        return player->resources[entry->itemId] > 0;
    }

    return Player_HasGear(player, (BackpackGearId)entry->itemId);
}

int UIInventory_BackpackEntryGetCount(const Player *player, int itemIndex) {
    const BackpackEntryDef *entry;

    entry = UIInventory_GetBackpackEntry(itemIndex);
    if (entry == NULL) {
        return 0;
    }

    if (entry->kind == BACKPACK_ENTRY_RESOURCE) {
        return player->resources[entry->itemId];
    }

    return Player_HasGear(player, (BackpackGearId)entry->itemId) ? 1 : 0;
}

const char *UIInventory_BackpackEntryGetCategory(const BackpackEntryDef *entry) {
    return entry->kind == BACKPACK_ENTRY_RESOURCE
        ? Loc_PickLiteral("Resource", "资源")
        : Loc_PickLiteral("Gear", "装备");
}

bool UIInventory_BackpackEntryCanUseDirectly(int itemIndex) {
    switch (itemIndex) {
        case 3:
        case 4:
        case 6:
        case 8:
        case 11:
            return true;
        default:
            return false;
    }
}

void UIInventory_GetBackpackEntryStatus(const Player *player, int itemIndex, char *buffer, size_t bufferSize) {
    const BackpackEntryDef *entry;

    entry = UIInventory_GetBackpackEntry(itemIndex);
    if (entry == NULL) {
        std::snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("Unknown", "未知"));
        return;
    }

    if (entry->kind == BACKPACK_ENTRY_RESOURCE) {
        std::snprintf(buffer, bufferSize, "%s: %d", Loc_PickLiteral("Quantity", "数量"), UIInventory_BackpackEntryGetCount(player, itemIndex));
        return;
    }

    std::snprintf(buffer,
                  bufferSize,
                  "%s: %s",
                  Loc_PickLiteral("Status", "状态"),
                  UIInventory_BackpackEntryIsOwned(player, itemIndex)
                      ? Loc_PickLiteral("Owned", "已拥有")
                      : Loc_PickLiteral("Missing", "未拥有"));
}

#undef LT

RecipeVisualDef UIInventory_GetRecipeVisual(RecipeType recipe) {
    switch (recipe) {
        case RECIPE_GLOW_STICK:
            return RecipeVisualDef{Color{110, 231, 184, 255}, Color{63, 142, 110, 255}};
        case RECIPE_ROPE:
            return RecipeVisualDef{Color{201, 161, 102, 255}, Color{116, 80, 45, 255}};
        case RECIPE_REINFORCED_METAL:
            return RecipeVisualDef{Color{168, 182, 198, 255}, Color{92, 107, 125, 255}};
        case RECIPE_LASER_GUN:
            return RecipeVisualDef{Color{255, 117, 117, 255}, Color{142, 57, 57, 255}};
        case RECIPE_PROTECTION_SUIT:
            return RecipeVisualDef{Color{122, 176, 255, 255}, Color{68, 97, 173, 255}};
        case RECIPE_SIGNAL_AMPLIFIER:
            return RecipeVisualDef{Color{255, 210, 104, 255}, Color{150, 104, 36, 255}};
        case RECIPE_FIELD_CAMP:
            return RecipeVisualDef{Color{205, 169, 129, 255}, Color{126, 89, 61, 255}};
        case RECIPE_COUNT:
        default:
            return RecipeVisualDef{Color{180, 190, 205, 255}, Color{92, 104, 120, 255}};
    }
}
