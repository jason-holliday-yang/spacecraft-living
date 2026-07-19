#include "map.h"
#include "player_presentation.h"
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
    GameMap map = {0};
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
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_PROTECTION_SUIT),
            "completed protection suit should no longer be craftable");
    memset(message, 0, sizeof(message));
    Require(!Tasks_TryCraft(&tasks, &map, &player, RECIPE_PROTECTION_SUIT, message, sizeof(message)),
            "completed protection suit should not consume another set of resources");
    Require(strstr(message, "already complete") != NULL,
            "duplicate gear crafting should explain that the build is already complete");

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
    player.resources[RESOURCE_JUNK_METAL] = 1;
    player.resources[RESOURCE_METAL_SCRAP] = 2;
    player.attackBonus = 0.0f;
    memset(message, 0, sizeof(message));
    Require(Tasks_CanCraftRecipe(&tasks, &player, RECIPE_REINFORCED_METAL),
            "weapon calibration should be craftable once advanced crafting is stable and materials are available");
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_REINFORCED_METAL, message, sizeof(message)),
            "weapon calibration should complete a valid bench-side weapon-frame service pass");
    Require(player.attackBonus == 2.0f,
            "weapon calibration should still grant a meaningful in-run attack bonus");
    Require(player.resources[RESOURCE_JUNK_METAL] == 0 && player.resources[RESOURCE_METAL_SCRAP] == 0,
            "weapon calibration should spend its listed metal materials");
    Require(strstr(message, "Weapon frame serviced") != NULL,
            "weapon calibration should explain the alignment-and-readiness payoff instead of a permanent growth route");

    player.resources[RESOURCE_METAL_SCRAP] = 1;
    player.resources[RESOURCE_ORE] = 2;
    Require(Tasks_CanCraftRecipe(&tasks, &player, RECIPE_LASER_GUN),
            "laser gun should use metal scrap plus ore according to the shared recipe catalog");
    Require(strcmp(Player_GetRecipeSummary(RECIPE_LASER_GUN), "1 Metal Scrap + 2 Ore. Core ranged weapon.") == 0,
            "laser gun summary should match the actual crafting resources");
    Require(strcmp(RecipeCatalog_Get(RECIPE_LASER_GUN)->ingredientText.english, "1 Metal Scrap, 2 Ore") == 0,
            "laser gun ingredient text should stay aligned with the craft logic");

    player.attackBonus = 8.0f;
    player.resources[RESOURCE_JUNK_METAL] = 1;
    player.resources[RESOURCE_METAL_SCRAP] = 2;
    memset(message, 0, sizeof(message));
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_REINFORCED_METAL),
            "weapon calibration should stop once the metal tuning cap is reached");
    Require(!Tasks_TryCraft(&tasks, &map, &player, RECIPE_REINFORCED_METAL, message, sizeof(message)),
            "capped weapon calibration should not consume another set of materials");
    Require(strstr(message, "already fully serviced") != NULL,
            "capped weapon calibration should explain that this material-set maintenance pass is already exhausted");

    tasks.stage = 3;
    player.gridX = WORKBENCH_X - 1;
    player.gridY = WORKBENCH_Y;
    player.oxygen = 22.0f;
    player.resources[RESOURCE_FRUIT] = 1;
    player.resources[RESOURCE_SHELL_FRUIT] = 1;
    player.resources[RESOURCE_SPECIAL_FUNGUS] = 1;
    player.resources[RESOURCE_GLOW_MOSS] = 1;
    player.resources[RESOURCE_CALM_MUSHROOM] = 1;
    memset(message, 0, sizeof(message));
    Require(Tasks_IsRecipeVisible(&tasks, RECIPE_RECOVERY_RATION),
            "recovery ration should be visible once outdoor preparation begins");
    Require(Tasks_CanCraftRecipe(&tasks, &player, RECIPE_RECOVERY_RATION),
            "recovery ration should craft from processed food ingredients at the workbench");
    Require(strcmp(Player_GetRecipeSummary(RECIPE_RECOVERY_RATION), "Fruit + Shell Fruit + Fungus + Glow Moss + Calm Mushroom. One sealed ration for healing, detox, and oxygen recovery.") == 0,
            "recovery ration summary should match the actual crafting resources");
    Require(strcmp(RecipeCatalog_Get(RECIPE_RECOVERY_RATION)->ingredientText.english, "1 Plant Fruit, 1 Shell Fruit, 1 Special Fungus, 1 Glow Moss, 1 Calm Mushroom") == 0,
            "recovery ration ingredient text should stay aligned with the craft logic");
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_RECOVERY_RATION, message, sizeof(message)),
            "recovery ration crafting should succeed with listed materials");
    Require(player.resources[RESOURCE_RECOVERY_RATION] == 1,
            "recovery ration crafting should add a usable consumable");
    Require(player.resources[RESOURCE_FRUIT] == 0
                && player.resources[RESOURCE_SHELL_FRUIT] == 0
                && player.resources[RESOURCE_SPECIAL_FUNGUS] == 0
                && player.resources[RESOURCE_GLOW_MOSS] == 0
                && player.resources[RESOURCE_CALM_MUSHROOM] == 0,
            "recovery ration crafting should spend its listed materials");
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_RECOVERY_RATION),
            "recovery ration should require another material set for repeat crafting");
    Player_SetStatus(&player, PLAYER_STATUS_LOW_OXYGEN, 1, 12.0f, player.oxygen);
    memset(message, 0, sizeof(message));
    Require(Player_UseSelectedConsumable(&player, RESOURCE_RECOVERY_RATION, message, sizeof(message)),
            "crafted recovery ration should be usable from the backpack");
    Require(player.resources[RESOURCE_RECOVERY_RATION] == 0,
            "using a recovery ration should consume one ration");
    Require(player.oxygen > 55.0f,
            "recovery ration should provide a strong outdoor oxygen refill");
    Require(Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_RESERVE),
            "recovery ration should grant an oxygen reserve status");
    Require(!Player_HasStatus(&player, PLAYER_STATUS_LOW_OXYGEN),
            "recovery ration should clear low oxygen when the refill is enough");
    Require(strstr(message, "Recovery Ration") != NULL,
            "recovery ration use text should identify the item");

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
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_ROPE),
            "completed rope should not remain craftable");

    tasks.stage = 5;
    player.resources[RESOURCE_WOOD] = 5;
    memset(message, 0, sizeof(message));
    Require(Tasks_TryCraft(&tasks, &map, &player, RECIPE_FIELD_CAMP, message, sizeof(message)),
            "field camp crafting should succeed once the stage 5 recipe resources are available");
    Require(player.hasFieldCamp, "field camp crafting should grant the outdoor recovery tool");
    Require(map.campPlaced, "field camp crafting should place the camp on the map");
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_FIELD_CAMP),
            "completed field camp should not remain craftable");
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
    Require(!Tasks_CanCraftRecipe(&tasks, &player, RECIPE_SIGNAL_AMPLIFIER),
            "completed signal amplifier should not remain craftable");

    puts("task_recipe smoke ok");
    return 0;
}
