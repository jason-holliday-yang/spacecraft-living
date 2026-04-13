#include "player.h"

#include <stddef.h>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static int GetPoisonTier(const Player *player) {
    if (player == NULL) {
        return 0;
    }
    if (player->poison >= 70.0f) {
        return 3;
    }
    if (player->poison >= 35.0f) {
        return 2;
    }
    if (player->poison > 0.0f) {
        return 1;
    }
    return 0;
}

static Vector2 LerpVector2(Vector2 from, Vector2 to, float t) {
    return (Vector2){
        from.x + (to.x - from.x) * t,
        from.y + (to.y - from.y) * t
    };
}

static float SmoothStep(float t) {
    if (t <= 0.0f) {
        return 0.0f;
    }
    if (t >= 1.0f) {
        return 1.0f;
    }
    return t * t * (3.0f - 2.0f * t);
}

void Player_Init(Player *player) {
    int resourceIndex;

    player->gridX = PLAYER_START_X;
    player->gridY = PLAYER_START_Y;
    player->facingX = 0;
    player->facingY = 1;
    player->worldPos = Map_GridToWorld(player->gridX, player->gridY);
    player->renderPos = player->worldPos;
    player->moveStartPos = player->worldPos;
    player->moveTargetPos = player->worldPos;
    player->health = INITIAL_HEALTH;
    player->stamina = INITIAL_STAMINA;
    player->pressure = INITIAL_PRESSURE;
    player->oxygen = INITIAL_OXYGEN;
    player->poison = 0.0f;
    player->moveTimer = 0.0f;
    player->safeRecoveryTimer = 0.0f;
    player->glowStickTimer = 0.0f;
    player->speedBoostTimer = 0.0f;
    player->blurPulse = 0.0f;
    player->moveAnimElapsed = 0.0f;
    player->moveAnimDuration = 0.0f;
    player->spriteAnimTimer = 0.0f;
    player->maxHealthBonus = 0.0f;
    player->maxStaminaBonus = 0.0f;
    player->attackBonus = 0.0f;
    player->deathCount = 0;
    player->lastFoodType = -1;
    player->repeatedFoodCount = 0;
    player->crouching = false;
    player->hasAxe = true;
    player->hasKnife = true;
    player->hasGlowStick = false;
    player->hasRope = false;
    player->hasLaserGun = false;
    player->hasProtectionSuit = false;
    player->hasSignalAmplifier = false;
    player->hasFieldCamp = false;
    Player_ClearAllStatuses(player);

    for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; resourceIndex++) {
        player->resources[resourceIndex] = 0;
    }
}

void Player_UpdateWorldPosition(Player *player) {
    player->worldPos = Map_GridToWorld(player->gridX, player->gridY);
    player->renderPos = player->worldPos;
    player->moveStartPos = player->worldPos;
    player->moveTargetPos = player->worldPos;
    player->moveAnimElapsed = 0.0f;
    player->moveAnimDuration = 0.0f;
}

bool Player_Move(Player *player, const GameMap *map, int deltaX, int deltaY) {
    int targetX;
    int targetY;

    if (deltaX != 0 || deltaY != 0) {
        player->facingX = deltaX;
        player->facingY = deltaY;
    }

    targetX = player->gridX + deltaX;
    targetY = player->gridY + deltaY;

    if (!Map_IsWalkable(map, targetX, targetY)) {
        return false;
    }

    player->gridX = targetX;
    player->gridY = targetY;
    player->worldPos = Map_GridToWorld(player->gridX, player->gridY);
    return true;
}

void Player_StartMoveAnimation(Player *player, Vector2 startPos, float duration) {
    player->moveStartPos = startPos;
    player->moveTargetPos = player->worldPos;
    player->renderPos = startPos;
    player->moveAnimElapsed = 0.0f;
    player->moveAnimDuration = duration > 0.0f ? duration : 0.01f;
}

void Player_UpdateAnimation(Player *player, float deltaTime) {
    if (player->moveAnimDuration > 0.0f) {
        float progress;

        player->moveAnimElapsed += deltaTime;
        progress = player->moveAnimElapsed / player->moveAnimDuration;
        if (progress >= 1.0f) {
            progress = 1.0f;
            player->moveAnimDuration = 0.0f;
            player->moveAnimElapsed = 0.0f;
        }

        player->renderPos = LerpVector2(player->moveStartPos, player->moveTargetPos, SmoothStep(progress));
        player->spriteAnimTimer += deltaTime;
        return;
    }

    player->renderPos = player->worldPos;
    player->spriteAnimTimer = 0.0f;
}

bool Player_IsMoveAnimating(const Player *player) {
    return player->moveAnimDuration > 0.0f;
}

float Player_GetMoveCooldown(const Player *player) {
    float cooldown;

    if (player->crouching) {
        cooldown = CROUCH_MOVE_COOLDOWN;
    } else if (player->oxygen <= 28.0f
               || Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN)
               || Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)
               || Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)
               || GetPoisonTier(player) >= 2) {
        cooldown = LOW_STAMINA_MOVE_COOLDOWN;
    } else {
        cooldown = NORMAL_MOVE_COOLDOWN;
    }

    if (player->speedBoostTimer > 0.0f) {
        cooldown *= 0.80f;
    }

    return cooldown;
}

float Player_GetMaxHealth(const Player *player) {
    return BASE_MAX_HEALTH + player->maxHealthBonus;
}

float Player_GetMaxStamina(const Player *player) {
    return BASE_MAX_STAMINA + player->maxStaminaBonus;
}

float Player_GetCurrentStaminaCap(const Player *player) {
    return Player_GetMaxStamina(player);
}

float Player_GetAttackPower(const Player *player) {
    float basePower;

    basePower = player->hasLaserGun ? 28.0f : 16.0f;
    if (Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION) || player->health <= 35.0f) {
        basePower *= 0.88f;
    }
    if (Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN) || player->oxygen < 30.0f) {
        basePower *= 0.86f;
    }
    if (Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        basePower *= 0.92f;
    }
    if (GetPoisonTier(player) >= 2) {
        basePower *= 0.90f;
    }
    return basePower + player->attackBonus;
}

float Player_GetAggroMultiplier(const Player *player) {
    if (Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)) {
        return 1.40f;
    }
    if (Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN) || Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        return 1.18f;
    }
    return 1.0f;
}

float Player_GetGatherMultiplier(const Player *player) {
    float multiplier;

    multiplier = 1.0f;
    if (player->oxygen < 35.0f || Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN)) {
        multiplier += 0.15f;
    }
    if (Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)) {
        multiplier += 0.12f;
    }
    if (Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION) || player->health < 35.0f) {
        multiplier += 0.14f;
    }
    multiplier += 0.08f * (float)GetPoisonTier(player);
    if (multiplier > 1.45f) {
        multiplier = 1.45f;
    }
    return multiplier;
}

bool Player_CanCraftAdvanced(const Player *player) {
    if (player == NULL) {
        return false;
    }

    return player->health >= 45.0f
        && player->oxygen >= 35.0f
        && !Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN)
        && !Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)
        && !Player_HasStatus(player, PLAYER_STATUS_OXYGEN_LEAK)
        && !Player_HasStatus(player, PLAYER_STATUS_CRITICAL_CONDITION)
        && GetPoisonTier(player) < 2;
}

bool Player_IsInDanger(const Player *player) {
    return player->health <= 35.0f
        || player->oxygen <= 10.0f
        || Player_HasStatus(player, PLAYER_STATUS_SUFFOCATING)
        || Player_HasStatus(player, PLAYER_STATUS_POISONED);
}

void Player_DamageHealth(Player *player, float amount) {
    player->health = ClampFloat(player->health - amount, 0.0f, Player_GetMaxHealth(player));
}

void Player_RecoverHealth(Player *player, float amount) {
    player->health = ClampFloat(player->health + amount, 0.0f, Player_GetMaxHealth(player));
    if (player->health > 35.0f) {
        Player_ClearStatus(player, PLAYER_STATUS_CRITICAL_CONDITION);
    }
}

void Player_ConsumeStamina(Player *player, float amount) {
    player->stamina = ClampFloat(player->stamina - amount, 0.0f, Player_GetCurrentStaminaCap(player));
}

void Player_RecoverStamina(Player *player, float amount) {
    player->stamina = ClampFloat(player->stamina + amount, 0.0f, Player_GetCurrentStaminaCap(player));
}

void Player_AddPressure(Player *player, float amount) {
    (void)amount;
    if (player != NULL) {
        player->pressure = INITIAL_PRESSURE;
    }
}

void Player_RelievePressure(Player *player, float amount) {
    (void)amount;
    if (player != NULL) {
        player->pressure = INITIAL_PRESSURE;
    }
}

void Player_AddOxygen(Player *player, float amount) {
    player->oxygen = ClampFloat(player->oxygen + amount, 0.0f, MAX_OXYGEN);
    if (player->oxygen > 0.0f) {
        Player_ClearStatus(player, PLAYER_STATUS_SUFFOCATING);
    }
    if (player->oxygen >= 30.0f) {
        Player_ClearStatus(player, PLAYER_STATUS_LOW_OXYGEN);
    } else if (player->oxygen > 10.0f && Player_HasStatus(player, PLAYER_STATUS_LOW_OXYGEN)) {
        const PlayerStatusEffect *effect;

        effect = Player_GetStatusEffect(player, PLAYER_STATUS_LOW_OXYGEN);
        if (effect != NULL && effect->level > 1) {
            Player_DowngradeStatus(player, PLAYER_STATUS_LOW_OXYGEN, 1, effect->remainingTime, player->oxygen);
        }
    }
}

void Player_DamageOxygen(Player *player, float amount) {
    player->oxygen = ClampFloat(player->oxygen - amount, 0.0f, MAX_OXYGEN);
}

void Player_AddPoison(Player *player, float amount) {
    float multiplier;

    multiplier = player->hasProtectionSuit ? 0.60f : 1.0f;
    player->poison = ClampFloat(player->poison + amount * multiplier, 0.0f, MAX_POISON);
}

void Player_ClearPoison(Player *player) {
    player->poison = 0.0f;
    Player_ClearStatus(player, PLAYER_STATUS_POISONED);
}
