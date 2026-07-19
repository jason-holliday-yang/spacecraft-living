#ifndef PLAYER_PRESENTATION_H
#define PLAYER_PRESENTATION_H

#include "c_compat.h"
#include "player.h"

/* Localized player status, inventory, and recipe presentation text. */

SCL_EXTERN_C_BEGIN

const char *Player_GetStatusName(PlayerStatusType status);
void Player_GetStatusSummary(const Player *player, PlayerStatusType status, char *buffer, int bufferSize);
void Player_GetStatusSourceText(PlayerStatusType status, char *buffer, int bufferSize);
void Player_GetStatusReliefText(PlayerStatusType status, char *buffer, int bufferSize);
void Player_GetStatusTooltip(const Player *player, PlayerStatusType status, char *buffer, int bufferSize);
const char *Player_GetResourceLabel(ResourceType resource);
const char *Player_GetRecipeName(RecipeType recipe);
const char *Player_GetRecipeSummary(RecipeType recipe);

SCL_EXTERN_C_END

#endif
