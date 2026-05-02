#include "player.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "status_system_smoke failed: %s\n", message);
    exit(1);
}

int main(void) {
    Player player;
    PlayerStatusType statuses[PLAYER_STATUS_COUNT];
    char tooltip[512];
    char message[256];
    int count;

    Player_Init(&player);

    Player_SetStatus(&player, PLAYER_STATUS_FILTERED, 1, 12.0f, 0.20f);
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 1, 8.0f, 12.0f);
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 3, 18.0f, 42.0f);
    Player_SetStatus(&player, PLAYER_STATUS_CAMP_RECOVERY, 1, 30.0f, 4.0f);
    Player_SetStatus(&player, PLAYER_STATUS_SUFFOCATING, 1, 3.0f, 7.0f);
    Player_SetStatus(&player, PLAYER_STATUS_OXYGEN_RESERVE, 1, 20.0f, 10.0f);

    Require(Player_GetStatusEffect(&player, PLAYER_STATUS_POISONED)->level == 3,
            "reapplying a status should upgrade to the higher level");
    Require(Player_GetStatusEffect(&player, PLAYER_STATUS_POISONED)->remainingTime >= 18.0f,
            "status upgrades should preserve the longest duration");
    Require(Player_GetStatusEffect(&player, PLAYER_STATUS_POISONED)->magnitude >= 42.0f,
            "status upgrades should preserve the strongest magnitude");

    count = Player_CollectActiveStatuses(&player, statuses, PLAYER_STATUS_COUNT);
    Require(count == 5,
            "status collection should return each active status exactly once");
    Require(statuses[0] == PLAYER_STATUS_SUFFOCATING
                && statuses[1] == PLAYER_STATUS_POISONED
                && statuses[2] == PLAYER_STATUS_CAMP_RECOVERY
                && statuses[3] == PLAYER_STATUS_OXYGEN_RESERVE
                && statuses[4] == PLAYER_STATUS_FILTERED,
            "status ordering should keep negatives first, then positive statuses by priority");

    Player_GetStatusTooltip(&player, PLAYER_STATUS_FILTERED, tooltip, (int)sizeof(tooltip));
    Require(strstr(tooltip, "Filtered") != NULL
                && strstr(tooltip, "hazard oxygen drain") != NULL
                && strstr(tooltip, "Duration:") != NULL,
            "filtered tooltip should expose the status name, effect, and duration");

    Player_DowngradeStatus(&player, PLAYER_STATUS_POISONED, 1, 5.0f, 8.0f);
    Require(Player_GetStatusEffect(&player, PLAYER_STATUS_POISONED)->level == 1,
            "status downgrade should update the stored level");

    Player_UpdateStatuses(&player, 12.5f);
    Require(!Player_HasStatus(&player, PLAYER_STATUS_FILTERED)
                && !Player_HasStatus(&player, PLAYER_STATUS_SUFFOCATING),
            "expired statuses should be cleared after their duration elapses");
    Require(Player_HasStatus(&player, PLAYER_STATUS_CAMP_RECOVERY),
            "longer-duration statuses should remain after shorter ones expire");

    Player_ClearStatus(&player, PLAYER_STATUS_POISONED);
    Player_ClearStatus(&player, PLAYER_STATUS_CAMP_RECOVERY);
    Player_ClearStatus(&player, PLAYER_STATUS_OXYGEN_RESERVE);
    count = Player_CollectActiveStatuses(&player, statuses, PLAYER_STATUS_COUNT);
    Require(count == 0,
            "clearing statuses should leave the active-status list empty");

    player.poison = 38.0f;
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 2, 12.0f, 38.0f);
    player.resources[RESOURCE_SPECIAL_FUNGUS] = 1;
    memset(message, 0, sizeof(message));
    Require(!Player_UseSelectedConsumable(&player, RESOURCE_SPECIAL_FUNGUS, message, (int)sizeof(message)),
            "special fungus should no longer be directly usable as a selected consumable");
    Require(player.poison == 38.0f,
            "special fungus should stay raw material until processed at the workbench");
    Require(Player_HasStatus(&player, PLAYER_STATUS_POISONED),
            "raw fungus should not clear poison directly");

    player.poison = 52.0f;
    Player_SetStatus(&player, PLAYER_STATUS_POISONED, 3, 12.0f, 52.0f);
    player.resources[RESOURCE_CALM_MUSHROOM] = 1;
    memset(message, 0, sizeof(message));
    Require(!Player_UseSelectedConsumable(&player, RESOURCE_CALM_MUSHROOM, message, (int)sizeof(message)),
            "calming mushroom should no longer be directly usable as a selected consumable");
    Require(player.poison == 52.0f,
            "calming mushroom should stay raw material until processed at the workbench");
    Require(Player_HasStatus(&player, PLAYER_STATUS_POISONED)
                && Player_GetStatusEffect(&player, PLAYER_STATUS_POISONED)->level == 3,
            "poison status should remain until a prepared Recovery Ration is used");
    Require(!Player_HasStatus(&player, PLAYER_STATUS_FILTERED),
            "raw calming mushroom should not provide a filtered breathing buffer");

    puts("status_system smoke ok");
    return 0;
}
