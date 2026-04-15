#include "player.h"

#include "localization.h"

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
            return Loc_PickLiteral("Poisoned", "中毒");
        case PLAYER_STATUS_OXYGEN_LEAK:
            return Loc_PickLiteral("Oxygen Leak", "漏氧");
        case PLAYER_STATUS_LOW_OXYGEN:
            return Loc_PickLiteral("Low Oxygen", "低氧");
        case PLAYER_STATUS_SUFFOCATING:
            return Loc_PickLiteral("Suffocating", "窒息");
        case PLAYER_STATUS_CRITICAL_CONDITION:
            return Loc_PickLiteral("Critical Condition", "危急状态");
        case PLAYER_STATUS_FILTERED:
            return Loc_PickLiteral("Filtered", "过滤呼吸");
        case PLAYER_STATUS_OXYGEN_RESERVE:
            return Loc_PickLiteral("Oxygen Reserve", "氧气储备");
        case PLAYER_STATUS_CAMP_RECOVERY:
            return Loc_PickLiteral("Camp Recovery", "营地恢复");
        case PLAYER_STATUS_COUNT:
        default:
            return Loc_PickLiteral("Unknown Status", "未知状态");
    }
}

void Player_GetStatusSummary(const Player *player, PlayerStatusType status, char *buffer, int bufferSize) {
    const PlayerStatusEffect *effect;

    effect = Player_GetStatusEffect(player, status);
    if (effect == NULL || !effect->active) {
        CopyText(buffer, bufferSize, Loc_PickLiteral("Inactive", "未激活"));
        return;
    }

    switch (status) {
        case PLAYER_STATUS_POISONED:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "%s %d：持续损失生命，并提高氧气消耗。",
                     Loc_PickLiteral("等级", "等级"),
                     effect->level);
            break;
        case PLAYER_STATUS_OXYGEN_LEAK:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "%s %d：额外氧气流失 %.1f/秒。",
                     Loc_PickLiteral("等级", "等级"),
                     effect->level,
                     effect->magnitude);
            break;
        case PLAYER_STATUS_LOW_OXYGEN:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "%s %d：当前氧气 %.0f。",
                     Loc_PickLiteral("等级", "等级"),
                     effect->level,
                     player != NULL ? player->oxygen : 0.0f);
            break;
        case PLAYER_STATUS_SUFFOCATING:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "%s",
                     Loc_PickLiteral("Critical suffocation: health drains until oxygen returns.", "严重窒息：在氧气恢复前会持续损失生命。"));
            break;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "%s %d：生命值降至 %.0f。",
                     Loc_PickLiteral("等级", "等级"),
                     effect->level,
                     player != NULL ? player->health : 0.0f);
            break;
        case PLAYER_STATUS_FILTERED:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Filtered: hazard oxygen drain and poison buildup are reduced.", "过滤呼吸：危险环境下的氧气消耗和毒素累积都会降低。"));
            break;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            snprintf(buffer,
                     (size_t)bufferSize,
                     "%s %.1f。",
                     Loc_PickLiteral("储备生效：额外氧气支持", "储备生效：额外氧气支持"),
                     effect->magnitude);
            break;
        case PLAYER_STATUS_CAMP_RECOVERY:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Recovery boost: gradual healing, oxygen stabilization, and poison relief while retreating.", "恢复增益：撤退时会缓慢治疗、稳定氧气并减轻毒素。"));
            break;
        case PLAYER_STATUS_COUNT:
        default:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Unknown status.", "未知状态。"));
            break;
    }
}

void Player_GetStatusSourceText(PlayerStatusType status, char *buffer, int bufferSize) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: toxic hazards, deep swamp spores, or venomous attacks.", "来源：毒性环境、深层沼泽孢子或带毒攻击。"));
            break;
        case PLAYER_STATUS_OXYGEN_LEAK:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: damaged equipment, harsh terrain, or environmental wear.", "来源：装备受损、恶劣地形或环境磨损。"));
            break;
        case PLAYER_STATUS_LOW_OXYGEN:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: oxygen dropping below the safety threshold.", "来源：氧气低于安全阈值。"));
            break;
        case PLAYER_STATUS_SUFFOCATING:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: oxygen depleted completely.", "来源：氧气完全耗尽。"));
            break;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: health falling into the danger zone.", "来源：生命值降入危险区间。"));
            break;
        case PLAYER_STATUS_FILTERED:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: protection gear, filters, or defensive support effects.", "来源：防护装备、过滤装置或防御型支援效果。"));
            break;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: reserve oxygen canisters or emergency support devices.", "来源：备用氧气装置或紧急支援设备。"));
            break;
        case PLAYER_STATUS_CAMP_RECOVERY:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: resting near the field camp or safe recovery point.", "来源：在野外营地或安全恢复点附近休整。"));
            break;
        case PLAYER_STATUS_COUNT:
        default:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Source: unknown.", "来源：未知。"));
            break;
    }
}

void Player_GetStatusReliefText(PlayerStatusType status, char *buffer, int bufferSize) {
    switch (status) {
        case PLAYER_STATUS_POISONED:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: antidotes, calm mushrooms, camp rest, or full base treatment.", "缓解方式：解毒物、宁神蘑菇、在营地休息，或基地完整治疗。"));
            break;
        case PLAYER_STATUS_OXYGEN_LEAK:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: seal the suit, rest at camp, or return to base support.", "缓解方式：修补防护服、在营地休息，或返回基地支援。"));
            break;
        case PLAYER_STATUS_LOW_OXYGEN:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: return to oxygen supply, camp support, or base air systems.", "缓解方式：返回补氧点、营地支援点或基地空气系统。"));
            break;
        case PLAYER_STATUS_SUFFOCATING:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: restore oxygen immediately or retreat to a recovery point.", "缓解方式：立刻恢复氧气，或撤退到恢复点。"));
            break;
        case PLAYER_STATUS_CRITICAL_CONDITION:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: heal, disengage, and retreat before taking another hit.", "缓解方式：先治疗、脱离战斗，并在再次受击前撤退。"));
            break;
        case PLAYER_STATUS_FILTERED:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: no action needed while the protective effect remains active.", "缓解方式：在保护效果持续期间无需额外处理。"));
            break;
        case PLAYER_STATUS_OXYGEN_RESERVE:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: conserve movement and use the reserve buffer to retreat.", "缓解方式：减少移动消耗，并利用储备氧气完成撤退。"));
            break;
        case PLAYER_STATUS_CAMP_RECOVERY:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: keep resting nearby to refresh it, then use the retreat window to disengage safely.", "缓解方式：继续在附近休整以刷新效果，再利用这段撤退窗口安全脱离。"));
            break;
        case PLAYER_STATUS_COUNT:
        default:
            CopyText(buffer, bufferSize, Loc_PickLiteral("Relief: unknown.", "缓解方式：未知。"));
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
        CopyText(buffer, bufferSize, Loc_PickLiteral("Inactive status", "未激活状态"));
        return;
    }

    Player_GetStatusSummary(player, status, summary, (int)sizeof(summary));
    Player_GetStatusSourceText(status, source, (int)sizeof(source));
    Player_GetStatusReliefText(status, relief, (int)sizeof(relief));

    if (effect->remainingTime > 0.0f) {
        snprintf(durationText, sizeof(durationText), "%s: %.1f%s", Loc_PickLiteral("Duration", "持续时间"), effect->remainingTime, Loc_PickLiteral("s", "秒"));
    } else {
        snprintf(durationText, sizeof(durationText), "%s: %s", Loc_PickLiteral("Duration", "持续时间"), Loc_PickLiteral("persistent", "持续"));
    }

    snprintf(buffer,
             (size_t)bufferSize,
             "%s\n%s\n%s\n%s\n%s: %d",
             Player_GetStatusName(status),
             summary,
             source,
             relief,
             Loc_PickLiteral("Level", "等级"),
             effect->level);
    if (effect->remainingTime >= 0.0f) {
        size_t used;

        used = strlen(buffer);
        if ((int)used < bufferSize - 1) {
            snprintf(buffer + used, (size_t)(bufferSize - (int)used), "\n%s", durationText);
        }
    }
}
