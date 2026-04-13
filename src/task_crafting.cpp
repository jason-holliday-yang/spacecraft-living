#include "task_runtime_internal.h"

bool Tasks_TryCraft(TaskSystem *tasks, GameMap *map, Player *player, RecipeType recipe, char *message, size_t messageSize) {
    if (tasks == NULL || map == NULL || player == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, "Crafting unavailable.");
        return false;
    }

    switch (recipe) {
        case RECIPE_GLOW_STICK:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasGlowStick = true;
            player->glowStickTimer = 60.0f;
            TasksRuntime_WriteMessage(message, messageSize, "Crafted Glow Stick. Night visibility increased for a while.");
            return true;
        case RECIPE_ROPE:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasRope = true;
            TasksRuntime_WriteMessage(message, messageSize, "Crafted Simple Rope. Face a swamp barrier and use F to turn it into a shortcut.");
            return true;
        case RECIPE_REINFORCED_METAL:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  "Advanced crafting requires the workbench and a stable enough condition to focus.",
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->resources[RESOURCE_METAL_SCRAP] += 1;
            TasksRuntime_WriteMessage(message, messageSize, "Crafted Reinforced Metal for the Laser Gun.");
            return true;
        case RECIPE_LASER_GUN:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  "The Laser Gun requires the workbench and a stable enough condition to assemble it.",
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasLaserGun = true;
            TasksRuntime_WriteMessage(message, messageSize, "Laser Gun assembled. Ranged combat is now available.");
            return true;
        case RECIPE_PROTECTION_SUIT:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  "The Protection Suit requires the workbench and a stable enough condition to assemble it.",
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasProtectionSuit = true;
            TasksRuntime_WriteMessage(message, messageSize, "Protection Suit crafted. Filtered breathing is online, reducing hazard oxygen drain plus poison and leak risk for deep-swamp and ruins pushes.");
            return true;
        case RECIPE_SIGNAL_AMPLIFIER:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  "The Signal Amplifier must be crafted at the workbench while your condition is stable.",
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasSignalAmplifier = true;
            TasksRuntime_WriteMessage(message, messageSize, "Signal Amplifier completed. You can now bypass the final boss, but the last tower approach still rewards stable oxygen prep.");
            return true;
        case RECIPE_FIELD_CAMP:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasFieldCamp = true;
            Map_SetFieldCamp(map, player->gridX, player->gridY);
            TasksRuntime_WriteMessage(message, messageSize, "Field Camp built. Rest beside it for partial health and oxygen recovery plus Camp Recovery as a safe outdoor fallback before another push.");
            return true;
        case RECIPE_COUNT:
        default:
            TasksRuntime_WriteMessage(message, messageSize, "Unknown recipe.");
            return false;
    }
}
