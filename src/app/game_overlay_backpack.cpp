#include "game_overlay_gameplay_internal.h"

#include "localization.h"
#include "ui_inventory_internal.h"
#include "ui_story_internal.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>

static bool TryUseSelectedBackpackItem(Game *game, char *message, size_t messageSize) {
    const BackpackEntryDef *entry;
    ResourceType resource;

    entry = UIInventory_GetBackpackEntry(game->ui.selectedBackpackItem);
    if (entry == NULL) {
        if (message != NULL && messageSize > 0) {
            message[0] = '\0';
        }
        return false;
    }

    if (entry->kind == BACKPACK_ENTRY_GEAR) {
        const BackpackGearId gearId = (BackpackGearId)entry->itemId;

        if (!UIInventory_BackpackEntryIsOwned(&game->runtime.player, game->ui.selectedBackpackItem)) {
            std::snprintf(message, messageSize, "%s", Loc_PickLiteral("That gear is not built yet.", "这件装备还没有制作出来。"));
            return false;
        }

        switch (gearId) {
            case BACKPACK_GEAR_ROPE: {
                const int targetX = game->runtime.player.gridX + game->runtime.player.facingX;
                const int targetY = game->runtime.player.gridY + game->runtime.player.facingY;

                if (Map_CanCrossWithRope(&game->runtime.map, game->runtime.player.gridX, game->runtime.player.gridY, targetX, targetY)) {
                    Map_CreateRopeBridge(&game->runtime.map, targetX, targetY);
                    std::snprintf(message,
                                  messageSize,
                                  "%s",
                                  Loc_PickLiteral("Rope deployed. The hazard is now tied into a usable shortcut.",
                                                  "绳索已部署。眼前的险地已经被固定成可用捷径。"));
                    return true;
                }

                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Rope readied. Face a marked swamp or deep barrier, then press F or use it here.",
                                              "绳索已准备好。面对标记出的沼泽或深水阻隔后，按 F 或在背包中使用它。"));
                return true;
            }
            case BACKPACK_GEAR_LASER_GUN:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Laser Gun is already equipped. Face a threat and press Space to fire.",
                                              "激光枪已经装备。面向威胁后按空格键发射。"));
                return false;
            case BACKPACK_GEAR_PROTECTION_SUIT:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Protection Suit works passively while carried. There is no manual backpack action for it.",
                                              "防护服在携带时被动生效，不需要在背包里手动使用。"));
                return false;
            case BACKPACK_GEAR_FIELD_CAMP:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Field Camp is ready. Stand beside the placed camp and press F to rest there.",
                                              "野外营地已就绪。站在已部署营地旁按 F 即可休整。"));
                return false;
            case BACKPACK_GEAR_SIGNAL_AMPLIFIER:
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Signal Amplifier packed. Carry it to the tower route when you commit to the calmer ending.",
                                              "信号放大器已携带。选择更平稳的终局路线时，把它带到塔楼。"));
                return false;
            case BACKPACK_GEAR_GLOW_STICK:
            default:
                game->runtime.player.glowStickTimer = fmaxf(game->runtime.player.glowStickTimer, 60.0f);
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("Glow Stick readied. Its light will cover the next dark stretch.",
                                              "荧光棒已准备好。它的光会撑过接下来那段黑暗路程。"));
                return true;
        }
    }

    switch (entry->itemId) {
        case RESOURCE_RECOVERY_RATION:
            resource = (ResourceType)entry->itemId;
            break;
        default:
            if (message != NULL && messageSize > 0) {
                std::snprintf(message,
                              messageSize,
                              "%s",
                              Loc_PickLiteral("That raw material has to be processed at the workbench before it becomes usable.",
                                              "这种原始素材必须先在工作台加工，之后才能使用。"));
            }
            return false;
    }

    return Player_UseSelectedConsumable(&game->runtime.player, resource, message, (int)messageSize);
}

void GameOverlay_UpdateBackpack(Game *game, bool *open, KeyboardKey alternateKey) {
    Vector2 mouse;
    int itemIndex;

    if (game->ui.selectedBackpackItem < 0 || game->ui.selectedBackpackItem >= BACKPACK_ENTRY_COUNT) {
        game->ui.selectedBackpackItem = 0;
    }

    if (GameOverlay_TryCloseOverlay(game, open, alternateKey)) {
        return;
    }

    if (IsKeyPressed(KEY_F) || GameOverlay_IsConfirmPressed()) {
        char message[256];

        if (TryUseSelectedBackpackItem(game, message, sizeof(message))) {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
            Game_PostMessage(game, message, 2.8f);
        } else {
            if (message[0] != '\0') {
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_WARNING);
                Game_PostMessage(game, message, 2.6f);
            }
        }
        return;
    }

    if (!GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        return;
    }

    itemIndex = GameOverlay_FindClickedIndexedRect(mouse, BACKPACK_ENTRY_COUNT, UI_GetBackpackSlotRect);
    if (itemIndex >= 0) {
        game->ui.selectedBackpackItem = itemIndex;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CONFIRM);
        return;
    }
}
