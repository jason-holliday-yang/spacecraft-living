#include "ui_system.h"

#include "localization.h"
#include "ui_runtime_internal.h"
#include "ui_components_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

void UI_DrawSettingsOverlay(const AssetBundle *assets,
                            const GameSettings *settings,
                            const char *accountName,
                            int saveCount,
                            bool accountActionsEnabled,
                            int screenWidth,
                            int screenHeight) {
    const UITheme *theme = UITheme_Get();
    const float scale = UIRuntime_GetScale(screenWidth, screenHeight);
    const Rectangle panel = UI_GetStandardOverlayRect(screenWidth, screenHeight);
    const Rectangle closeRect = UI_GetSettingsCloseButtonRect(screenWidth, screenHeight);
    const Rectangle languageRowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, 3);
    const Rectangle accountRowRect = UI_GetSettingsRowRect(screenWidth, screenHeight, 4);
    const Rectangle languageEnglishRect = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 0);
    const Rectangle languageChineseRect = UI_GetSettingsLanguageButtonRect(screenWidth, screenHeight, 1);
    const Rectangle accountSwitchRect = UI_GetSettingsAccountButtonRect(screenWidth, screenHeight, 0);
    const Rectangle accountDeleteRect = UI_GetSettingsAccountButtonRect(screenWidth, screenHeight, 1);
    const bool englishSelected = Loc_NormalizeLanguage((int)settings->language) == GAME_LANGUAGE_EN;
    char accountBuffer[128];
    const char *accountHint = accountActionsEnabled
        ? Loc_PickLiteral("Account controls are available in this terminal session.", "当前终端会话允许管理本地账号。")
        : Loc_PickLiteral("Return to the main menu settings to manage the active account.", "请返回主菜单设置管理当前账号。 ");

    std::snprintf(accountBuffer,
                  sizeof(accountBuffer),
                  "%s  //  %s %d",
                  (accountName != NULL && accountName[0] != '\0') ? accountName : Loc_PickLiteral("Unknown", "未知"),
                  Loc_PickLiteral("Archives", "存档"),
                  saveCount);

    UIComponents_DrawScrim(screenWidth, screenHeight, 210);
    UIComponents_DrawFrame(panel, scale, UI_FRAME_STANDARD, true);
    UIComponents_DrawPanelHeader(assets,
                                 panel,
                                 Loc_PickLiteral("LOXI SYSTEM TERMINAL // CONFIGURATION", "洛希系统终端 // 配置"),
                                 LOC_UI_SETTINGS_TITLE,
                                 "",
                                 scale,
                                 theme->signalCyan);
    UIComponents_DrawCompactButton(assets, closeRect, LOC_UI_CLOSE, true, false, UI_ACTION_GHOST, scale);
    UIRuntime_DrawText(assets,
                       LOC_UI_PRESS_ESC_CLOSE,
                       Vector2{closeRect.x - UIRuntime_MeasureText(assets, LOC_UI_PRESS_ESC_CLOSE, 13.5f * scale).x - 16.0f * scale,
                               closeRect.y + closeRect.height * 0.5f - UIRuntime_MeasureText(assets, LOC_UI_PRESS_ESC_CLOSE, 13.5f * scale).y * 0.5f},
                       13.5f * scale,
                       theme->textSecondary);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Calibrate audio, language, and local archive access.", "校准音频、语言和本地档案访问设置。"),
                       Vector2{panel.x + 34.0f * scale, panel.y + 89.0f * scale},
                       14.5f * scale,
                       theme->textSecondary);

    UIComponents_DrawSettingsSliderRow(assets,
                          settings,
                          0,
                          Loc_PickLiteral("Master Volume", "总音量"),
                          Loc_PickLiteral("Controls the complete expedition mix.", "控制远征中的完整声音混合。"),
                          screenWidth,
                          screenHeight);
    UIComponents_DrawSettingsSliderRow(assets,
                          settings,
                          1,
                          Loc_PickLiteral("Music Volume", "音乐音量"),
                          Loc_PickLiteral("Adjusts the ambient and narrative score.", "调整环境音乐和叙事配乐。"),
                          screenWidth,
                          screenHeight);
    UIComponents_DrawSettingsSliderRow(assets,
                          settings,
                          2,
                          Loc_PickLiteral("SFX Volume", "音效音量"),
                          Loc_PickLiteral("Adjusts UI, combat, and environmental signals.", "调整界面、战斗和环境信号音效。"),
                          screenWidth,
                          screenHeight);

    UIComponents_DrawCard(languageRowRect, false, true, scale, theme->signalBlue);
    UIRuntime_DrawText(assets,
                       LOC_UI_LANGUAGE,
                       Vector2{languageRowRect.x + 22.0f * scale, languageRowRect.y + 13.0f * scale},
                       21.0f * scale,
                       theme->iceWhite);
    UIRuntime_DrawWrappedText(assets,
                              Loc_PickLiteral("Select the archive display language.", "选择档案终端的显示语言。"),
                              Rectangle{languageRowRect.x + 22.0f * scale, languageRowRect.y + 40.0f * scale, 230.0f * scale, 20.0f * scale},
                              12.0f * scale,
                              13.0f * scale,
                              theme->textSecondary);
    UIComponents_DrawCompactButton(assets,
                                   languageEnglishRect,
                                   Loc_GetLanguageNativeName(GAME_LANGUAGE_EN),
                                   true,
                                   englishSelected,
                                   UI_ACTION_SECONDARY,
                                   scale);
    UIComponents_DrawCompactButton(assets,
                                   languageChineseRect,
                                   "简体中文",
                                   true,
                                   !englishSelected,
                                   UI_ACTION_SECONDARY,
                                   scale);

    UIComponents_DrawCard(accountRowRect, false, true, scale, accountActionsEnabled ? theme->positive : theme->disabled);
    UIRuntime_DrawText(assets,
                       Loc_PickLiteral("Local Identity", "本地身份"),
                       Vector2{accountRowRect.x + 22.0f * scale, accountRowRect.y + 13.0f * scale},
                       21.0f * scale,
                       theme->iceWhite);
    UIRuntime_DrawText(assets,
                       accountBuffer,
                       Vector2{accountRowRect.x + 22.0f * scale, accountRowRect.y + 41.0f * scale},
                       13.0f * scale,
                       accountActionsEnabled ? theme->positive : theme->textMuted);
    UIComponents_DrawCompactButton(assets,
                                   accountSwitchRect,
                                   Loc_PickLiteral("Switch Identity", "切换身份"),
                                   accountActionsEnabled,
                                   false,
                                   UI_ACTION_SECONDARY,
                                   scale);
    UIComponents_DrawCompactButton(assets,
                                   accountDeleteRect,
                                   Loc_PickLiteral("Delete Identity", "删除身份"),
                                   accountActionsEnabled,
                                   false,
                                   UI_ACTION_DANGER,
                                   scale);
    UIRuntime_DrawWrappedText(assets,
                              accountHint,
                              Rectangle{accountRowRect.x + 248.0f * scale,
                                        accountRowRect.y + 51.0f * scale,
                                        accountRowRect.width - 618.0f * scale,
                                        16.0f * scale},
                              11.5f * scale,
                              13.0f * scale,
                              accountActionsEnabled ? theme->textSecondary : theme->textMuted);
}
