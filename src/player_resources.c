#include "player.h"

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
            return "Wood";
        case RESOURCE_ORE:
            return "Ore";
        case RESOURCE_METAL_SCRAP:
            return "Metal Scrap";
        case RESOURCE_FRUIT:
            return "Plant Fruit";
        case RESOURCE_SPECIAL_FUNGUS:
            return "Special Fungus";
        case RESOURCE_ENERGY_CORE:
            return "Energy Core";
        case RESOURCE_GLOW_MOSS:
            return "Glow Moss";
        case RESOURCE_ALIEN_VINE:
            return "Alien Vine";
        case RESOURCE_SHELL_FRUIT:
            return "Shell Fruit";
        case RESOURCE_JUNK_METAL:
            return "Junk Metal";
        case RESOURCE_ENERGY_CRYSTAL:
            return "Energy Crystal";
        case RESOURCE_CALM_MUSHROOM:
            return "Calming Mushroom";
        case RESOURCE_PROTECTIVE_FIBER:
            return "Protective Fiber";
        case RESOURCE_RELIC_FRAGMENT:
            return "Relic Fragment";
        case RESOURCE_BOSS_SCALE:
            return "Boss Scale";
        case RESOURCE_ALIEN_SLIME:
            return "Alien Slime";
        case RESOURCE_COUNT:
        default:
            return "Unknown Resource";
    }
}

const char *Player_GetRecipeName(RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
        return "Unknown Recipe";
    }

    return entry->name;
}

const char *Player_GetRecipeSummary(RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
        return "";
    }

    return entry->summary;
}
