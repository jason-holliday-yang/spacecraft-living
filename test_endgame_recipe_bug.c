#include "map.h"
#include "player.h"
#include "task_system.h"
#include "recipe_catalog.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);

    // 模拟终局状态
    tasks.stage = 7;
    tasks.amplifierUnlocked = true;
    tasks.ending = ENDING_HEROIC; // 这里设定为已进入结局状态！这是问题点！

    printf("Testing with tasks.ending = ENDING_HEROIC\n");
    printf("tasks.amplifierUnlocked = %s\n", tasks.amplifierUnlocked ? "true" : "false");
    
    bool isVisible = Tasks_IsRecipeVisible(&tasks, RECIPE_SIGNAL_AMPLIFIER);
    printf("Tasks_IsRecipeVisible returned: %s\n", isVisible ? "true" : "false");
    
    const RecipeCatalogEntry* entry = RecipeCatalog_Get(RECIPE_SIGNAL_AMPLIFIER);
    printf("Recipe entry != NULL: %s\n", (entry != NULL) ? "true" : "false");
    
    if (entry != NULL) {
        printf("  entry->requiresAmplifierUnlock: %s\n", entry->requiresAmplifierUnlock ? "true" : "false");
        printf("  entry->unlockStage: %d\n", entry->unlockStage);
        printf("  RecipeCatalog_IsVisible: %s\n", 
            RecipeCatalog_IsVisible(entry, tasks.amplifierUnlocked, tasks.stage) ? "true" : "false");
    }
    
    printf("\nNow let's set tasks.ending = ENDING_NONE\n");
    tasks.ending = ENDING_NONE;
    isVisible = Tasks_IsRecipeVisible(&tasks, RECIPE_SIGNAL_AMPLIFIER);
    printf("Tasks_IsRecipeVisible now: %s\n", isVisible ? "true" : "false");
    
    return 0;
}
