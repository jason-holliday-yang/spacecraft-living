#include "player.h"
#include "localization.h"

#include <stddef.h>
#include <stdio.h>

static void WriteMessage(char *message, int messageSize, const char *text) {
    if (message != NULL && messageSize > 0) {
        snprintf(message, (size_t)messageSize, "%s", text);
    }
}

static void DowngradeOxygenLeak(Player *player) {
    const PlayerStatusEffect *effect;

    if (player == NULL || !Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        return;
    }

    effect = Player_GetStatusEffect(player, PLAYER_STATUS_OXYGEN_LEAK);
    if (effect == NULL || effect->level <= 1) {
        Player_ClearStatus(player, PLAYER_STATUS_OXYGEN_LEAK);
        return;
    }

    Player_DowngradeStatus(player,
                           PLAYER_STATUS_OXYGEN_LEAK,
                           effect->level - 1,
                           effect->remainingTime,
                           effect->magnitude * 0.70f);
}

static void ApplyFilteredBuff(Player *player, int level, float duration, float magnitude) {
    if (player == NULL) {
        return;
    }

    Player_SetStatus(player, PLAYER_STATUS_FILTERED, level, duration, magnitude);
}

static void ApplyOxygenReserveBuff(Player *player, int level, float duration, float magnitude) {
    if (player == NULL) {
        return;
    }

    Player_SetStatus(player, PLAYER_STATUS_OXYGEN_RESERVE, level, duration, magnitude);
}

static void UseRecoveryRation(Player *player) {
    Player_RecoverHealth(player, 26.0f);
    Player_AddOxygen(player, 34.0f);
    Player_ClearStatus(player, PLAYER_STATUS_SUFFOCATING);
    if (player->oxygen > 32.0f) {
        Player_ClearStatus(player, PLAYER_STATUS_LOW_OXYGEN);
    }
    Player_ClearPoison(player);
    DowngradeOxygenLeak(player);
    Player_ClearStatus(player, PLAYER_STATUS_OXYGEN_LEAK);
    ApplyFilteredBuff(player, 2, 62.0f, 0.28f);
    ApplyOxygenReserveBuff(player, 2, 60.0f, 22.0f);
    player->speedBoostTimer = 14.0f;
}

bool Player_UseQuickConsumable(Player *player, char *message, int messageSize) {
    if (player->resources[RESOURCE_RECOVERY_RATION] > 0) {
        player->resources[RESOURCE_RECOVERY_RATION] -= 1;
        UseRecoveryRation(player);
        WriteMessage(message,
                     messageSize,
                     Loc_PickLiteral("You break the Recovery Ration seal. The processed mix clears poison, steadies breathing, and pushes both health and oxygen back up at once.",
                                     "你撕开了复苏口粮的密封层。处理好的混合物会同时解毒、稳住呼吸，并把生命和氧气一起抬上来。"));
        return true;
    }

    WriteMessage(message,
                 messageSize,
                 Loc_PickLiteral("No Recovery Ration is ready. Raw forage has to be processed at the workbench first.",
                                 "目前没有现成的复苏口粮。野外采来的食材必须先到工作台加工。"));
    return false;
}

bool Player_UseSelectedConsumable(Player *player, ResourceType resource, char *message, int messageSize) {
    switch (resource) {
        case RESOURCE_RECOVERY_RATION:
            if (player->resources[RESOURCE_RECOVERY_RATION] <= 0) {
                WriteMessage(message, messageSize, Loc_PickLiteral("No Recovery Ration remains.", "已经没有复苏口粮了。"));
                return false;
            }
            player->resources[RESOURCE_RECOVERY_RATION] -= 1;
            UseRecoveryRation(player);
            WriteMessage(message,
                         messageSize,
                         Loc_PickLiteral("You use the prepared Recovery Ration. Health returns, poison clears, and the processed air mix restores oxygen in the same motion.",
                                         "你使用了备好的复苏口粮。生命回升、中毒清空，连处理过的呼吸混合物也在同一口里把氧气补了回来。"));
            return true;
        default:
            WriteMessage(message,
                         messageSize,
                         Loc_PickLiteral("Raw materials cannot be used directly anymore. Process them into a Recovery Ration at the workbench first.",
                                         "原始素材现在不能直接使用。请先到工作台把它们加工成复苏口粮。"));
            return false;
    }
}
