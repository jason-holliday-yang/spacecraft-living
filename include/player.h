#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "map.h"

typedef enum ResourceType {
    RESOURCE_WOOD = 0,
    RESOURCE_ORE,
    RESOURCE_METAL_SCRAP,
    RESOURCE_FRUIT,
    RESOURCE_SPECIAL_FUNGUS,
    RESOURCE_ENERGY_CORE,
    RESOURCE_GLOW_MOSS,
    RESOURCE_ALIEN_VINE,
    RESOURCE_SHELL_FRUIT,
    RESOURCE_JUNK_METAL,
    RESOURCE_ENERGY_CRYSTAL,
    RESOURCE_CALM_MUSHROOM,
    RESOURCE_PROTECTIVE_FIBER,
    RESOURCE_RELIC_FRAGMENT,
    RESOURCE_BOSS_SCALE,
    RESOURCE_ALIEN_SLIME,
    RESOURCE_COUNT
} ResourceType;

typedef enum RecipeType {
    RECIPE_GLOW_STICK = 0,
    RECIPE_ROPE,
    RECIPE_REINFORCED_METAL,
    RECIPE_LASER_GUN,
    RECIPE_PROTECTION_SUIT,
    RECIPE_SIGNAL_AMPLIFIER,
    RECIPE_FIELD_CAMP,
    RECIPE_COUNT
} RecipeType;

typedef enum ConsumableFocus {
    CONSUMABLE_FOOD = 0,
    CONSUMABLE_CALM
} ConsumableFocus;

typedef struct Player {
    int gridX;
    int gridY;
    int facingX;
    int facingY;
    Vector2 worldPos;
    float stamina;
    float pressure;
    float oxygen;
    float poison;
    float moveTimer;
    float safeRecoveryTimer;
    float pressureDamageTimer;
    float glowStickTimer;
    float speedBoostTimer;
    float noPressureTimer;
    float blurPulse;
    float maxStaminaBonus;
    float attackBonus;
    int deathCount;
    int lastFoodType;
    int repeatedFoodCount;
    bool crouching;
    bool hasAxe;
    bool hasKnife;
    bool hasGlowStick;
    bool hasRope;
    bool hasLaserGun;
    bool hasProtectionSuit;
    bool hasSignalAmplifier;
    bool hasFieldCamp;
    int resources[RESOURCE_COUNT];
} Player;

void Player_Init(Player *player);
void Player_UpdateWorldPosition(Player *player);
bool Player_Move(Player *player, const GameMap *map, int deltaX, int deltaY);
float Player_GetMoveCooldown(const Player *player);
float Player_GetMaxStamina(const Player *player);
float Player_GetCurrentStaminaCap(const Player *player);
float Player_GetAttackPower(const Player *player);
float Player_GetAggroMultiplier(const Player *player);
float Player_GetGatherMultiplier(const Player *player);
bool Player_CanCraftAdvanced(const Player *player);
bool Player_IsInDanger(const Player *player);
void Player_ConsumeStamina(Player *player, float amount);
void Player_RecoverStamina(Player *player, float amount);
void Player_AddPressure(Player *player, float amount);
void Player_RelievePressure(Player *player, float amount);
void Player_AddOxygen(Player *player, float amount);
void Player_DamageOxygen(Player *player, float amount);
void Player_AddPoison(Player *player, float amount);
void Player_ClearPoison(Player *player);
void Player_AddResource(Player *player, ResourceType resource, int amount);
bool Player_SpendResource(Player *player, ResourceType resource, int amount);
bool Player_HasResources(const Player *player, ResourceType resource, int amount);
bool Player_UseQuickConsumable(Player *player, ConsumableFocus focus, char *message, int messageSize);
const char *Player_GetResourceLabel(ResourceType resource);
const char *Player_GetRecipeName(RecipeType recipe);
const char *Player_GetRecipeSummary(RecipeType recipe);

#endif
