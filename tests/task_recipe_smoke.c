#include "map.h"
#include "player.h"
#include "recipe_catalog.h"
#include "task_system.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "task_recipe_smoke failed: %s\n", message);
    exit(1);
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    char message[256];

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);

    player.gridX = WORKBENCH_X - 1;
    player.gridY = WORKBENCH_Y;
    player.facingX = 0;
    player.facingY = -1;
    Require(!Player_Move(&player, &map, 1, 0),
            "player should not move into blocking workbench tiles");
    Require(player.gridX == WORKBENCH_X - 1 && player.gridY == WORKBENCH_Y,
            "blocked movement should keep the player on the original tile");
    Require(player.facingX == 1 && player.facingY == 0,
            "blocked movement should still update facing direction");

    tasks.stage = 4;
    player.gridX = WORKBENCH_X - 1;
    player.gridY = WORKBENCH_Y;
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.resources[RESOURCE_WOOD] = 4;
    player.resources[RESOURCE_ALIEN_VINE] = 4;
    player.resources[RESOURCE_PROTECTIVE_FIBER] = 1;

    Require(Tasks_IsRecipeVisible(&tasks, RECIPE_PROTECTION_SUIT),
            "protection suit should be visible at stage 4");
    Require(Tasks_CanCraftRecipe(&tasks, &player, RECIPE_PROTECTION_SUIT),
            "protection suit should be craftable at stage 4 with resources and workbench access");
    memset(message, 0, sizeof(message));
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_PROTECTION_SUIT, message, sizeof(message)),
            "protection suit crafting should succeed at stage 4");
    Require(player.hasProtectionSuit, "protection suit flag should be set after crafting");

    tasks.stage = 3;
    player.resources[RESOURCE_JUNK_METAL] = 1;
    player.resources[RESOURCE_METAL_SCRAP] = 2;
    Require(!Tasks_IsRecipeVisible(&tasks, RECIPE_REINFORCED_METAL),
            "reinforced metal should stay locked before stage 4");

    tasks.stage = 4;
    player.oxygen = 26.0f;
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_REINFORCED_METAL),
            "advanced crafting should reject unstable oxygen");

    player.oxygen = INITIAL_OXYGEN;
    player.resources[RESOURCE_METAL_SCRAP] = 1;
    player.resources[RESOURCE_ORE] = 2;
    Require(Tasks_CanCraftRecipe(&tasks, &player, RECIPE_LASER_GUN),
            "laser gun should use metal scrap plus ore according to the shared recipe catalog");
    Require(strcmp(Player_GetRecipeSummary(RECIPE_LASER_GUN), "1 Metal Scrap + 2 Ore. Core ranged weapon.") == 0,
            "laser gun summary should match the actual crafting resources");
    Require(strcmp(RecipeCatalog_Get(RECIPE_LASER_GUN)->ingredientText.english, "1 Metal Scrap, 2 Ore") == 0,
            "laser gun ingredient text should stay aligned with the craft logic");

    tasks.stage = 3;
    player.gridX = WORKBENCH_X - 1;
    player.gridY = WORKBENCH_Y;
    player.resources[RESOURCE_WOOD] = 2;
    player.resources[RESOURCE_ALIEN_VINE] = 2;
    memset(message, 0, sizeof(message));
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_ROPE, message, sizeof(message)),
            "rope crafting should succeed once the stage 3 recipe resources are available");
    Require(player.hasRope, "rope crafting should grant the traversal tool");
    Require(strstr(message, "shortcut") != NULL && strstr(message, "F") != NULL,
            "rope crafting text should explain how the traversal shortcut works");

    tasks.stage = 5;
    player.resources[RESOURCE_WOOD] = 5;
    memset(message, 0, sizeof(message));
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_FIELD_CAMP, message, sizeof(message)),
            "field camp crafting should succeed once the stage 5 recipe resources are available");
    Require(player.hasFieldCamp, "field camp crafting should grant the outdoor recovery tool");
    Require(map.campPlaced, "field camp crafting should place the camp on the map");
    Require(strstr(message, "health") != NULL && strstr(message, "Camp Recovery") != NULL,
            "field camp crafting text should explain the outdoor health and oxygen recovery role");
    Require(strstr(Player_GetRecipeSummary(RECIPE_FIELD_CAMP), "Camp Recovery") != NULL,
            "field camp recipe summary should reflect the new recovery buff");
    Require(strstr(Player_GetRecipeSummary(RECIPE_PROTECTION_SUIT), "filtered") != NULL
                || strstr(Player_GetRecipeSummary(RECIPE_PROTECTION_SUIT), "Filtered") != NULL,
            "protection suit recipe summary should describe filtered protection");
    tasks.amplifierUnlocked = true;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    memset(message, 0, sizeof(message));
    Require(Tasks_IsRecipeVisible(&tasks, RECIPE_SIGNAL_AMPLIFIER),
            "signal amplifier should become visible after Loxi unlocks the peaceful route");
    Require(Tasks_CanCraftRecipe(&tasks, &player, RECIPE_SIGNAL_AMPLIFIER),
            "signal amplifier should now craft from relic fragments alone once unlocked");
    Require(strcmp(RecipeCatalog_Get(RECIPE_SIGNAL_AMPLIFIER)->ingredientText.english, "3 Relic Fragments") == 0,
            "signal amplifier ingredient text should match the simplified relic-fragment recipe");
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_SIGNAL_AMPLIFIER, message, sizeof(message)),
            "signal amplifier crafting should succeed once three relic fragments are available");
    Require(player.hasSignalAmplifier,
            "signal amplifier crafting should grant the peaceful-route device");

    puts("task_recipe smoke ok");
    return 0;
}
