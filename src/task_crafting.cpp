#include "task_runtime_internal.h"

bool Tasks_TryCraft(TaskSystem *tasks, GameMap *map, Player *player, RecipeType recipe, char *message, size_t messageSize) {
    if (tasks == NULL || map == NULL || player == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Crafting unavailable.", "当前无法制作。"));
        return false;
    }

    switch (recipe) {
        case RECIPE_GLOW_STICK:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasGlowStick = true;
            player->glowStickTimer = 60.0f;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Crafted Glow Stick. Night visibility increased for a while.", "已制作荧光棒。夜间视野会在一段时间内提升。"));
            return true;
        case RECIPE_ROPE:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasRope = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Crafted Simple Rope. Face a swamp barrier and use F to turn it into a shortcut.", "已制作简易绳索。面对沼泽阻隔时按 F，可把它变成捷径。"));
            return true;
        case RECIPE_REINFORCED_METAL:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("Advanced crafting requires the workbench and a stable enough condition to focus.", "高级制作需要工作台，并且你的状态要足够稳定。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->resources[RESOURCE_METAL_SCRAP] += 1;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Crafted Reinforced Metal for the Laser Gun.", "已制作强化金属，可用于组装激光枪。"));
            return true;
        case RECIPE_LASER_GUN:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Laser Gun requires the workbench and a stable enough condition to assemble it.", "激光枪需要在工作台旁并在状态稳定时组装。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasLaserGun = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Laser Gun assembled. Ranged combat is now available.", "激光枪已组装完成。现在可以进行远程战斗。"));
            return true;
        case RECIPE_PROTECTION_SUIT:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Protection Suit requires the workbench and a stable enough condition to assemble it.", "防护服需要在工作台旁并在状态稳定时组装。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasProtectionSuit = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Protection Suit crafted. Filtered breathing is online, reducing hazard oxygen drain plus poison and leak risk for deep-swamp and ruins pushes.", "防护服已制作完成。过滤呼吸系统已启用，可降低危险区域的氧气消耗，并减轻中毒与漏气风险。"));
            return true;
        case RECIPE_SIGNAL_AMPLIFIER:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Signal Amplifier must be crafted at the workbench while your condition is stable.", "信号放大器必须在工作台旁、且状态稳定时制作。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasSignalAmplifier = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Signal Amplifier completed. You can now bypass the final boss, but the last tower approach still rewards stable oxygen prep.", "信号放大器已完成。现在可以绕开最终首领，但最后登塔前仍然需要充足的氧气准备。"));
            return true;
        case RECIPE_FIELD_CAMP:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasFieldCamp = true;
            Map_SetFieldCamp(map, player->gridX, player->gridY);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Field Camp built. Rest beside it for partial health and oxygen recovery plus Camp Recovery as a safe outdoor fallback before another push.", "已建立野外营地。可以在旁边休息，恢复部分生命与氧气，并在下一次推进前获得安全的野外回撤点。"));
            return true;
        case RECIPE_COUNT:
        default:
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Unknown recipe.", "未知配方。"));
            return false;
    }
}
