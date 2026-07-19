#include "task_crafting_internal.h"
#include "task_runtime_internal.h"
#include "localization.h"

static constexpr float kWeaponCalibrationBonus = 2.0f;
static constexpr float kMaxWeaponCalibrationBonus = 8.0f;

bool Tasks_TryCraft(TaskSystem *tasks, GameMap *map, Player *player, RecipeType recipe, char *message, size_t messageSize) {
    if (tasks == NULL || map == NULL || player == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("The crafting setup is unavailable right now.", "当前无法完成制作。"));
        return false;
    }
    if (TasksRuntime_IsRecipeCompleted(tasks, map, player, recipe)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("That build is already complete and ready in your pack.",
                                                  "这项制作已经完成，并且已经在背包中就绪。"));
        return false;
    }

    switch (recipe) {
        case RECIPE_GLOW_STICK:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasGlowStick = true;
            player->glowStickTimer = 60.0f;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Glow Stick assembled. The next stretch of darkness will not close in as easily.", "荧光棒已组装完成。接下来那段黑暗路程，不会那么容易把你吞没。"));
            return true;
        case RECIPE_ROPE:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasRope = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Simple Rope finished. Face a marked swamp barrier and use F to lash it into a shortcut.", "简易绳索已完成。面对可处理的沼泽阻隔时按 F，就能把它绑成一条捷径。"));
            return true;
        case RECIPE_REINFORCED_METAL:
            if (player->attackBonus >= kMaxWeaponCalibrationBonus) {
                TasksRuntime_WriteMessage(message,
                                          messageSize,
                                          Loc_PickLiteral("The weapon frame is already fully serviced with this material set.",
                                                          "这套材料能做的武器框架维护已经完成。"));
                return false;
            }
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("Weapon-frame service needs the workbench and a body steady enough to focus.", "武器框架维护需要依托工作台，而且你的身体状态必须足够稳定。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->attackBonus += kWeaponCalibrationBonus;
            if (player->attackBonus > kMaxWeaponCalibrationBonus) {
                player->attackBonus = kMaxWeaponCalibrationBonus;
            }
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Weapon frame serviced. The ranged assembly is now better aligned and field-ready.", "武器框架维护完成。远程组件的对位已经重新稳定，能够继续投入外勤。"));
            return true;
        case RECIPE_LASER_GUN:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Laser Gun has to be assembled at the workbench while your condition stays steady.", "激光枪必须在工作台旁、并在身体状态稳定时完成装配。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasLaserGun = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Laser Gun assembled. You now have something that can answer from a distance before the planet reaches you first.", "激光枪已装配完成。现在，你终于有了能在危险逼近前先从远处作出回应的武器。"));
            return true;
        case RECIPE_PROTECTION_SUIT:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Protection Suit has to be assembled at the workbench while your vitals remain steady.", "防护服必须在工作台旁、且生命体征平稳时完成组装。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasProtectionSuit = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Protection Suit crafted. Filtered breathing is online, and the swamp air should feel a little less eager to kill you.", "防护服已制作完成。过滤呼吸系统已经上线，沼地与遗迹里的危险空气终于没那么容易要你的命了。"));
            return true;
        case RECIPE_SIGNAL_AMPLIFIER:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Signal Amplifier has to be built at the workbench while your condition stays stable.", "信号放大器必须在工作台旁、且状态稳定时制作。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            player->hasSignalAmplifier = true;
            tasks->signalAmplifierCrafted = true;
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Signal Amplifier completed. The tower now has a calmer answer available, though the final climb still demands clean oxygen preparation.", "信号放大器已完成。塔楼如今多了一种更平稳的解法，但最后的攀登依旧需要充足而稳定的氧气准备。"));
            return true;
        case RECIPE_FIELD_CAMP:
            if (!TasksRuntime_SpendRecipeResources(tasks, player, recipe, NULL, message, messageSize)) {
                return false;
            }
            player->hasFieldCamp = true;
            Map_SetFieldCamp(map, player->gridX, player->gridY);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Field Camp built. It gives partial health and oxygen recovery, plus a short Camp Recovery boost for the next push.", "野外营地已建立。它能提供部分生命与氧气恢复，并为下一次推进附带短暂的营地恢复增益。"));
            return true;
        case RECIPE_RECOVERY_RATION:
            if (!TasksRuntime_SpendRecipeResources(tasks,
                                                  player,
                                                  recipe,
                                                  Loc_PickLiteral("The Recovery Ration has to be sealed at the workbench before the field mix stays stable.", "复苏口粮必须在工作台旁完成密封，这份野外混合物才能稳定下来。"),
                                                  message,
                                                  messageSize)) {
                return false;
            }
            Player_AddResource(player, RESOURCE_RECOVERY_RATION, 1);
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Recovery Ration sealed. Press X during a field push to restore health, clear poison, and recover oxygen in one use.", "复苏口粮已密封。外勤推进时按 X，就能一次完成回血、解毒和补氧。"));
            return true;
        case RECIPE_COUNT:
        default:
            TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("That recipe is unreadable.", "该配方目前无法识别。"));
            return false;
    }
}
