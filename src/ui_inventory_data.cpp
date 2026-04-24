#include "ui_inventory_internal.h"

#include <cstdio>

#define LT(en, zh) LocalizedText{en, zh}

static const BackpackEntryDef kBackpackEntries[BACKPACK_ENTRY_COUNT] = {
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_WOOD, LT("Wood", "木材"), LT("Storm-dried structural timber scavenged from the wreck and nearby growths. Crude, dependable, and good for the first repairs, rope work, lamps, and camp frames.", "从残骸和附近植被里拆下的风干结构木料，粗糙却可靠，适合最早期的修补、绳索制作、照明和营地骨架。"), Color{166, 121, 84, 255}, Color{108, 74, 46, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ORE, LT("Ore", "矿石"), LT("Heavy mineral chunks cut from deeper ground. Once refined, they become the hard parts needed for serious tools and weapons.", "从更深地层剥离出的高密度矿块，精炼后能变成真正够硬的部件，用来制作高级工具和武器。"), Color{145, 156, 170, 255}, Color{88, 103, 118, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_METAL_SCRAP, LT("Metal Scrap", "金属残片"), LT("Bent ship plating and torn brackets salvaged from the crash. Useful anywhere a repair needs something stronger than improvised wood.", "从坠毁飞船上回收的弯曲舱板和断裂支架。凡是临时木料撑不住的地方，都得靠它补强。"), Color{158, 171, 184, 255}, Color{98, 113, 128, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_FRUIT, LT("Plant Fruit", "植物果实"), LT("Soft, water-rich fruit from the marsh edge. It will not last long in a pack, but it buys quick recovery when your body starts to fail.", "采自沼泽边缘的柔软果实，含水量高，不耐存放，却能在身体快撑不住时迅速顶一口气。"), Color{238, 127, 93, 255}, Color{142, 69, 49, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_SPECIAL_FUNGUS, LT("Special Fungus", "特殊菌株"), LT("An unstable fungal mass that reacts to toxins and leaking air paths. Hard to trust, but valuable when your suit and blood chemistry both start drifting.", "会对毒素和漏氧异常产生反应的不稳定菌株，不算让人安心，但当防护服和身体状态一起失控时，它很有价值。"), Color{164, 121, 220, 255}, Color{101, 70, 150, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ENERGY_CORE, LT("Energy Core", "能源核心"), LT("A sealed emergency power heart torn from the wreck. Rare, heavy, and essential for waking dead systems back into service.", "从残骸内部拆出的密封应急动力核心，稀有而沉重，是让停摆系统重新醒来的关键。"), Color{255, 207, 96, 255}, Color{185, 126, 45, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_GLOW_MOSS, LT("Glow Moss", "发光苔"), LT("Cold bioluminescent moss that breathes out a faint clean vapor when crushed. Good for emergency oxygen, filter support, and keeping panic at bay in dark ground.", "捏碎后会吐出微弱洁净气雾的冷色生物荧光苔，适合应急补氧、临时过滤，以及在黑暗地带稳住呼吸。"), Color{119, 234, 170, 255}, Color{54, 144, 95, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ALIEN_VINE, LT("Alien Vine", "异星藤蔓"), LT("Tough living fiber with an almost rubbery pull. Ideal for tying ladders, bracing gear, and forcing a path across wet broken ground.", "韧性惊人的活体纤维，拉伸时带着近似橡胶的回弹感，适合绑缚器材、加固装备，以及在湿滑断面上强行拉出一条路。"), Color{98, 204, 132, 255}, Color{44, 122, 67, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_SHELL_FRUIT, LT("Shell Fruit", "壳果"), LT("A dense fruit sealed behind a brittle outer shell. More filling than common field forage, with enough stored moisture to help breathing hold longer.", "外壳脆硬、果肉厚实的高密度果实，比普通野外采食更耐饿，内部储水也足以让呼吸多撑一段时间。"), Color{244, 188, 112, 255}, Color{151, 107, 55, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_JUNK_METAL, LT("Junk Metal", "废旧金属"), LT("Industrial-grade scrap heavy enough to hold shape under strain. Ugly material, but exactly what you want for braces, mounts, and reinforced assemblies.", "足够在高压下保持形状的工业废金属，不好看，却正适合拿来做支架、底座和强化结构。"), Color{132, 141, 150, 255}, Color{72, 82, 91, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ENERGY_CRYSTAL, LT("Energy Crystal", "能量晶体"), LT("A charged crystal harvested from deeper terrain. Its pulse can wake dormant housings and feed power into machinery that should have stayed dead.", "从深层地带获得的带电晶体，脉冲足以唤醒休眠舱体，也能把能量送进那些本该彻底死掉的机械里。"), Color{103, 220, 255, 255}, Color{48, 116, 164, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_CALM_MUSHROOM, LT("Calming Mushroom", "宁神蘑菇"), LT("A pale medicinal mushroom that settles breathing and cuts poison spread. Unpleasant taste, reliable result.", "颜色苍白的药用蘑菇，能安抚呼吸节律并压住毒素扩散，味道难吃，但效果可靠。"), Color{125, 178, 255, 255}, Color{71, 111, 188, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_PROTECTIVE_FIBER, LT("Protective Fiber", "防护纤维"), LT("Dense woven material recovered from resistant growths and old insulation stock. Tough enough to become the breathing layer of a field suit.", "从耐受性植质和旧绝缘材料中提炼出的高密编织纤维，强度足以成为野外防护服的过滤内层。"), Color{212, 233, 182, 255}, Color{128, 156, 90, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_RELIC_FRAGMENT, LT("Relic Fragment", "遗迹碎片"), LT("Patterned shards from the old ruins. Their surfaces still carry routing data useful for the tower and the final decision ahead.", "来自古老遗迹的刻纹碎片，表面仍保留着可被读取的路径数据，与塔楼以及最终选择直接相关。"), Color{190, 224, 232, 255}, Color{106, 141, 152, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_BOSS_SCALE, LT("Boss Scale", "守卫鳞片"), LT("A scorched plate torn from the guardian. Heavy proof that the most dangerous thing in the ruins can, in fact, be brought down.", "从守卫身上剥落的焦灼鳞片，沉得像一块战斗后的结论，证明遗迹中最危险的存在并非不可击倒。"), Color{222, 126, 102, 255}, Color{139, 70, 58, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ALIEN_SLIME, LT("Alien Slime", "异星黏液"), LT("Caustic residue left by deep organisms and late-zone encounters. Sticky, reactive, and best handled as a hazard first, material second.", "深层生物和后段区域遭遇留下的腐蚀性黏液，附着力强、反应活跃，首先是危险源，其次才勉强算材料。"), Color{108, 233, 195, 255}, Color{42, 139, 112, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_GLOW_STICK, LT("Glow Stick", "荧光棒"), LT("A sealed improvised lamp bright enough to cut through ship-dark corridors, swamp fog, and any stretch where the planet tries to swallow your sight.", "密封式临时照明棒，亮度足以劈开飞船深处的黑暗、沼泽雾气，以及所有试图吞掉视线的路段。"), Color{110, 231, 184, 255}, Color{63, 142, 110, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_ROPE, LT("Simple Rope", "简易绳索"), LT("Hand-twisted field rope for tying off unstable ground. With a good anchor, it turns certain swamp hazards into something survivable.", "手工拧成的野外绳索，专门用来拴住不稳定地形。只要找到合适着力点，就能把某些沼泽险段变成可以通过的路。"), Color{201, 161, 102, 255}, Color{116, 80, 45, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_LASER_GUN, LT("Laser Gun", "激光枪"), LT("A rebuilt ship weapon meant for targets too large, too armored, or too relentless to handle up close.", "重组后的飞船武器，专门应对那些体型太大、外壳太硬、或逼近速度太快而不适合近身处理的目标。"), Color{255, 117, 117, 255}, Color{142, 57, 57, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_PROTECTION_SUIT, LT("Protection Suit", "防护服"), LT("A patched filtration suit that buys cleaner breaths in poisoned wetlands, ruined chambers, and any place where the air itself wants you gone.", "经过拼补的过滤防护服，能让你在毒雾沼地、遗迹深处，以及所有连空气都想把你赶走的地方多争取几口干净呼吸。"), Color{122, 176, 255, 255}, Color{68, 97, 173, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_SIGNAL_AMPLIFIER, LT("Signal Amplifier", "信号放大器"), LT("A precision relay built from mapped fragments. It gives the tower a way to steady the system instead of forcing the guardian route head-on.", "利用已解读碎片组装的精密中继装置，能让塔楼通过稳住整套系统来完成救援，而不是硬闯守卫路线。"), Color{255, 210, 104, 255}, Color{150, 104, 36, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_FIELD_CAMP, LT("Field Camp", "野外营地"), LT("A compact shelter frame and supply cache for dangerous pushes. Not home, not safety, but enough warmth and order to keep one more expedition alive.", "由简易支架和补给箱构成的临时据点，专为高风险推进准备。它算不上家，也谈不上真正安全，但足以让下一次深入还有机会活着回来。"), Color{205, 169, 129, 255}, Color{126, 89, 61, 255}}
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
