#include "map.h"
#include "player.h"
#include "task_system.h"
#include "../src/task_runtime_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "camp_recovery_smoke failed: %s\n", message);
    exit(1);
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    char message[256];
    float healthAfterCamp;
    float oxygenAfterCamp;
    float poisonAfterCamp;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);

    tasks.stage = 5;
    map.campPlaced = true;
    map.campX = ROPE_BARRIER_B_X - 1;
    map.campY = ROPE_BARRIER_B_Y;

    player.gridX = map.campX;
    player.gridY = map.campY;
    player.health = 40.0f;
    player.oxygen = 20.0f;
    player.stamina = 18.0f;
    player.pressure = 75.0f;
    player.poison = 34.0f;
    Player_SetStatus(&player, PLAYER_STATUS_OXYGEN_LEAK, 2, 24.0f, 1.1f);

    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "field camp interaction should succeed when resting beside the camp");
    Require(strstr(message, "Camp Recovery") != NULL,
            "field camp interaction should mention the recovery status it grants");
    Require(player.health > 40.0f && player.health < Player_GetMaxHealth(&player),
            "field camp should restore only part of health");
    Require(player.oxygen > 20.0f && player.oxygen < MAX_OXYGEN,
            "field camp should restore only part of oxygen");
    Require(player.poison < 34.0f,
            "field camp should partially reduce poison");
    Require(Player_HasStatus(&player, PLAYER_STATUS_CAMP_RECOVERY),
            "field camp should grant camp recovery status");

    healthAfterCamp = player.health;
    oxygenAfterCamp = player.oxygen;
    poisonAfterCamp = player.poison;

    player.gridX = EXTERIOR_X(50);
    player.gridY = EXTERIOR_Y(90);
    Tasks_Update(&tasks, &map, &player, 5.0f);
    Require(player.health > healthAfterCamp,
            "camp recovery should continue restoring some health after leaving camp");
    Require(player.oxygen > oxygenAfterCamp,
            "camp recovery should continue restoring some oxygen after leaving camp");
    Require(player.poison <= poisonAfterCamp,
            "camp recovery should continue stabilizing poison after leaving camp");

    player.gridX = OXYGEN_CONSOLE_X - 1;
    player.gridY = OXYGEN_CONSOLE_Y;
    player.health = 22.0f;
    player.oxygen = 6.0f;
    player.stamina = 12.0f;
    player.pressure = 66.0f;
    player.poison = 48.0f;
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 2, 20.0f, 48.0f);
    Player_SetStatus(&player, PLAYER_STATUS_LOW_OXYGEN, 2, 8.0f, 6.0f);
    Player_SetStatus(&player, PLAYER_STATUS_CRITICAL_CONDITION, 1, 8.0f, 22.0f);
    memset(message, 0, sizeof(message));
    Require(Tasks_HandleInteraction(&tasks, &map, &player, message, sizeof(message)),
            "base oxygen console should remain usable");
    Require(player.health == Player_GetMaxHealth(&player)
                && player.oxygen == MAX_OXYGEN
                && player.stamina == Player_GetCurrentStaminaCap(&player),
            "base oxygen console should still perform a full reset");
    Require(player.poison == 0.0f && player.pressure == 0.0f,
            "base oxygen console should clear poison and hidden pressure");
    Require(!Player_HasStatus(&player, PLAYER_STATUS_POISONED)
                && !Player_HasStatus(&player, PLAYER_STATUS_LOW_OXYGEN)
                && !Player_HasStatus(&player, PLAYER_STATUS_CRITICAL_CONDITION),
            "base oxygen console should clear negative statuses completely");

    puts("camp_recovery smoke ok");
    return 0;
}
