#include "player.h"

#include "localization.h"
#include "recipe_catalog.h"

#include <stddef.h>

void Player_AddResource(Player *player, ResourceType resource, int amount) {
    if (resource < 0 || resource >= RESOURCE_COUNT || amount <= 0) {
        return;
    }
    player->resources[resource] += amount;
}

bool Player_SpendResource(Player *player, ResourceType resource, int amount) {
    if (resource < 0 || resource >= RESOURCE_COUNT || amount < 0) {
        return false;
    }
    if (player->resources[resource] < amount) {
        return false;
    }
    player->resources[resource] -= amount;
    return true;
}

bool Player_HasResources(const Player *player, ResourceType resource, int amount) {
    if (resource < 0 || resource >= RESOURCE_COUNT || amount < 0) {
        return false;
    }
    return player->resources[resource] >= amount;
}

const char *Player_GetResourceLabel(ResourceType resource) {
    switch (resource) {
        case RESOURCE_WOOD:
            return Loc_PickLiteral("Wood", "木材");
        case RESOURCE_ORE:
            return Loc_PickLiteral("Ore", "矿石");
        case RESOURCE_METAL_SCRAP:
            return Loc_PickLiteral("Metal Scrap", "金属残片");
        case RESOURCE_FRUIT:
            return Loc_PickLiteral("Plant Fruit", "植物果实");
        case RESOURCE_SPECIAL_FUNGUS:
            return Loc_PickLiteral("Special Fungus", "特殊菌株");
        case RESOURCE_ENERGY_CORE:
            return Loc_PickLiteral("Energy Core", "能源核心");
        case RESOURCE_GLOW_MOSS:
            return Loc_PickLiteral("Glow Moss", "发光苔");
        case RESOURCE_ALIEN_VINE:
            return Loc_PickLiteral("Alien Vine", "异星藤蔓");
        case RESOURCE_SHELL_FRUIT:
            return Loc_PickLiteral("Shell Fruit", "壳果");
        case RESOURCE_JUNK_METAL:
            return Loc_PickLiteral("Junk Metal", "废旧金属");
        case RESOURCE_ENERGY_CRYSTAL:
            return Loc_PickLiteral("Energy Crystal", "能量晶体");
        case RESOURCE_CALM_MUSHROOM:
            return Loc_PickLiteral("Calming Mushroom", "宁神蘑菇");
        case RESOURCE_PROTECTIVE_FIBER:
            return Loc_PickLiteral("Protective Fiber", "防护纤维");
        case RESOURCE_RELIC_FRAGMENT:
            return Loc_PickLiteral("Relic Fragment", "遗迹碎片");
        case RESOURCE_BOSS_SCALE:
            return Loc_PickLiteral("Boss Scale", "守卫鳞片");
        case RESOURCE_ALIEN_SLIME:
            return Loc_PickLiteral("Alien Slime", "异星黏液");
        case RESOURCE_COUNT:
        default:
            return Loc_PickLiteral("Unknown Resource", "未知资源");
    }
}

const char *Player_GetRecipeName(RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
        return Loc_PickLiteral("Unknown Recipe", "未知配方");
    }

    return Loc_PickText(entry->name);
}

const char *Player_GetRecipeSummary(RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
        return "";
    }

    return Loc_PickText(entry->summary);
}
