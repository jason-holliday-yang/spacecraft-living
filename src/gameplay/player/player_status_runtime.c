#include "player.h"

#include <stddef.h>

static bool IsValidStatusType(PlayerStatusType status) {
    return status >= PLAYER_STATUS_POISONED && status < PLAYER_STATUS_COUNT;
}

static int CompareStatusForDisplay(const Player *player, PlayerStatusType left, PlayerStatusType right) {
    const PlayerStatusEffect *leftEffect;
    const PlayerStatusEffect *rightEffect;
    int leftNegative;
    int rightNegative;
    int leftPriority;
    int rightPriority;

    leftEffect = Player_GetStatusEffect(player, left);
    rightEffect = Player_GetStatusEffect(player, right);
    leftNegative = Player_IsStatusNegative(left) ? 1 : 0;
    rightNegative = Player_IsStatusNegative(right) ? 1 : 0;
    if (leftNegative != rightNegative) {
        return rightNegative - leftNegative;
    }

    leftPriority = Player_GetStatusPriority(left);
    rightPriority = Player_GetStatusPriority(right);
    if (leftPriority != rightPriority) {
        return rightPriority - leftPriority;
    }

    if (leftEffect != NULL && rightEffect != NULL && leftEffect->level != rightEffect->level) {
        return rightEffect->level - leftEffect->level;
    }

    return (int)left - (int)right;
}

bool Player_HasStatus(const Player *player, PlayerStatusType status) {
    if (player == NULL || !IsValidStatusType(status)) {
        return false;
    }

    return player->statusEffects[status].active;
}

const PlayerStatusEffect *Player_GetStatusEffect(const Player *player, PlayerStatusType status) {
    if (player == NULL || !IsValidStatusType(status)) {
        return NULL;
    }

    return &player->statusEffects[status];
}

void Player_SetStatus(Player *player, PlayerStatusType status, int level, float remainingTime, float magnitude) {
    PlayerStatusEffect *effect;
    int normalizedLevel;
    float normalizedTime;
    float normalizedMagnitude;

    if (player == NULL || !IsValidStatusType(status)) {
        return;
    }

    effect = &player->statusEffects[status];
    normalizedLevel = level < 1 ? 1 : level;
    normalizedTime = remainingTime < 0.0f ? 0.0f : remainingTime;
    normalizedMagnitude = magnitude < 0.0f ? 0.0f : magnitude;
    effect->active = true;
    if (effect->level < normalizedLevel) {
        effect->level = normalizedLevel;
    }
    if (effect->remainingTime < normalizedTime) {
        effect->remainingTime = normalizedTime;
    }
    if (effect->magnitude < normalizedMagnitude) {
        effect->magnitude = normalizedMagnitude;
    }
}

void Player_DowngradeStatus(Player *player, PlayerStatusType status, int level, float remainingTime, float magnitude) {
    PlayerStatusEffect *effect;

    if (player == NULL || !IsValidStatusType(status) || !Player_HasStatus(player, status)) {
        return;
    }

    effect = &player->statusEffects[status];
    if (level <= 0 && remainingTime <= 0.0f && magnitude <= 0.0f) {
        Player_ClearStatus(player, status);
        return;
    }

    effect->level = level < 1 ? 1 : level;
    effect->remainingTime = remainingTime < 0.0f ? 0.0f : remainingTime;
    effect->magnitude = magnitude < 0.0f ? 0.0f : magnitude;
}

void Player_ClearStatus(Player *player, PlayerStatusType status) {
    PlayerStatusEffect *effect;

    if (player == NULL || !IsValidStatusType(status)) {
        return;
    }

    effect = &player->statusEffects[status];
    effect->active = false;
    effect->level = 0;
    effect->remainingTime = 0.0f;
    effect->magnitude = 0.0f;
}

void Player_ClearAllStatuses(Player *player) {
    int index;

    if (player == NULL) {
        return;
    }

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        Player_ClearStatus(player, (PlayerStatusType)index);
    }
}

void Player_UpdateStatuses(Player *player, float deltaTime) {
    int index;

    if (player == NULL || deltaTime <= 0.0f) {
        return;
    }

    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        PlayerStatusEffect *effect;

        effect = &player->statusEffects[index];
        if (!effect->active || effect->remainingTime <= 0.0f) {
            continue;
        }

        effect->remainingTime -= deltaTime;
        if (effect->remainingTime <= 0.0f) {
            Player_ClearStatus(player, (PlayerStatusType)index);
        }
    }
}

bool Player_IsStatusNegative(PlayerStatusType status) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
        case PLAYER_STATUS_OXYGEN_LEAK:
        case PLAYER_STATUS_LOW_OXYGEN:
        case PLAYER_STATUS_SUFFOCATING:
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return true;
        case PLAYER_STATUS_FILTERED:
        case PLAYER_STATUS_OXYGEN_RESERVE:
        case PLAYER_STATUS_CAMP_RECOVERY:
        case PLAYER_STATUS_COUNT:
        default:
            return false;
    }
}

int Player_GetStatusPriority(PlayerStatusType status) {
    switch (status) {
        case PLAYER_STATUS_SUFFOCATING:
            return 90;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return 80;
        case PLAYER_STATUS_POISONED:
            return 70;
        case PLAYER_STATUS_OXYGEN_LEAK:
            return 60;
        case PLAYER_STATUS_LOW_OXYGEN:
            return 50;
        case PLAYER_STATUS_CAMP_RECOVERY:
            return 30;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            return 20;
        case PLAYER_STATUS_FILTERED:
            return 10;
        case PLAYER_STATUS_COUNT:
        default:
            return 0;
    }
}

int Player_GetActiveStatusCount(const Player *player) {
    int count;
    int index;

    if (player == NULL) {
        return 0;
    }

    count = 0;
    for (index = 0; index < PLAYER_STATUS_COUNT; index++) {
        if (player->statusEffects[index].active) {
            count += 1;
        }
    }

    return count;
}

int Player_CollectActiveStatuses(const Player *player, PlayerStatusType *statuses, int maxStatuses) {
    int count;
    int index;
    int left;

    if (player == NULL || statuses == NULL || maxStatuses <= 0) {
        return 0;
    }

    count = 0;
    for (index = 0; index < PLAYER_STATUS_COUNT && count < maxStatuses; index++) {
        if (!player->statusEffects[index].active) {
            continue;
        }

        statuses[count] = (PlayerStatusType)index;
        count += 1;
    }

    for (left = 0; left < count; left++) {
        int right;

        for (right = left + 1; right < count; right++) {
            if (CompareStatusForDisplay(player, statuses[left], statuses[right]) > 0) {
                PlayerStatusType swap;

                swap = statuses[left];
                statuses[left] = statuses[right];
                statuses[right] = swap;
            }
        }
    }

    return count;
}
