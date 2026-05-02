#include "map.h"
#include "player.h"
#include "task_system.h"
#include "../src/task_runtime_internal.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "health_oxygen_smoke failed: %s\n", message);
    exit(1);
}

static float MeasureOxygenLoss(int gridX, int gridY, float deltaTime, bool protectionSuit, float *poisonOut, bool *oxygenLeakOut) {
    GameMap map;
    Player player;
    TaskSystem tasks;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);
    tasks.stage = 7;
    tasks.oxygenRepairLevel = 2;
    player.gridX = gridX;
    player.gridY = gridY;
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    player.hasProtectionSuit = protectionSuit;
    Player_ClearAllStatuses(&player);

    Tasks_Update(&tasks, &map, &player, deltaTime);
    if (poisonOut != NULL) {
        *poisonOut = player.poison;
    }
    if (oxygenLeakOut != NULL) {
        *oxygenLeakOut = Player_HasStatus(&player, PLAYER_STATUS_OXYGEN_LEAK);
    }
    return INITIAL_OXYGEN - player.oxygen;
}

static float MeasureOxygenLossWithReserve(int gridX, int gridY, float deltaTime, float oxygenStart, int reserveLevel, float reserveDuration, float reserveMagnitude) {
    GameMap map;
    Player player;
    TaskSystem tasks;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);
    tasks.stage = 7;
    tasks.oxygenRepairLevel = 2;
    player.gridX = gridX;
    player.gridY = gridY;
    player.health = INITIAL_HEALTH;
    player.oxygen = oxygenStart;
    player.poison = 0.0f;
    Player_ClearAllStatuses(&player);
    Player_SetStatus(&player, PLAYER_STATUS_OXYGEN_RESERVE, reserveLevel, reserveDuration, reserveMagnitude);

    Tasks_Update(&tasks, &map, &player, deltaTime);
    return oxygenStart - player.oxygen;
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    float forestLoss;
    float outerLoss;
    float deepLoss;
    float deepSuitLoss;
    float ruinsLoss;
    float ringUpperLoss;
    float ruinsSuitLoss;
    float ruinsReserveLoss;
    float deepPoison;
    float deepSuitPoison;
    bool ruinsLeak;
    float baseHealth;
    float baseOxygen;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);

    player.oxygen = 29.0f;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_LOW_OXYGEN)
                && Player_GetStatusEffect(&player, PLAYER_STATUS_LOW_OXYGEN)->level == 1,
            "oxygen below the warning threshold should activate low oxygen level 1");

    player.oxygen = 0.0f;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_SUFFOCATING),
            "zero oxygen should activate suffocating");

    player.health = 14.0f;
    player.oxygen = INITIAL_OXYGEN;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(Player_HasStatus(&player, PLAYER_STATUS_CRITICAL_CONDITION)
                && Player_GetStatusEffect(&player, PLAYER_STATUS_CRITICAL_CONDITION)->level == 2,
            "very low health should activate critical condition level 2");

    forestLoss = MeasureOxygenLoss(EXTERIOR_X(90), EXTERIOR_Y(80), 5.0f, false, NULL, NULL);
    outerLoss = MeasureOxygenLoss(EXTERIOR_X(102), EXTERIOR_Y(56), 5.0f, false, NULL, NULL);
    deepLoss = MeasureOxygenLoss(EXTERIOR_X(116), EXTERIOR_Y(50), 5.0f, false, &deepPoison, NULL);
    deepSuitLoss = MeasureOxygenLoss(EXTERIOR_X(116), EXTERIOR_Y(50), 5.0f, true, &deepSuitPoison, NULL);
    ringUpperLoss = MeasureOxygenLoss(EXTERIOR_X(48), EXTERIOR_Y(12), 5.0f, false, NULL, NULL);
    ruinsLoss = MeasureOxygenLoss(SIGNAL_TOWER_X, SIGNAL_TOWER_Y + 6, 5.0f, false, NULL, &ruinsLeak);
    ruinsSuitLoss = MeasureOxygenLoss(SIGNAL_TOWER_X, SIGNAL_TOWER_Y + 6, 5.0f, true, NULL, NULL);
    ruinsReserveLoss = MeasureOxygenLossWithReserve(SIGNAL_TOWER_X, SIGNAL_TOWER_Y + 6, 5.0f, 40.0f, 2, 45.0f, 20.0f);

    player.gridX = SHIP_CORRIDOR_X + 1;
    player.gridY = SHIP_CORRIDOR_Y + 1;
    tasks.stage = 7;
    tasks.oxygenRepairLevel = 2;
    player.health = 28.0f;
    player.oxygen = 12.0f;
    player.poison = 40.0f;
    player.safeRecoveryTimer = 0.0f;
    Player_ClearAllStatuses(&player);
    Player_SetStatus(&player, PLAYER_STATUS_OXYGEN_LEAK, 2, 18.0f, 1.3f);
    baseHealth = player.health;
    baseOxygen = player.oxygen;
    Tasks_Update(&tasks, &map, &player, 5.0f);
    player.gridX = EXTERIOR_X(48);
    player.gridY = EXTERIOR_Y(12);
    Require(strcmp(Map_GetLocationNameAt(player.gridX, player.gridY), "Monolith Ring") == 0,
            "upper ruins tile used by the smoke test should still belong to Monolith Ring");
    Require(TasksRuntime_IsMonolithRing(&player),
            "Monolith Ring classification should match the real named location even near the tower-height band");
    Require(!TasksRuntime_IsTowerPlateau(&player),
            "Monolith Ring tiles should no longer be misclassified as Signal Tower Plateau just because they share a Y band");

    player.gridX = EXTERIOR_X(48);
    player.gridY = EXTERIOR_Y(24);
    Require(strcmp(Map_GetLocationNameAt(player.gridX, player.gridY), "Monolith Ring") == 0,
            "lower ruins tile used by the smoke test should still belong to Monolith Ring");
    Require(TasksRuntime_IsMonolithRing(&player),
            "Monolith Ring classification should cover the full named location instead of only a narrow vertical strip");

    Require(forestLoss > 0.0f && forestLoss < outerLoss,
            "forest should remain the lightest oxygen-pressure zone");
    Require(outerLoss < deepLoss,
            "deep swamp should drain more oxygen than the outer swamp");
    Require(outerLoss < ruinsLoss,
            "ruins should apply heavier oxygen pressure than the outer swamp");
    Require(ringUpperLoss < ruinsLoss,
            "tower plateau oxygen pressure should remain stricter than nearby Monolith Ring tiles");
    Require(deepPoison > 0.0f,
            "deep swamp should accumulate poison as a core area identity");
    Require(ruinsLeak,
            "ruins should be able to trigger oxygen leak as a distinct threat");
    Require(deepSuitLoss < deepLoss,
            "protection suit should reduce deep-swamp oxygen loss");
    Require(deepSuitPoison < deepPoison,
            "protection suit should reduce deep-swamp poison accumulation");
    Require(ruinsSuitLoss < ruinsLoss,
            "protection suit should also reduce ruins oxygen pressure");
    Require(ruinsReserveLoss < ruinsLoss,
            "oxygen reserve should create meaningful breathing room in the ruins");
    Require(player.health < baseHealth,
            "poison should still be able to damage health inside the ship");
    Require(fabsf(player.oxygen - baseOxygen) < 0.001f,
            "fully repaired ship interiors should stop further oxygen loss without auto-refilling oxygen");
    Require(player.poison >= 40.0f,
            "standing inside the ship should no longer grant passive poison recovery without using a facility");

    player.gridX = SHIP_CORRIDOR_X + 1;
    player.gridY = SHIP_CORRIDOR_Y + 1;
    player.health = 28.0f;
    player.oxygen = 0.0f;
    player.poison = 0.0f;
    Player_ClearAllStatuses(&player);
    baseHealth = player.health;
    Tasks_Update(&tasks, &map, &player, 2.0f);
    Require(fabsf(player.health - baseHealth) < 0.001f,
            "fully repaired ship interiors should stop suffocation damage until the player leaves");
    Require(!Player_HasStatus(&player, PLAYER_STATUS_SUFFOCATING),
            "stable ship oxygen support should clear the suffocating alert while inside the base");

    puts("health_oxygen smoke ok");
    return 0;
}
