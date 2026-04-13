#include "player.h"

#include <stddef.h>
#include <stdio.h>

static void WriteMessage(char *message, int messageSize, const char *text) {
    if (message != NULL && messageSize > 0) {
        snprintf(message, (size_t)messageSize, "%s", text);
    }
}

static float ClampFloatLocal(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void ReducePoison(Player *player, float amount) {
    if (player == NULL || amount <= 0.0f) {
        return;
    }

    player->poison = ClampFloatLocal(player->poison - amount, 0.0f, MAX_POISON);
    if (player->poison <= 0.0f) {
        Player_ClearStatus(player, PLAYER_STATUS_POISONED);
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

static float GetRepeatModifier(Player *player, int foodType) {
    float modifier;

    if (player->lastFoodType == foodType) {
        if (player->repeatedFoodCount < 3) {
            player->repeatedFoodCount += 1;
        }
    } else {
        player->lastFoodType = foodType;
        player->repeatedFoodCount = 1;
    }

    modifier = 1.0f;
    if (player->repeatedFoodCount >= 2) {
        modifier *= 0.70f;
    }
    if (player->repeatedFoodCount >= 3) {
        modifier *= 0.70f;
    }
    return modifier;
}

bool Player_UseQuickConsumable(Player *player, ConsumableFocus focus, char *message, int messageSize) {
    float modifier;

    if (focus == CONSUMABLE_FOOD) {
        if (player->resources[RESOURCE_FRUIT] > 0 && player->resources[RESOURCE_SHELL_FRUIT] > 0) {
            modifier = GetRepeatModifier(player, 100);
            player->resources[RESOURCE_FRUIT] -= 1;
            player->resources[RESOURCE_SHELL_FRUIT] -= 1;
            Player_RecoverHealth(player, 28.0f * modifier);
            Player_RecoverStamina(player, 10.0f * modifier);
            Player_AddOxygen(player, 15.0f);
            ApplyOxygenReserveBuff(player, 2, 48.0f, 20.0f);
            player->speedBoostTimer = 25.0f;
            WriteMessage(message, messageSize, "Field ration combo consumed: health restored, oxygen stabilized, and reserve breathing time gained.");
            return true;
        }

        if (player->resources[RESOURCE_SHELL_FRUIT] > 0) {
            modifier = GetRepeatModifier(player, 101);
            player->resources[RESOURCE_SHELL_FRUIT] -= 1;
            Player_RecoverHealth(player, 18.0f * modifier);
            Player_RecoverStamina(player, 6.0f * modifier);
            Player_AddOxygen(player, 10.0f);
            ApplyOxygenReserveBuff(player, 1, 32.0f, 12.0f);
            player->speedBoostTimer = 18.0f;
            WriteMessage(message, messageSize, "Shell fruit consumed: health recovered and an oxygen reserve kicked in.");
            return true;
        }

        if (player->resources[RESOURCE_FRUIT] > 0) {
            modifier = GetRepeatModifier(player, 102);
            player->resources[RESOURCE_FRUIT] -= 1;
            Player_RecoverHealth(player, 16.0f * modifier);
            Player_RecoverStamina(player, 4.0f * modifier);
            WriteMessage(message, messageSize, "Plant fruit consumed: a quick health recovery in the field.");
            return true;
        }

        WriteMessage(message, messageSize, "No quick healing food available.");
        return false;
    }

    if (player->resources[RESOURCE_SPECIAL_FUNGUS] > 0 && player->resources[RESOURCE_CALM_MUSHROOM] > 0) {
        player->resources[RESOURCE_SPECIAL_FUNGUS] -= 1;
        player->resources[RESOURCE_CALM_MUSHROOM] -= 1;
        Player_RecoverHealth(player, 18.0f);
        Player_AddOxygen(player, 10.0f);
        ReducePoison(player, 60.0f);
        Player_ClearPoison(player);
        DowngradeOxygenLeak(player);
        Player_ClearStatus(player, PLAYER_STATUS_OXYGEN_LEAK);
        ApplyFilteredBuff(player, 2, 70.0f, 0.35f);
        WriteMessage(message, messageSize, "Medicinal mix used: poison purged, suit seals stabilized, and filtered breathing improved.");
        return true;
    }

    if (player->resources[RESOURCE_CALM_MUSHROOM] > 0) {
        player->resources[RESOURCE_CALM_MUSHROOM] -= 1;
        Player_RecoverHealth(player, 10.0f);
        ReducePoison(player, 45.0f);
        if (player->poison <= 10.0f) {
            Player_ClearPoison(player);
        }
        ApplyFilteredBuff(player, 1, 45.0f, 0.22f);
        WriteMessage(message, messageSize, "Calming mushroom used: poison eased and temporary filtered breathing applied.");
        return true;
    }

    if (player->resources[RESOURCE_SPECIAL_FUNGUS] > 0) {
        player->resources[RESOURCE_SPECIAL_FUNGUS] -= 1;
        Player_RecoverHealth(player, 8.0f);
        ReducePoison(player, 18.0f);
        DowngradeOxygenLeak(player);
        WriteMessage(message, messageSize, "Special fungus used: anomalies stabilized and poison buildup slowed.");
        return true;
    }

    if (player->resources[RESOURCE_GLOW_MOSS] > 0) {
        player->resources[RESOURCE_GLOW_MOSS] -= 1;
        Player_AddOxygen(player, 18.0f);
        ApplyFilteredBuff(player, 1, 24.0f, 0.18f);
        ApplyOxygenReserveBuff(player, 1, 22.0f, 10.0f);
        WriteMessage(message, messageSize, "Glow moss used: oxygen stabilized and a brief filter buffer came online.");
        return true;
    }

    WriteMessage(message, messageSize, "No quick antidote or filter item available.");
    return false;
}

bool Player_UseSelectedConsumable(Player *player, ResourceType resource, char *message, int messageSize) {
    float modifier;

    switch (resource) {
        case RESOURCE_FRUIT:
            if (player->resources[RESOURCE_FRUIT] <= 0) {
                WriteMessage(message, messageSize, "No Plant Fruit left to consume.");
                return false;
            }
            modifier = GetRepeatModifier(player, 102);
            player->resources[RESOURCE_FRUIT] -= 1;
            Player_RecoverHealth(player, 16.0f * modifier);
            Player_RecoverStamina(player, 4.0f * modifier);
            WriteMessage(message, messageSize, "Plant fruit consumed: a quick health recovery in the field.");
            return true;
        case RESOURCE_SHELL_FRUIT:
            if (player->resources[RESOURCE_SHELL_FRUIT] <= 0) {
                WriteMessage(message, messageSize, "No Shell Fruit left to consume.");
                return false;
            }
            modifier = GetRepeatModifier(player, 101);
            player->resources[RESOURCE_SHELL_FRUIT] -= 1;
            Player_RecoverHealth(player, 18.0f * modifier);
            Player_RecoverStamina(player, 6.0f * modifier);
            Player_AddOxygen(player, 10.0f);
            ApplyOxygenReserveBuff(player, 1, 32.0f, 12.0f);
            player->speedBoostTimer = 18.0f;
            WriteMessage(message, messageSize, "Shell fruit consumed: health recovered and an oxygen reserve kicked in.");
            return true;
        case RESOURCE_SPECIAL_FUNGUS:
            if (player->resources[RESOURCE_SPECIAL_FUNGUS] <= 0) {
                WriteMessage(message, messageSize, "No Special Fungus left to use.");
                return false;
            }
            player->resources[RESOURCE_SPECIAL_FUNGUS] -= 1;
            Player_RecoverHealth(player, 8.0f);
            ReducePoison(player, 18.0f);
            DowngradeOxygenLeak(player);
            WriteMessage(message, messageSize, "Special fungus used: anomalies stabilized and poison buildup slowed.");
            return true;
        case RESOURCE_CALM_MUSHROOM:
            if (player->resources[RESOURCE_CALM_MUSHROOM] <= 0) {
                WriteMessage(message, messageSize, "No Calming Mushroom left to use.");
                return false;
            }
            player->resources[RESOURCE_CALM_MUSHROOM] -= 1;
            Player_RecoverHealth(player, 10.0f);
            ReducePoison(player, 45.0f);
            if (player->poison <= 10.0f) {
                Player_ClearPoison(player);
            }
            ApplyFilteredBuff(player, 1, 45.0f, 0.22f);
            WriteMessage(message, messageSize, "Calming mushroom used: poison eased and temporary filtered breathing applied.");
            return true;
        case RESOURCE_GLOW_MOSS:
            if (player->resources[RESOURCE_GLOW_MOSS] <= 0) {
                WriteMessage(message, messageSize, "No Glow Moss left to use.");
                return false;
            }
            player->resources[RESOURCE_GLOW_MOSS] -= 1;
            Player_AddOxygen(player, 18.0f);
            ApplyFilteredBuff(player, 1, 24.0f, 0.18f);
            ApplyOxygenReserveBuff(player, 1, 22.0f, 10.0f);
            WriteMessage(message, messageSize, "Glow moss used: oxygen stabilized and a brief filter buffer came online.");
            return true;
        default:
            WriteMessage(message, messageSize, "This item cannot be used directly.");
            return false;
    }
}
