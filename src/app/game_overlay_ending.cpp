#include "game_overlay_frontend_internal.h"
#include "task_presentation.h"
#include "game_session_internal.h"

#include "localization.h"
#include "ui_runtime_internal.h"

#include <cmath>
#include <cstdio>
#include <cstring>

static int GetEndingChoicePopupButtonCount(const Game *game) {
    const int availableEndingCount = game != NULL ? Tasks_GetAvailableEndingCount(&game->runtime.tasks) : 0;

    return availableEndingCount > 1 ? availableEndingCount + 1 : 2;
}

static AudioCue GetRouteConfirmCue(GameEnding ending) {
    switch (ending) {
        case ENDING_PEACEFUL:
            return AUDIO_CUE_ENDING_PEACEFUL;
        case ENDING_SETTLEMENT:
            return AUDIO_CUE_ENDING_SETTLEMENT;
        case ENDING_HEROIC:
            return AUDIO_CUE_ENDING;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            return AUDIO_CUE_WARNING;
    }
}

static void ConfirmEndingChoice(Game *game, GameEnding ending) {
    char message[256];

    if (game == NULL) {
        return;
    }

    if (!Tasks_SelectEndingRoute(&game->runtime.tasks, ending)) {
        return;
    }

    Tasks_UpdateObjective(&game->runtime.tasks, &game->runtime.player);
    game->ui.settlementConfirmOpen = false;
    game->ui.settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
    switch (ending) {
        case ENDING_HEROIC:
            std::snprintf(message,
                          sizeof(message),
                          "%s",
                          Loc_PickLiteral("Heroic route locked. Hunt the guardian in the northwest ruins, then finish at the Signal Tower.",
                                          "强行救援路线已锁定。先去西北遗迹猎杀守卫，再到信号塔完成最后一步。"));
            break;
        case ENDING_PEACEFUL:
            std::snprintf(message,
                          sizeof(message),
                          "%s",
                          Loc_PickLiteral("Peaceful route locked. Prepare the Signal Amplifier, then carry it to the Signal Tower.",
                                          "和平救援路线已锁定。先准备信号放大器，再把它带到信号塔。"));
            break;
        case ENDING_SETTLEMENT:
            std::snprintf(message,
                          sizeof(message),
                          "%s",
                          Loc_PickLiteral("Settlement route locked. Return to Loxi once you are ready to confirm staying here for good.",
                                          "定居路线已锁定。等你准备好真正留下来时，再回到洛希那里确认。"));
            break;
        case ENDING_FAILURE:
        case ENDING_NONE:
        default:
            message[0] = '\0';
            break;
    }
    if (message[0] != '\0') {
        Game_PostMessage(game, message, 4.2f);
    }
    Audio_PlayCue(&game->services.audio, GetRouteConfirmCue(ending));
}

void GameOverlay_UpdateSettlementConfirm(Game *game) {
    Vector2 mouse;
    int buttonIndex;
    int buttonCount;
    int availableEndingCount;
    bool activateSelection;

    activateSelection = false;
    availableEndingCount = game != NULL ? Tasks_GetAvailableEndingCount(&game->runtime.tasks) : 0;
    buttonCount = GetEndingChoicePopupButtonCount(game);

    if (availableEndingCount <= 0) {
        game->ui.settlementConfirmOpen = false;
        game->ui.settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->ui.settlementConfirmOpen = false;
        game->ui.settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        game->ui.settlementConfirmSelection = (game->ui.settlementConfirmSelection + buttonCount - 1) % buttonCount;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
        game->ui.settlementConfirmSelection = (game->ui.settlementConfirmSelection + 1) % buttonCount;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = -1;
        for (int index = 0; index < buttonCount; index++) {
            if (CheckCollisionPointRec(mouse,
                                       UI_GetSettlementConfirmButtonRect(GetScreenWidth(),
                                                                         GetScreenHeight(),
                                                                         index,
                                                                         buttonCount))) {
                buttonIndex = index;
                break;
            }
        }
        if (buttonIndex >= 0) {
            game->ui.settlementConfirmSelection = buttonIndex;
            activateSelection = true;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        activateSelection = true;
    }

    if (!activateSelection) {
        return;
    }

    if (game->ui.settlementConfirmSelection >= availableEndingCount) {
        game->ui.settlementConfirmOpen = false;
        game->ui.settlementConfirmSelection = SETTLEMENT_CONFIRM_BUTTON_HEROIC;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    game->ui.endingRoutePendingConfirm = Tasks_GetAvailableEndingAt(&game->runtime.tasks, game->ui.settlementConfirmSelection);
    game->ui.endingRouteDoubleConfirmOpen = true;
    game->ui.endingRouteDoubleConfirmSelection = 0;
    game->ui.settlementConfirmOpen = false;
    Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
}

void GameOverlay_UpdateEndingRouteDoubleConfirm(Game *game) {
    Vector2 mouse;
    int buttonIndex;

    if (game == NULL) {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->ui.endingRouteDoubleConfirmOpen = false;
        game->ui.settlementConfirmOpen = true;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        return;
    }

    if (GameOverlay_IsBackwardNavigationPressed()) {
        game->ui.endingRouteDoubleConfirmSelection = (game->ui.endingRouteDoubleConfirmSelection + 2 - 1) % 2;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_IsForwardNavigationPressed()) {
        game->ui.endingRouteDoubleConfirmSelection = (game->ui.endingRouteDoubleConfirmSelection + 1) % 2;
        Audio_PlayCue(&game->services.audio, AUDIO_CUE_OPEN);
    }

    if (GameOverlay_TryGetPrimaryClickPosition(&mouse)) {
        buttonIndex = -1;
        for (int index = 0; index < 2; index++) {
            if (CheckCollisionPointRec(mouse,
                                       UI_GetEndingRouteConfirmButtonRect(GetScreenWidth(),
                                                                          GetScreenHeight(),
                                                                          index,
                                                                          2))) {
                buttonIndex = index;
                break;
            }
        }
        if (buttonIndex >= 0) {
            game->ui.endingRouteDoubleConfirmSelection = buttonIndex;
            if (buttonIndex == 0) {
                ConfirmEndingChoice(game, game->ui.endingRoutePendingConfirm);
                game->ui.endingRouteDoubleConfirmOpen = false;
            } else {
                game->ui.endingRouteDoubleConfirmOpen = false;
                game->ui.settlementConfirmOpen = true;
                Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
            }
            return;
        }
    }

    if (GameOverlay_IsConfirmPressed()) {
        if (game->ui.endingRouteDoubleConfirmSelection == 0) {
            ConfirmEndingChoice(game, game->ui.endingRoutePendingConfirm);
        } else {
            Audio_PlayCue(&game->services.audio, AUDIO_CUE_CLOSE);
        }
        game->ui.endingRouteDoubleConfirmOpen = false;
    }
}

void GameOverlay_UpdateEndingMenu(Game *game) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        game->ui.endingMenuSelection -= 1;
        if (game->ui.endingMenuSelection < 0) {
            game->ui.endingMenuSelection = 3;
        }
        return;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game->ui.endingMenuSelection = (game->ui.endingMenuSelection + 1) % 4;
        return;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
        switch (game->ui.endingMenuSelection) {
            case 0:
                Game_BeginScreenTransition(game, SCREEN_TRANSITION_RETURN_TO_MENU, -1);
                break;
            case 1:
                Game_ReturnToMenu(game);
                break;
            case 2:
                if (game->account.hasSaveFile) {
                    Game_OpenSavePanel(game, SAVE_PANEL_MODE_LOAD);
                    game->flow.state = GAME_STATE_INTRO;
                }
                break;
            case 3:
                game->flow.requestClose = true;
                break;
            default:
                break;
        }
    }
}
