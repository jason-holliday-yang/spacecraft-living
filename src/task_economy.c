#include "task_runtime_internal.h"

#include "localization.h"
#include "recipe_catalog.h"

#include <stdio.h>
#include <string.h>

static const float kMaxWeaponCalibrationBonus = 8.0f;

typedef struct DisplayTextReplacement {
    const char *from;
    const char *to;
} DisplayTextReplacement;

static void AppendDisplayText(char *dest, size_t destSize, size_t *destLength, const char *text) {
    size_t copyLength;

    if (dest == NULL || destSize == 0 || destLength == NULL || text == NULL) {
        return;
    }

    if (*destLength >= destSize - 1) {
        dest[destSize - 1] = '\0';
        return;
    }

    copyLength = strlen(text);
    if (copyLength > destSize - 1 - *destLength) {
        copyLength = destSize - 1 - *destLength;
    }

    memcpy(dest + *destLength, text, copyLength);
    *destLength += copyLength;
    dest[*destLength] = '\0';
}

void TasksRuntime_SanitizeDisplayText(const char *source, char *dest, size_t destSize) {
    static const DisplayTextReplacement kReplacements[] = {
        {"West W5", "West final pass"},
        {"West W4", "West fourth pass"},
        {"West W3", "West third pass"},
        {"West W2", "West second pass"},
        {"West W1", "West opening pass"},
        {"South S5", "South final pass"},
        {"South S4", "South fourth pass"},
        {"South S3", "South third pass"},
        {"South S2", "South second pass"},
        {"South S1", "South opening pass"},
        {"Cross X3", "Shared ending context"},
        {"Cross X2", "Strategy synthesis"},
        {"Cross X1", "Shared insight"},
        {"X3-informed", "fully informed"},
        {"X3 context", "Shared ending context"},
        {"X2 context", "Strategy context"},
        {"X1 trace alignment", "shared trace alignment"},
        {"X2 strategy rewrite", "strategy synthesis"},
        {"X2 rewrite", "strategy synthesis"},
        {"X3 complete", "Shared ending context complete"},
        {"X2 complete", "Strategy synthesis complete"},
        {"X1 ready", "Shared insight ready"},
        {"X2 ready", "Strategy synthesis ready"},
        {"X3 ready", "Shared ending context ready"},
        {"W5", "final west pass"},
        {"W4", "fourth west pass"},
        {"W3", "third west pass"},
        {"W2", "second west pass"},
        {"W1", "opening west pass"},
        {"S5", "final south pass"},
        {"S4", "fourth south pass"},
        {"S3", "third south pass"},
        {"S2", "second south pass"},
        {"S1", "opening south pass"},
        {"X3", "shared ending context"},
        {"X2", "strategy synthesis"},
        {"X1", "shared insight"},
        {"Pressure", "Strain"},
        {"pressure", "strain"}
    };
    size_t sourceIndex;
    size_t destLength;
    size_t replacementIndex;

    if (dest == NULL || destSize == 0) {
        return;
    }

    if (source == NULL) {
        dest[0] = '\0';
        return;
    }

    dest[0] = '\0';
    sourceIndex = 0;
    destLength = 0;
    while (source[sourceIndex] != '\0' && destLength < destSize - 1) {
        int matched;

        matched = 0;
        for (replacementIndex = 0; replacementIndex < sizeof(kReplacements) / sizeof(kReplacements[0]); replacementIndex++) {
            const DisplayTextReplacement *replacement;
            size_t fromLength;

            replacement = &kReplacements[replacementIndex];
            fromLength = strlen(replacement->from);
            if (strncmp(source + sourceIndex, replacement->from, fromLength) != 0) {
                continue;
            }

            AppendDisplayText(dest, destSize, &destLength, replacement->to);
            sourceIndex += fromLength;
            matched = 1;
            break;
        }

        if (!matched) {
            dest[destLength++] = source[sourceIndex++];
            dest[destLength] = '\0';
        }
    }
}

bool Tasks_IsNearWorkbench(const Player *player) {
    return TasksRuntime_IsNearRect(player, WORKBENCH_X, WORKBENCH_Y, STATION_FOOTPRINT_WIDTH, STATION_FOOTPRINT_HEIGHT);
}

bool TasksRuntime_IsShipInteriorNode(const ResourceNode *node) {
    return node != NULL && node->area == MAP_AREA_BASE;
}

void TasksRuntime_WriteMessage(char *message, size_t messageSize, const char *text) {
    if (message != NULL && messageSize > 0) {
        TasksRuntime_SanitizeDisplayText(Loc_Translate(text), message, messageSize);
    }
}

ResourceNodeInfoState TasksRuntime_GetNodeInfoState(const ResourceNode *node) {
    if (TasksRuntime_IsShipInteriorNode(node)) {
        return node != NULL && node->active
            ? RESOURCE_NODE_INFO_SHIP_SUPPLY_READY
            : RESOURCE_NODE_INFO_SHIP_SUPPLY_EMPTY;
    }

    if (node == NULL || !node->active) {
        return RESOURCE_NODE_INFO_DEPLETED;
    }

    if (node->respawnsRemaining < node->initialRespawnsRemaining) {
        return RESOURCE_NODE_INFO_RESTORED;
    }

    return RESOURCE_NODE_INFO_READY;
}

void TasksRuntime_DescribeNodeStatus(const ResourceNode *node, char *message, size_t messageSize) {
    const char *resourceLabel;

    if (node == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Resource status unavailable.", "当前无法读取资源状态。"));
        return;
    }

    resourceLabel = Player_GetResourceLabel(node->type);
    switch (TasksRuntime_GetNodeInfoState(node)) {
        case RESOURCE_NODE_INFO_READY:
            snprintf(message, messageSize, "%s node: intact and ready to harvest.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_RESTORED:
            snprintf(message, messageSize, "%s node: regrown and ready to harvest again.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_SHIP_SUPPLY_READY:
            snprintf(message, messageSize, "%s ship supply: one salvage pull remains.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_SHIP_SUPPLY_EMPTY:
            if (message != NULL && messageSize > 0) {
                message[0] = '\0';
            }
            break;
        case RESOURCE_NODE_INFO_DEPLETED:
        default:
            if (message != NULL && messageSize > 0) {
                message[0] = '\0';
            }
            break;
    }
}

bool Tasks_IsRecipeVisible(const TaskSystem *tasks, RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    if (tasks == NULL) {
        return false;
    }

    entry = RecipeCatalog_Get(recipe);
    return RecipeCatalog_IsVisible(entry, tasks->amplifierUnlocked, tasks->stage);
}

bool TasksRuntime_IsRecipeCompleted(const TaskSystem *tasks,
                                    const GameMap *map,
                                    const Player *player,
                                    RecipeType recipe) {
    (void)tasks;

    if (player == NULL) {
        return false;
    }

    switch (recipe) {
        case RECIPE_GLOW_STICK:
            return player->hasGlowStick;
        case RECIPE_ROPE:
            return player->hasRope;
        case RECIPE_LASER_GUN:
            return player->hasLaserGun;
        case RECIPE_PROTECTION_SUIT:
            return player->hasProtectionSuit;
        case RECIPE_SIGNAL_AMPLIFIER:
            return player->hasSignalAmplifier;
        case RECIPE_FIELD_CAMP:
            return player->hasFieldCamp || (map != NULL && map->campPlaced);
        case RECIPE_RECOVERY_RATION:
        case RECIPE_REINFORCED_METAL:
        case RECIPE_COUNT:
        default:
            return false;
    }
}

bool Tasks_CanCraftRecipe(const TaskSystem *tasks, const Player *player, RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    if (tasks == NULL || player == NULL || !Tasks_IsRecipeVisible(tasks, recipe)) {
        return false;
    }

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
        return false;
    }

    if (TasksRuntime_IsRecipeCompleted(tasks, NULL, player, recipe)) {
        return false;
    }
    if (recipe == RECIPE_REINFORCED_METAL && player->attackBonus >= kMaxWeaponCalibrationBonus) {
        return false;
    }
    if (entry->requiresWorkbench && !Tasks_IsNearWorkbench(player)) {
        return false;
    }
    if (entry->requiresLowStress && !Player_CanCraftAdvanced(player)) {
        return false;
    }

    return RecipeCatalog_HasResources(player, recipe);
}

bool TasksRuntime_SpendRecipeResources(TaskSystem *tasks,
                                       Player *player,
                                       RecipeType recipe,
                                       const char *advancedRequirementMessage,
                                       char *message,
                                       size_t messageSize) {
    const RecipeCatalogEntry *entry;
    char missingMessage[128];

    entry = RecipeCatalog_Get(recipe);
    if (tasks == NULL || player == NULL || entry == NULL) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Crafting data is unavailable.", "当前无法读取制作数据。"));
        return false;
    }
    if (!Tasks_IsRecipeVisible(tasks, recipe)) {
        TasksRuntime_WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
        return false;
    }
    if (TasksRuntime_IsRecipeCompleted(tasks, NULL, player, recipe)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("That build is already complete and ready in your pack.",
                                                  "这项制作已经完成，并且已经在背包中就绪。"));
        return false;
    }
    if (recipe == RECIPE_REINFORCED_METAL && player->attackBonus >= kMaxWeaponCalibrationBonus) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  Loc_PickLiteral("The weapon frame is already fully serviced with this material set.",
                                                  "这套材料能做的武器框架维护已经完成。"));
        return false;
    }
    if (entry->requiresWorkbench && !Tasks_IsNearWorkbench(player)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  advancedRequirementMessage != NULL ? advancedRequirementMessage : Loc_PickLiteral("Move in beside the workbench first.", "请先靠近工作台。"));
        return false;
    }
    if (entry->requiresLowStress && !Player_CanCraftAdvanced(player)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  advancedRequirementMessage != NULL ? advancedRequirementMessage : Loc_PickLiteral("Steady your health, oxygen, and anomalies first.", "请先稳定生命、氧气与异常状态。"));
        return false;
    }
    if (!RecipeCatalog_SpendResources(player, recipe)) {
        snprintf(missingMessage,
                 sizeof(missingMessage),
                 Loc_PickLiteral("Missing materials for this build: %s.",
                                 "这项制作缺少材料：%s。"),
                 Loc_PickText(entry->ingredientText));
        TasksRuntime_WriteMessage(message, messageSize, missingMessage);
        return false;
    }

    return true;
}

bool TasksRuntime_CollectNode(TaskSystem *tasks, Player *player, ResourceNode *node, char *message, size_t messageSize) {
    float oxygenCost;
    int yieldCount;
    const char *messageFormat;
    const char *resourceLabel;
    bool shipInteriorNode;
    ResourceNodeInfoState nodeInfoState;

    oxygenCost = (node->special ? 4.4f : 2.8f) * Player_GetGatherMultiplier(player);
    shipInteriorNode = TasksRuntime_IsShipInteriorNode(node);
    nodeInfoState = TasksRuntime_GetNodeInfoState(node);
    if (player->health <= 12.0f || Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("You are in no condition to gather safely right now.", "你当前状态太差，无法安全采集。"));
        return false;
    }
    if (player->oxygen <= oxygenCost + 4.0f || Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)) {
        TasksRuntime_WriteMessage(message, messageSize, Loc_PickLiteral("Your oxygen margin is too thin for a safe harvest.", "你的氧气余量太低，无法安全采集。"));
        return false;
    }

    yieldCount = node->baseYield;
    if (tasks->currentEvent == EVENT_HARVEST) {
        yieldCount *= 2;
    }

    Player_DamageOxygen(player, oxygenCost);
    Player_AddResource(player, node->type, yieldCount);
    resourceLabel = Player_GetResourceLabel(node->type);

    if (shipInteriorNode) {
        node->respawnsRemaining = 0;
        node->active = false;
        node->awayTimer = 0.0f;
    } else if (node->respawnsRemaining > 0) {
        node->respawnsRemaining -= 1;
        node->active = false;
        node->awayTimer = 0.0f;
    } else {
        node->active = false;
    }

    if (shipInteriorNode) {
        messageFormat = Loc_PickLiteral("Recovered %s x%d from a one-use ship supply cache.",
                                        "已回收 %s x%d（来自飞船内一次性补给箱）。");
    } else if (nodeInfoState == RESOURCE_NODE_INFO_RESTORED) {
        messageFormat = Loc_PickLiteral("Harvested %s x%d from a regrown node.",
                                        "已采集 %s x%d（来自重新长成的资源节点）。");
    } else {
        messageFormat = Loc_PickLiteral("Harvested %s x%d",
                                        "已采集 %s x%d");
    }
    snprintf(message, messageSize, messageFormat, resourceLabel, yieldCount);
    return true;
}
