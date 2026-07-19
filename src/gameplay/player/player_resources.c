#include "player.h"


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
