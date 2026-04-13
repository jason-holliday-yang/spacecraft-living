#include "player.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void CopyText(char *buffer, int bufferSize, const char *text) {
    if (buffer != NULL && bufferSize > 0) {
        snprintf(buffer, (size_t)bufferSize, "%s", text != NULL ? text : "");
    }
}

const char *Player_GetStatusName(PlayerStatusType status) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            return "Poisoned";
        case PLAYER_STATUS_OXYGEN_LEAK:
            return "Oxygen Leak";
        case PLAYER_STATUS_LOW_OXYGEN:
            return "Low Oxygen";
        case PLAYER_STATUS_SUFFOCATING:
            return "Suffocating";
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return "Critical Condition";
        case PLAYER_STATUS_FILTERED:
            return "Filtered";
        case PLAYER_STATUS_OXYGEN_RESERVE:
            return "Oxygen Reserve";
        case PLAYER_STATUS_CAMP_RECOVERY:
            return "Camp Recovery";
        case PLAYER_STATUS_COUNT:
        default:
            return "Unknown Status";
    }
}

void Player_GetStatusSummary(const Player *player, PlayerStatusType status, char *buffer, int bufferSize) {
    const PlayerStatusEffect *effect;

    effect = Player_GetStatusEffect(player, status);
    if (effect == NULL || !effect->active) {
        CopyText(buffer, bufferSize, "Inactive");
        return;
    }

    switch (status) {
        case PLAYER_STATUS_POISONED:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "Lv.%d poison: drains health and increases oxygen loss.",
                     effect->level);
            break;
        case PLAYER_STATUS_OXYGEN_LEAK:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "Lv.%d leak: extra oxygen drain %.1f/s.",
                     effect->level,
                     effect->magnitude);
            break;
        case PLAYER_STATUS_LOW_OXYGEN:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "Lv.%d warning: oxygen at %.0f.",
                     effect->level,
                     player != NULL ? player->oxygen : 0.0f);
            break;
        case PLAYER_STATUS_SUFFOCATING:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "Critical suffocation: health drains until oxygen returns.");
            break;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "Lv.%d critical: health at %.0f.",
                     effect->level,
                     player != NULL ? player->health : 0.0f);
            break;
        case PLAYER_STATUS_FILTERED:
            CopyText(buffer, bufferSize, "Filtered: hazard oxygen drain and poison buildup are reduced.");
            break;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "Reserve active: bonus oxygen support %.1f.",
                     effect->magnitude);
            break;
        case PLAYER_STATUS_CAMP_RECOVERY:
            CopyText(buffer, bufferSize, "Recovery boost: gradual healing, oxygen stabilization, and poison relief while retreating.");
            break;
        case PLAYER_STATUS_COUNT:
        default:
            CopyText(buffer, bufferSize, "Unknown status.");
            break;
    }
}

void Player_GetStatusSourceText(PlayerStatusType status, char *buffer, int bufferSize) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            CopyText(buffer, bufferSize, "Source: toxic hazards, deep swamp spores, or venomous attacks.");
            break;
        case PLAYER_STATUS_OXYGEN_LEAK:
            CopyText(buffer, bufferSize, "Source: damaged equipment, harsh terrain, or environmental wear.");
            break;
        case PLAYER_STATUS_LOW_OXYGEN:
            CopyText(buffer, bufferSize, "Source: oxygen dropping below the safety threshold.");
            break;
        case PLAYER_STATUS_SUFFOCATING:
            CopyText(buffer, bufferSize, "Source: oxygen depleted completely.");
            break;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            CopyText(buffer, bufferSize, "Source: health falling into the danger zone.");
            break;
        case PLAYER_STATUS_FILTERED:
            CopyText(buffer, bufferSize, "Source: protection gear, filters, or defensive support effects.");
            break;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            CopyText(buffer, bufferSize, "Source: reserve oxygen canisters or emergency support devices.");
            break;
        case PLAYER_STATUS_CAMP_RECOVERY:
            CopyText(buffer, bufferSize, "Source: resting near the field camp or safe recovery point.");
            break;
        case PLAYER_STATUS_COUNT:
        default:
            CopyText(buffer, bufferSize, "Source: unknown.");
            break;
    }
}

void Player_GetStatusReliefText(PlayerStatusType status, char *buffer, int bufferSize) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            CopyText(buffer, bufferSize, "Relief: antidotes, calm mushrooms, camp rest, or full base treatment.");
            break;
        case PLAYER_STATUS_OXYGEN_LEAK:
            CopyText(buffer, bufferSize, "Relief: seal the suit, rest at camp, or return to base support.");
            break;
        case PLAYER_STATUS_LOW_OXYGEN:
            CopyText(buffer, bufferSize, "Relief: return to oxygen supply, camp support, or base air systems.");
            break;
        case PLAYER_STATUS_SUFFOCATING:
            CopyText(buffer, bufferSize, "Relief: restore oxygen immediately or retreat to a recovery point.");
            break;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            CopyText(buffer, bufferSize, "Relief: heal, disengage, and retreat before taking another hit.");
            break;
        case PLAYER_STATUS_FILTERED:
            CopyText(buffer, bufferSize, "Relief: no action needed while the protective effect remains active.");
            break;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            CopyText(buffer, bufferSize, "Relief: conserve movement and use the reserve buffer to retreat.");
            break;
        case PLAYER_STATUS_CAMP_RECOVERY:
            CopyText(buffer, bufferSize, "Relief: keep resting nearby to refresh it, then use the retreat window to disengage safely.");
            break;
        case PLAYER_STATUS_COUNT:
        default:
            CopyText(buffer, bufferSize, "Relief: unknown.");
            break;
    }
}

void Player_GetStatusTooltip(const Player *player, PlayerStatusType status, char *buffer, int bufferSize) {
    char summary[160];
    char source[160];
    char relief[160];
    const PlayerStatusEffect *effect;
    char durationText[64];

    if (buffer == NULL || bufferSize <= 0) {
        return;
    }

    effect = Player_GetStatusEffect(player, status);
    if (effect == NULL || !effect->active) {
        CopyText(buffer, bufferSize, "Inactive status");
        return;
    }

    Player_GetStatusSummary(player, status, summary, (int)sizeof(summary));
    Player_GetStatusSourceText(status, source, (int)sizeof(source));
    Player_GetStatusReliefText(status, relief, (int)sizeof(relief));

    if (effect->remainingTime > 0.0f) {
        snprintf(durationText, sizeof(durationText), "Duration: %.1fs", effect->remainingTime);
    } else {
        snprintf(durationText, sizeof(durationText), "Duration: persistent");
    }

    snprintf(buffer,
             (size_t)bufferSize,
             "%s\n%s\n%s\n%s\nLevel: %d",
             Player_GetStatusName(status),
             summary,
             source,
             relief,
             effect->level);
    if (effect->remainingTime >= 0.0f) {
        size_t used;

        used = strlen(buffer);
        if ((int)used < bufferSize - 1) {
            snprintf(buffer + used, (size_t)(bufferSize - (int)used), "\n%s", durationText);
        }
    }
}
