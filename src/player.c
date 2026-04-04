#include "player.h"

#include <stddef.h>
#include <stdio.h>

static float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void WriteMessage(char *message, int messageSize, const char *text) {
    if (message != NULL && messageSize > 0) {
        snprintf(message, (size_t)messageSize, "%s", text);
    }
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

void Player_Init(Player *player) {
    int resourceIndex;

    player->gridX = PLAYER_START_X;
    player->gridY = PLAYER_START_Y;
    player->facingX = 0;
    player->facingY = 1;
    player->worldPos = Map_GridToWorld(player->gridX, player->gridY);
    player->stamina = INITIAL_STAMINA;
    player->pressure = INITIAL_PRESSURE;
    player->oxygen = INITIAL_OXYGEN;
    player->poison = 0.0f;
    player->moveTimer = 0.0f;
    player->safeRecoveryTimer = 0.0f;
    player->pressureDamageTimer = 0.0f;
    player->glowStickTimer = 0.0f;
    player->speedBoostTimer = 0.0f;
    player->noPressureTimer = 0.0f;
    player->blurPulse = 0.0f;
    player->maxStaminaBonus = 0.0f;
    player->attackBonus = 0.0f;
    player->deathCount = 0;
    player->lastFoodType = -1;
    player->repeatedFoodCount = 0;
    player->crouching = false;
    player->isDowned = false;
    player->downedTimer = 0.0f;
    player->hasAxe = true;
    player->hasKnife = true;
    player->hasGlowStick = false;
    player->hasRope = false;
    player->hasLaserGun = false;
    player->hasProtectionSuit = false;
    player->hasSignalAmplifier = false;
    player->hasFieldCamp = false;

    for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; resourceIndex++) {
        player->resources[resourceIndex] = 0;
    }
}

void Player_UpdateWorldPosition(Player *player) {
    player->worldPos = Map_GridToWorld(player->gridX, player->gridY);
}

bool Player_Move(Player *player, const GameMap *map, int deltaX, int deltaY) {
    int targetX;
    int targetY;

    targetX = player->gridX + deltaX;
    targetY = player->gridY + deltaY;

    if (!Map_IsWalkable(map, targetX, targetY)) {
        return false;
    }

    player->gridX = targetX;
    player->gridY = targetY;
    if (deltaX != 0 || deltaY != 0) {
        player->facingX = deltaX;
        player->facingY = deltaY;
    }
    Player_UpdateWorldPosition(player);
    return true;
}

float Player_GetMoveCooldown(const Player *player) {
    float cooldown;

    if (player->crouching) {
        cooldown = CROUCH_MOVE_COOLDOWN;
    } else if (player->stamina <= STAMINA_LOW_THRESHOLD) {
        cooldown = LOW_STAMINA_MOVE_COOLDOWN;
    } else {
        cooldown = NORMAL_MOVE_COOLDOWN;
    }

    if (player->speedBoostTimer > 0.0f) {
        cooldown *= 0.80f;
    }

    return cooldown;
}

float Player_GetMaxStamina(const Player *player) {
    return BASE_MAX_STAMINA + player->maxStaminaBonus;
}

float Player_GetCurrentStaminaCap(const Player *player) {
    float cap;

    cap = Player_GetMaxStamina(player);
    if (player->oxygen < 30.0f) {
        cap *= 0.80f;
    }
    return cap;
}

float Player_GetAttackPower(const Player *player) {
    float basePower;

    basePower = player->hasLaserGun ? 28.0f : 16.0f;
    if (player->pressure >= 30.0f) {
        basePower *= 0.90f;
    }
    if (player->oxygen < 30.0f) {
        basePower *= 0.85f;
    }
    return basePower + player->attackBonus;
}

float Player_GetAggroMultiplier(const Player *player) {
    if (player->pressure >= 80.0f) {
        return 1.50f;
    }
    return 1.0f;
}

float Player_GetGatherMultiplier(const Player *player) {
    if (player->pressure >= 30.0f || player->oxygen < 30.0f) {
        return 1.10f;
    }
    return 1.0f;
}

bool Player_CanCraftAdvanced(const Player *player) {
    return player->pressure < 60.0f;
}

bool Player_IsInDanger(const Player *player) {
    return player->stamina <= 0.0f || player->oxygen <= 10.0f || player->pressure >= 80.0f;
}

void Player_ConsumeStamina(Player *player, float amount) {
    player->stamina = ClampFloat(player->stamina - amount, 0.0f, Player_GetCurrentStaminaCap(player));
}

void Player_RecoverStamina(Player *player, float amount) {
    player->stamina = ClampFloat(player->stamina + amount, 0.0f, Player_GetCurrentStaminaCap(player));
}

void Player_AddPressure(Player *player, float amount) {
    if (player->noPressureTimer > 0.0f) {
        return;
    }
    player->pressure = ClampFloat(player->pressure + amount, 0.0f, MAX_PRESSURE);
}

void Player_RelievePressure(Player *player, float amount) {
    player->pressure = ClampFloat(player->pressure - amount, 0.0f, MAX_PRESSURE);
}

void Player_AddOxygen(Player *player, float amount) {
    player->oxygen = ClampFloat(player->oxygen + amount, 0.0f, MAX_OXYGEN);
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
}

void Player_AddResource(Player *player, ResourceType resource, int amount) {
    if (resource < 0 || resource >= RESOURCE_COUNT || amount <= 0) {
        return;
    }
    player->resources[resource] += amount;
}

bool Player_SpendResource(Player *player, ResourceType resource, int amount) {
    if (resource < 0 || resource >= RESOURCE_COUNT || amount < 0) {
        return false;
    }
    if (player->resources[resource] < amount) {
        return false;
    }
    player->resources[resource] -= amount;
    return true;
}

bool Player_HasResources(const Player *player, ResourceType resource, int amount) {
    if (resource < 0 || resource >= RESOURCE_COUNT || amount < 0) {
        return false;
    }
    return player->resources[resource] >= amount;
}

bool Player_UseQuickConsumable(Player *player, ConsumableFocus focus, char *message, int messageSize) {
    float modifier;

    if (focus == CONSUMABLE_FOOD) {
        if (player->resources[RESOURCE_FRUIT] > 0 && player->resources[RESOURCE_SHELL_FRUIT] > 0) {
            modifier = GetRepeatModifier(player, 100);
            player->resources[RESOURCE_FRUIT] -= 1;
            player->resources[RESOURCE_SHELL_FRUIT] -= 1;
            Player_RecoverStamina(player, 35.0f * modifier);
            player->speedBoostTimer = 30.0f;
            WriteMessage(message, messageSize, "Fruit combo consumed: stamina restored and speed boosted.");
            return true;
        }

        if (player->resources[RESOURCE_SHELL_FRUIT] > 0) {
            modifier = GetRepeatModifier(player, 101);
            player->resources[RESOURCE_SHELL_FRUIT] -= 1;
            Player_RecoverStamina(player, 15.0f * modifier);
            player->speedBoostTimer = 20.0f;
            WriteMessage(message, messageSize, "Shell fruit consumed: stamina restored and sprint boosted.");
            return true;
        }

        if (player->resources[RESOURCE_FRUIT] > 0) {
            modifier = GetRepeatModifier(player, 102);
            player->resources[RESOURCE_FRUIT] -= 1;
            Player_RecoverStamina(player, 12.0f * modifier);
            WriteMessage(message, messageSize, "Plant fruit consumed: small stamina recovery.");
            return true;
        }

        WriteMessage(message, messageSize, "No quick recovery food available.");
        return false;
    }

    if (player->resources[RESOURCE_SPECIAL_FUNGUS] > 0 && player->resources[RESOURCE_CALM_MUSHROOM] > 0) {
        player->resources[RESOURCE_SPECIAL_FUNGUS] -= 1;
        player->resources[RESOURCE_CALM_MUSHROOM] -= 1;
        Player_RelievePressure(player, 50.0f);
        player->noPressureTimer = 60.0f;
        Player_ClearPoison(player);
        WriteMessage(message, messageSize, "Mushroom combo used: stress reduced and growth paused.");
        return true;
    }

    if (player->resources[RESOURCE_CALM_MUSHROOM] > 0) {
        player->resources[RESOURCE_CALM_MUSHROOM] -= 1;
        Player_RelievePressure(player, 30.0f);
        player->noPressureTimer = 30.0f;
        Player_ClearPoison(player);
        WriteMessage(message, messageSize, "Calming mushroom used: stress reduced and poison cleared.");
        return true;
    }

    if (player->resources[RESOURCE_SPECIAL_FUNGUS] > 0) {
        player->resources[RESOURCE_SPECIAL_FUNGUS] -= 1;
        Player_RelievePressure(player, 20.0f);
        WriteMessage(message, messageSize, "Special fungus used: stress slightly reduced.");
        return true;
    }

    if (player->resources[RESOURCE_GLOW_MOSS] > 0) {
        player->resources[RESOURCE_GLOW_MOSS] -= 1;
        Player_RelievePressure(player, 10.0f);
        WriteMessage(message, messageSize, "Glow moss helped stabilize your stress.");
        return true;
    }

    WriteMessage(message, messageSize, "No quick stress-relief item available.");
    return false;
}

const char *Player_GetResourceLabel(ResourceType resource) {
    switch (resource) {
        case RESOURCE_WOOD:
            return "Wood";
        case RESOURCE_ORE:
            return "Ore";
        case RESOURCE_METAL_SCRAP:
            return "Metal Scrap";
        case RESOURCE_FRUIT:
            return "Plant Fruit";
        case RESOURCE_SPECIAL_FUNGUS:
            return "Special Fungus";
        case RESOURCE_ENERGY_CORE:
            return "Energy Core";
        case RESOURCE_GLOW_MOSS:
            return "Glow Moss";
        case RESOURCE_ALIEN_VINE:
            return "Alien Vine";
        case RESOURCE_SHELL_FRUIT:
            return "Shell Fruit";
        case RESOURCE_JUNK_METAL:
            return "Junk Metal";
        case RESOURCE_ENERGY_CRYSTAL:
            return "Energy Crystal";
        case RESOURCE_CALM_MUSHROOM:
            return "Calming Mushroom";
        case RESOURCE_PROTECTIVE_FIBER:
            return "Protective Fiber";
        case RESOURCE_RELIC_FRAGMENT:
            return "Relic Fragment";
        case RESOURCE_BOSS_SCALE:
            return "Boss Scale";
        case RESOURCE_ALIEN_SLIME:
            return "Alien Slime";
        case RESOURCE_COUNT:
        default:
            return "Unknown Resource";
    }
}

const char *Player_GetRecipeName(RecipeType recipe) {
    switch (recipe) {
        case RECIPE_GLOW_STICK:
            return "Glow Stick";
        case RECIPE_ROPE:
            return "Simple Rope";
        case RECIPE_REINFORCED_METAL:
            return "Reinforced Metal";
        case RECIPE_LASER_GUN:
            return "Laser Gun";
        case RECIPE_PROTECTION_SUIT:
            return "Protection Suit";
        case RECIPE_SIGNAL_AMPLIFIER:
            return "Signal Amplifier";
        case RECIPE_FIELD_CAMP:
            return "Field Camp";
        case RECIPE_COUNT:
        default:
            return "Unknown Recipe";
    }
}

const char *Player_GetRecipeSummary(RecipeType recipe) {
    switch (recipe) {
        case RECIPE_GLOW_STICK:
            return "1 Wood + 1 Glow Moss. Expands night vision.";
        case RECIPE_ROPE:
            return "1 Wood + 2 Vines. Crosses swamp and solves shortcuts.";
        case RECIPE_REINFORCED_METAL:
            return "1 Junk Metal + 2 Metal Scrap. Prepares laser parts.";
        case RECIPE_LASER_GUN:
            return "1 Reinforced Metal + 2 Ore. Core ranged weapon.";
        case RECIPE_PROTECTION_SUIT:
            return "1 Wood + 2 Vines + 1 Fiber. Reduces damage and poison buildup.";
        case RECIPE_SIGNAL_AMPLIFIER:
            return "1 Energy Core + 3 Relic Fragments. Peaceful tower activation.";
        case RECIPE_FIELD_CAMP:
            return "5 Wood. Build a temporary outdoor camp.";
        case RECIPE_COUNT:
        default:
            return "";
    }
}
