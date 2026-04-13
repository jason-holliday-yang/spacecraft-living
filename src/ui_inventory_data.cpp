#include "ui_inventory_internal.h"

#include <cstdio>

static const BackpackEntryDef kBackpackEntries[BACKPACK_ENTRY_COUNT] = {
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_WOOD, "Wood", "Basic construction timber. Used in early repairs, rope work, glow sticks, and field camp setup.", Color{166, 121, 84, 255}, Color{108, 74, 46, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ORE, "Ore", "Dense mineral chunks refined into advanced tools, especially the laser gun.", Color{145, 156, 170, 255}, Color{88, 103, 118, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_METAL_SCRAP, "Metal Scrap", "Recovered ship fragments used for oxygen repairs and reinforced fabrication.", Color{158, 171, 184, 255}, Color{98, 113, 128, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_FRUIT, "Plant Fruit", "Quick field food. Restores health and pairs well with shell fruit for a stronger ration combo.", Color{238, 127, 93, 255}, Color{142, 69, 49, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_SPECIAL_FUNGUS, "Special Fungus", "Unstable fungal growth that eases poison buildup and can calm oxygen-leak anomalies.", Color{164, 121, 220, 255}, Color{101, 70, 150, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ENERGY_CORE, "Energy Core", "High-value power source needed for major system restoration and peaceful rescue tech.", Color{255, 207, 96, 255}, Color{185, 126, 45, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_GLOW_MOSS, "Glow Moss", "Bioluminescent material that restores oxygen, grants filtered breathing, and helps stabilize deep-swamp or ruins retreats.", Color{119, 234, 170, 255}, Color{54, 144, 95, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ALIEN_VINE, "Alien Vine", "Flexible organic fiber used in rope, suit work, and traversal support.", Color{98, 204, 132, 255}, Color{44, 122, 67, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_SHELL_FRUIT, "Shell Fruit", "Dense ration fruit that restores health, adds oxygen, and grants a reserve buffer for longer swamp or ruins pushes.", Color{244, 188, 112, 255}, Color{151, 107, 55, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_JUNK_METAL, "Junk Metal", "Heavier industrial salvage needed for reinforced components.", Color{132, 141, 150, 255}, Color{72, 82, 91, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ENERGY_CRYSTAL, "Energy Crystal", "Charged crystal from deeper zones. A strong power-material signal source.", Color{103, 220, 255, 255}, Color{48, 116, 164, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_CALM_MUSHROOM, "Calming Mushroom", "Reliable field antidote that cuts poison and grants temporary filtered breathing.", Color{125, 178, 255, 255}, Color{71, 111, 188, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_PROTECTIVE_FIBER, "Protective Fiber", "Durable woven matter used to produce the protection suit.", Color{212, 233, 182, 255}, Color{128, 156, 90, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_RELIC_FRAGMENT, "Relic Fragment", "Ancient ruins material linked to the signal amplifier and final route prep.", Color{190, 224, 232, 255}, Color{106, 141, 152, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_BOSS_SCALE, "Boss Scale", "Rare trophy from the guardian. Proof of combat success in the final area.", Color{222, 126, 102, 255}, Color{139, 70, 58, 255}},
    {BACKPACK_ENTRY_RESOURCE, RESOURCE_ALIEN_SLIME, "Alien Slime", "Hazardous residue from deep creatures and late-zone encounters.", Color{108, 233, 195, 255}, Color{42, 139, 112, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_GLOW_STICK, "Glow Stick", "Portable light tool that improves visibility during dark phases.", Color{110, 231, 184, 255}, Color{63, 142, 110, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_ROPE, "Simple Rope", "Traversal tool used to convert specific swamp barriers into passable shortcuts.", Color{201, 161, 102, 255}, Color{116, 80, 45, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_LASER_GUN, "Laser Gun", "Primary ranged weapon for stronger monsters and the final guardian route.", Color{255, 117, 117, 255}, Color{142, 57, 57, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_PROTECTION_SUIT, "Protection Suit", "Filtered suit that reduces hazard oxygen drain plus poison and leak risk in dangerous regions.", Color{122, 176, 255, 255}, Color{68, 97, 173, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_SIGNAL_AMPLIFIER, "Signal Amplifier", "Unlocks the peaceful tower route without defeating the final boss, though the final oxygen push still needs preparation.", Color{255, 210, 104, 255}, Color{150, 104, 36, 255}},
    {BACKPACK_ENTRY_GEAR, BACKPACK_GEAR_FIELD_CAMP, "Field Camp", "Temporary outdoor recovery point that restores part of health and oxygen and grants Camp Recovery before the next route segment.", Color{205, 169, 129, 255}, Color{126, 89, 61, 255}}
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
    return entry->kind == BACKPACK_ENTRY_RESOURCE ? "Resource" : "Gear";
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
        std::snprintf(buffer, bufferSize, "Unknown");
        return;
    }

    if (entry->kind == BACKPACK_ENTRY_RESOURCE) {
        std::snprintf(buffer, bufferSize, "Quantity: %d", UIInventory_BackpackEntryGetCount(player, itemIndex));
        return;
    }

    std::snprintf(buffer, bufferSize, "Status: %s", UIInventory_BackpackEntryIsOwned(player, itemIndex) ? "Owned" : "Missing");
}

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
