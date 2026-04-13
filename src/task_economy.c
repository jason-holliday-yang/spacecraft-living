#include "task_runtime_internal.h"

#include "localization.h"
#include "recipe_catalog.h"

#include <stdio.h>
#include <string.h>

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
        TasksRuntime_SanitizeDisplayText(text, message, messageSize);
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
        TasksRuntime_WriteMessage(message, messageSize, "Resource status unavailable.");
        return;
    }

    resourceLabel = Player_GetResourceLabel(node->type);
    switch (TasksRuntime_GetNodeInfoState(node)) {
        case RESOURCE_NODE_INFO_READY:
            snprintf(message, messageSize, "%s node: ready to collect.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_RESTORED:
            snprintf(message, messageSize, "%s node: restored and ready to collect.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_SHIP_SUPPLY_READY:
            snprintf(message, messageSize, "%s ship supply: ready for one collection.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_SHIP_SUPPLY_EMPTY:
            snprintf(message, messageSize, "%s ship supply: exhausted. Interior supply caches do not replenish.", resourceLabel);
            break;
        case RESOURCE_NODE_INFO_DEPLETED:
        default:
            snprintf(message, messageSize, "%s node: depleted for now. Leave this area and return later.", resourceLabel);
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

bool Tasks_CanCraftRecipe(const TaskSystem *tasks, const Player *player, RecipeType recipe) {
    const RecipeCatalogEntry *entry;

    if (tasks == NULL || player == NULL || !Tasks_IsRecipeVisible(tasks, recipe)) {
        return false;
    }

    entry = RecipeCatalog_Get(recipe);
    if (entry == NULL) {
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
        TasksRuntime_WriteMessage(message, messageSize, "Crafting data unavailable.");
        return false;
    }
    if (!Tasks_IsRecipeVisible(tasks, recipe)) {
        TasksRuntime_WriteMessage(message, messageSize, LOC_UI_RECIPE_LOCKED);
        return false;
    }
    if (entry->requiresWorkbench && !Tasks_IsNearWorkbench(player)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  advancedRequirementMessage != NULL ? advancedRequirementMessage : "Move next to the workbench.");
        return false;
    }
    if (entry->requiresLowStress && !Player_CanCraftAdvanced(player)) {
        TasksRuntime_WriteMessage(message,
                                  messageSize,
                                  advancedRequirementMessage != NULL ? advancedRequirementMessage : "Stabilize health, oxygen, and anomalies first.");
        return false;
    }
    if (!RecipeCatalog_SpendResources(player, recipe)) {
        snprintf(missingMessage, sizeof(missingMessage), "Missing materials: %s.", entry->ingredientText);
        TasksRuntime_WriteMessage(message, messageSize, missingMessage);
        return false;
    }

    return true;
}

bool TasksRuntime_CollectNode(TaskSystem *tasks, Player *player, ResourceNode *node, char *message, size_t messageSize) {
    float oxygenCost;
    int yieldCount;
    const char *resourceLabel;
    bool shipInteriorNode;
    ResourceNodeInfoState nodeInfoState;

    oxygenCost = (node->special ? 4.4f : 2.8f) * Player_GetGatherMultiplier(player);
    shipInteriorNode = TasksRuntime_IsShipInteriorNode(node);
    nodeInfoState = TasksRuntime_GetNodeInfoState(node);
    if (player->health <= 12.0f || Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)) {
        TasksRuntime_WriteMessage(message, messageSize, "You are too injured to gather safely right now.");
        return false;
    }
    if (player->oxygen <= oxygenCost + 4.0f || Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)) {
        TasksRuntime_WriteMessage(message, messageSize, "Your oxygen margin is too thin to gather safely right now.");
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
        snprintf(message, messageSize, "Collected %s x%d from a one-time ship supply.", resourceLabel, yieldCount);
    } else if (nodeInfoState == RESOURCE_NODE_INFO_RESTORED) {
        snprintf(message, messageSize, "Collected %s x%d from a restored node.", resourceLabel, yieldCount);
    } else {
        snprintf(message, messageSize, "Collected %s x%d", resourceLabel, yieldCount);
    }
    return true;
}
