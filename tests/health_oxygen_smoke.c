#include "map.h"
#include "player.h"
#include "task_system.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
    float ruinsSuitLoss;
    float ruinsReserveLoss;
    float deepPoison;
    float deepSuitPoison;
    bool ruinsLeak;

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

    forestLoss = MeasureOxygenLoss(EXTERIOR_X(50), EXTERIOR_Y(90), 5.0f, false, NULL, NULL);
    outerLoss = MeasureOxygenLoss(EXTERIOR_X(106), EXTERIOR_Y(80), 5.0f, false, NULL, NULL);
    deepLoss = MeasureOxygenLoss(EXTERIOR_X(112), EXTERIOR_Y(72), 5.0f, false, &deepPoison, NULL);
    deepSuitLoss = MeasureOxygenLoss(EXTERIOR_X(112), EXTERIOR_Y(72), 5.0f, true, &deepSuitPoison, NULL);
    ruinsLoss = MeasureOxygenLoss(SIGNAL_TOWER_X, SIGNAL_TOWER_Y + 6, 5.0f, false, NULL, &ruinsLeak);
    ruinsSuitLoss = MeasureOxygenLoss(SIGNAL_TOWER_X, SIGNAL_TOWER_Y + 6, 5.0f, true, NULL, NULL);
    ruinsReserveLoss = MeasureOxygenLossWithReserve(SIGNAL_TOWER_X, SIGNAL_TOWER_Y + 6, 5.0f, 40.0f, 2, 45.0f, 20.0f);

    Require(forestLoss > 0.0f && forestLoss < outerLoss,
            "forest should remain the lightest oxygen-pressure zone");
    Require(outerLoss < deepLoss,
            "deep swamp should drain more oxygen than the outer swamp");
    Require(outerLoss < ruinsLoss,
            "ruins should apply heavier oxygen pressure than the outer swamp");
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

    puts("health_oxygen smoke ok");
    return 0;
}
