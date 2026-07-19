#include "ui_system.h"
#include "task_presentation.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

void UI_DrawDeathPopup(const Player *player, bool hasSave, const AssetBundle *assets, int screenWidth, int screenHeight, int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle hintRect;
    char deathBuffer[128];
    const char *buttonLabels[DEATH_POPUP_BUTTON_COUNT] = {
        Loc_PickLiteral("Restart", "重新开始"),
        Loc_PickLiteral("Load", "读档"),
        Loc_PickLiteral("Main Menu", "主菜单")
    };
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = Rectangle{screenWidth * 0.5f - 350.0f * scale, screenHeight * 0.5f - 170.0f * scale, 700.0f * scale, 340.0f * scale};
    bodyRect = Rectangle{panel.x + 54.0f * scale, panel.y + 132.0f * scale, panel.width - 108.0f * scale, 28.0f * scale};
    hintRect = Rectangle{panel.x + 54.0f * scale, panel.y + 166.0f * scale, panel.width - 108.0f * scale, 44.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{10, 5, 5, 220});

    UIRuntime_DrawPanel(panel, Color{30, 10, 10, 245}, Color{255, 80, 80, 90});

    UIRuntime_DrawText(assets, Loc_PickLiteral("YOU DIED", "你已死亡"), Vector2{panel.x + panel.width * 0.5f - UIRuntime_MeasureText(assets, Loc_PickLiteral("YOU DIED", "你已死亡"), 38.0f * scale).x * 0.5f, panel.y + 35.0f * scale}, 38.0f * scale, Color{255, 80, 80, 255});

    std::snprintf(deathBuffer, sizeof(deathBuffer), "%s %d", Loc_PickLiteral("Deaths", "死亡次数"), player->deathCount);
    UIRuntime_DrawText(assets, deathBuffer, Vector2{panel.x + panel.width * 0.5f - UIRuntime_MeasureText(assets, deathBuffer, 22.0f * scale).x * 0.5f, panel.y + 95.0f * scale}, 22.0f * scale, Color{200, 200, 200, 255});

    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("Suit vital systems have failed. This run is over as soon as your health is depleted.",
                                              "防护服生命体征系统已经崩溃。生命值归零后，本轮会立刻结束。"),
                              bodyRect,
                              19.0f * scale,
                              22.0f * scale,
                              Color{180, 180, 180, 255});
    UIRuntime_DrawWrappedText(assets,
                              hasSave
                                ? Loc_PickLiteral("Restart begins a fresh run immediately from Loxi's room and skips the opening crash recap. Load lets you return to an existing save.",
                                                  "重新开始会直接从洛希房间重新开局，并跳过开场坠毁剧情。读档可以返回已有存档。")
                                : Loc_PickLiteral("Restart begins a fresh run immediately from Loxi's room and skips the opening crash recap. No save is available to load for this account.",
                                                  "重新开始会直接从洛希房间重新开局，并跳过开场坠毁剧情。当前账号没有可读取的存档。"),
                              hintRect,
                              16.0f * scale,
                              18.0f * scale,
                              Color{196, 184, 184, 255});

    DrawLine(panel.x + 40.0f * scale, panel.y + 212.0f * scale, panel.x + panel.width - 40.0f * scale, panel.y + 212.0f * scale, Color{255, 100, 100, 120});

    for (buttonIndex = 0; buttonIndex < DEATH_POPUP_BUTTON_COUNT; buttonIndex++) {
        Rectangle buttonRect;
        bool enabled;

        buttonRect = UI_GetDeathPopupButtonRect(screenWidth, screenHeight, buttonIndex);
        enabled = buttonIndex != DEATH_POPUP_BUTTON_LOAD || hasSave;
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], enabled);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}

void UI_DrawSettlementConfirmPopup(const AssetBundle *assets,
                                   const Player *player,
                                   const TaskSystem *tasks,
                                   int screenWidth,
                                   int screenHeight,
                                   int buttonCount,
                                   int selectedButton) {
    (void)player;
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle noteRect;
    const char *titleText;
    const char *bodyText;
    const char *noteText;
    const char *buttonLabels[SETTLEMENT_CONFIRM_BUTTON_COUNT] = {0};
    const int availableEndingCount = tasks != NULL ? Tasks_GetAvailableEndingCount(tasks) : 0;
    char singleEndingBody[320];
    char singleEndingNote[160];
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetSettlementConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 96.0f * scale, panel.width - 68.0f * scale, 62.0f * scale};
    noteRect = Rectangle{panel.x + 34.0f * scale, panel.y + 166.0f * scale, panel.width - 68.0f * scale, 50.0f * scale};
    if (availableEndingCount <= 1) {
        GameEnding onlyEnding = tasks != NULL ? Tasks_GetAvailableEndingAt(tasks, 0) : ENDING_NONE;
        const char *onlyEndingTitle = Tasks_GetEndingTitle(onlyEnding);

        titleText = Loc_PickLiteral("Confirm Ending", "确认结局");
        std::snprintf(singleEndingBody,
                      sizeof(singleEndingBody),
                      Loc_PickLiteral("Loxi's archive review now supports only one final answer: %s. Confirming here means you are no longer choosing in uncertainty; you are accepting the route the full record now makes possible. The run will still continue until you carry out its final in-world step.",
                                      "洛希完成档案复核后，目前只剩下一个被完整证据支撑的终局回答：%s。在这里确认，不再只是挑选路线，而是正式接受整套记录如今所指向的结论；但本轮仍不会立刻结束，你还需要在世界里把它最后一步真正做完。"),
                      onlyEndingTitle);
        std::snprintf(singleEndingNote,
                      sizeof(singleEndingNote),
                      Loc_PickLiteral("Final route under review: %s", "当前进入最终复核的路线：%s"),
                      onlyEndingTitle);
        bodyText = singleEndingBody;
        noteText = singleEndingNote;
        buttonLabels[0] = onlyEnding != ENDING_NONE ? onlyEndingTitle : Loc_PickLiteral("Confirm", "确认");
        buttonLabels[1] = Loc_PickLiteral("Cancel", "取消");
    } else {
        titleText = Loc_PickLiteral("Choose Final Ending", "选择最终结局");
        bodyText = Loc_PickLiteral("Loxi has assembled enough evidence for more than one honest ending. Choose the answer you are willing to put your name on here, then return to the world and carry that decision through before the run truly ends.",
                                   "洛希已经把证据拼到足以支撑不止一种诚实结局。请在这里选出你愿意亲自承担的回答，然后回到世界里把这个决定真正执行到底，本轮才会结束。");
        noteText = Loc_PickLiteral("Only routes supported by recovered logs, archived investigations, and any required world-state conditions are shown here.",
                                   "这里出现的每条路线，都已经被你亲自回收的日志、完成归档的调查线以及对应世界状态共同支撑。");
        for (buttonIndex = 0; buttonIndex < availableEndingCount && buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT - 1; buttonIndex++) {
            buttonLabels[buttonIndex] = Tasks_GetEndingTitle(Tasks_GetAvailableEndingAt(tasks, buttonIndex));
        }
        buttonLabels[availableEndingCount] = Loc_PickLiteral("Cancel", "取消");
    }

    for (buttonIndex = 0; buttonIndex < buttonCount && buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT; buttonIndex++) {
        if (buttonLabels[buttonIndex] == NULL) {
            buttonLabels[buttonIndex] = buttonIndex == buttonCount - 1
                ? Loc_PickLiteral("Cancel", "取消")
                : Loc_PickLiteral("Unavailable", "不可用");
        }
    }

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{6, 10, 18, 200});
    UIRuntime_DrawPanel(panel, Color{16, 22, 30, 245}, Color{255, 214, 154, 80});
    UIRuntime_DrawText(assets, titleText, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 32.0f * scale, WHITE);
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), 16.0f * scale).x - 30.0f * scale, panel.y + 34.0f * scale}, 16.0f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets, bodyText, bodyRect, 18.0f * scale, 22.0f * scale, Color{226, 235, 244, 255});
    UIRuntime_DrawWrappedText(assets, noteText, noteRect, 16.0f * scale, 19.0f * scale, Color{255, 214, 154, 255});

    for (buttonIndex = 0; buttonIndex < buttonCount && buttonIndex < SETTLEMENT_CONFIRM_BUTTON_COUNT; buttonIndex++) {
        Rectangle buttonRect;

        buttonRect = UI_GetSettlementConfirmButtonRect(screenWidth, screenHeight, buttonIndex, buttonCount);
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], true);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}

void UI_DrawEndingRouteConfirmPopup(const AssetBundle *assets,
                                    GameEnding pendingEnding,
                                    int screenWidth,
                                    int screenHeight,
                                    int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    const char *titleText;
    char bodyTextBuffer[320];
    const char *endingTitle;
    const char *buttonLabels[2];
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetEndingRouteConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 68.0f * scale, panel.width - 68.0f * scale, 64.0f * scale};
    endingTitle = Tasks_GetEndingTitle(pendingEnding);
    titleText = Loc_PickLiteral("Confirm Your Choice", "确认你的选择");
    std::snprintf(bodyTextBuffer,
                  sizeof(bodyTextBuffer),
                  Loc_PickLiteral("Are you sure you want to commit to %s? Once locked in, winding paths collapse into a single ending, and you cannot change it for the remainder of this run.",
                                  "你确定要锁定 %s 路线吗？一旦确认，所有支路将收敛为唯一的结局走向，本轮剩余时间里不可再更改。"),
                  endingTitle);
    buttonLabels[0] = Loc_PickLiteral("Lock In", "确定锁定");
    buttonLabels[1] = Loc_PickLiteral("Cancel", "取消");

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{6, 10, 18, 200});
    UIRuntime_DrawPanel(panel, Color{16, 22, 30, 245}, Color{255, 214, 154, 80});
    UIRuntime_DrawText(assets, titleText, Vector2{panel.x + 34.0f * scale, panel.y + 28.0f * scale}, 28.0f * scale, WHITE);
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC cancels", "按 ESC 取消"), 16.0f * scale).x - 30.0f * scale, panel.y + 32.0f * scale}, 16.0f * scale, Color{182, 199, 214, 255});
    UIRuntime_DrawWrappedText(assets, bodyTextBuffer, bodyRect, 17.0f * scale, 20.0f * scale, Color{226, 235, 244, 255});

    for (buttonIndex = 0; buttonIndex < 2; buttonIndex++) {
        Rectangle buttonRect;

        buttonRect = UI_GetEndingRouteConfirmButtonRect(screenWidth, screenHeight, buttonIndex, 2);
        UIRuntime_DrawButton(assets, buttonRect, buttonLabels[buttonIndex], true);
        if (buttonIndex == selectedButton) {
            DrawRectangleRoundedLinesEx(buttonRect, 0.16f, 8, 2.0f, Color{255, 214, 154, 220});
        }
    }
}

void UI_DrawAccountDeleteConfirmPopup(const AssetBundle *assets,
                                      const char *accountName,
                                      int screenWidth,
                                      int screenHeight,
                                      int selectedButton) {
    float scale;
    Rectangle panel;
    Rectangle bodyRect;
    Rectangle noteRect;
    char titleBuffer[96];
    const char *buttonLabels[ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT] = {
        Loc_PickLiteral("Keep Account", "保留账号"),
        Loc_PickLiteral("Delete Account", "删除账号")
    };
    int buttonIndex;

    scale = UIRuntime_GetScale(screenWidth, screenHeight);
    panel = UI_GetAccountDeleteConfirmPanelRect(screenWidth, screenHeight);
    bodyRect = Rectangle{panel.x + 34.0f * scale, panel.y + 98.0f * scale, panel.width - 68.0f * scale, 66.0f * scale};
    noteRect = Rectangle{panel.x + 34.0f * scale, panel.y + 172.0f * scale, panel.width - 68.0f * scale, 56.0f * scale};

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{7, 5, 9, 224});
    DrawCircleGradient((int)(panel.x + panel.width * 0.5f),
                       (int)(panel.y + panel.height * 0.45f),
                       panel.width * 0.62f,
                       Color{149, 38, 48, 66},
                       Color{20, 7, 12, 0});
    UIComponents_DrawFrame(panel, scale, UI_FRAME_DANGER, true);
    std::snprintf(titleBuffer, sizeof(titleBuffer), "%s %s?", Loc_PickLiteral("SEVER", "删除"), (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("this identity", "此身份"));
    UIComponents_DrawDiamond(Vector2{panel.x + 42.0f * scale, panel.y + 48.0f * scale},
                12.0f * scale,
                Color{116, 27, 36, 255},
                Color{255, 139, 105, 220});
    UIRuntime_DrawText(assets, titleBuffer, Vector2{panel.x + 66.0f * scale, panel.y + 28.0f * scale}, 31.0f * scale, Color{255, 218, 178, 255});
    UIRuntime_DrawText(assets, Loc_PickLiteral("ESC CANCELS", "按 ESC 取消"), Vector2{panel.x + panel.width - UIRuntime_MeasureText(assets, Loc_PickLiteral("ESC CANCELS", "按 ESC 取消"), 14.0f * scale).x - 30.0f * scale, panel.y + 36.0f * scale}, 14.0f * scale, Color{165, 188, 183, 255});
    DrawLineEx(Vector2{panel.x + 34.0f * scale, panel.y + 78.0f * scale},
               Vector2{panel.x + panel.width - 34.0f * scale, panel.y + 78.0f * scale},
               1.2f * scale,
               Color{224, 103, 81, 110});
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("Sever this field identity and every expedition archive bound to it.", "删除这个远征身份，以及与其绑定的全部存档。"),
                              bodyRect,
                              18.0f * scale,
                              22.0f * scale,
                              Color{238, 222, 205, 255});
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("The archive cannot reconstruct a severed identity.", "身份一旦删除，档案将无法复原。"),
                              noteRect,
                              16.0f * scale,
                              19.0f * scale,
                              Color{255, 164, 115, 255});

    for (buttonIndex = 0; buttonIndex < ACCOUNT_DELETE_CONFIRM_BUTTON_COUNT; ++buttonIndex) {
        Rectangle buttonRect;

        buttonRect = UI_GetAccountDeleteConfirmButtonRect(screenWidth, screenHeight, buttonIndex);
        UIComponents_DrawActionButton(assets,
                                      buttonRect,
                                      buttonLabels[buttonIndex],
                                      true,
                                      buttonIndex == selectedButton,
                                      buttonIndex == ACCOUNT_DELETE_CONFIRM_BUTTON_DELETE ? UI_ACTION_DANGER : UI_ACTION_SECONDARY,
                                      scale);
    }
}
