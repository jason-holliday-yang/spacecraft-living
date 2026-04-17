#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include "c_compat.h"

/* Runtime language selection plus shared localized UI text helpers. */

SCL_EXTERN_C_BEGIN

typedef enum GameLanguage {
    GAME_LANGUAGE_EN = 0,
    GAME_LANGUAGE_ZH_CN,
    GAME_LANGUAGE_COUNT
} GameLanguage;

typedef struct LocalizedText {
    const char *english;
    const char *simplifiedChinese;
} LocalizedText;

typedef enum LocTextId {
    LOC_TEXT_UI_HEALTH = 0,
    LOC_TEXT_UI_OXYGEN,
    LOC_TEXT_UI_STAGE,
    LOC_TEXT_UI_OBJECTIVE,
    LOC_TEXT_UI_DAY,
    LOC_TEXT_UI_EVENT,
    LOC_TEXT_UI_MENU_START,
    LOC_TEXT_UI_MENU_LOAD,
    LOC_TEXT_UI_MENU_SETTINGS,
    LOC_TEXT_UI_MENU_EXIT,
    LOC_TEXT_UI_PAUSE_TITLE,
    LOC_TEXT_UI_PAUSE_CONTINUE,
    LOC_TEXT_UI_PAUSE_SAVE,
    LOC_TEXT_UI_PAUSE_LOAD,
    LOC_TEXT_UI_PAUSE_SETTINGS,
    LOC_TEXT_UI_PAUSE_MENU,
    LOC_TEXT_UI_COMM_TITLE,
    LOC_TEXT_UI_CRAFT_TITLE,
    LOC_TEXT_UI_HELP_TITLE,
    LOC_TEXT_UI_HELP_HINT,
    LOC_TEXT_UI_SETTINGS_TITLE,
    LOC_TEXT_UI_RECIPE_LOCKED,
    LOC_TEXT_UI_SAVE_SUCCESS,
    LOC_TEXT_UI_SAVE_FAILED,
    LOC_TEXT_UI_SAVE_MISSING,
    LOC_TEXT_UI_SAVE_DELETED,
    LOC_TEXT_UI_SAVE_DELETE_FAILED,
    LOC_TEXT_UI_END_EXIT,
    LOC_TEXT_UI_CLOSE,
    LOC_TEXT_UI_PRESS_ESC_CLOSE,
    LOC_TEXT_UI_PRESS_ESC_RETURN,
    LOC_TEXT_UI_LANGUAGE,
    LOC_TEXT_COUNT
} LocTextId;

GameLanguage Loc_GetLanguage(void);
void Loc_SetLanguage(GameLanguage language);
GameLanguage Loc_NormalizeLanguage(int languageValue);
const char *Loc_GetText(LocTextId id);
const char *Loc_PickLiteral(const char *english, const char *simplifiedChinese);
const char *Loc_PickText(LocalizedText text);
const char *Loc_Translate(const char *english);
const char *Loc_GetLanguageNativeName(GameLanguage language);
const char *Loc_GetAreaNameText(const char *canonicalEnglish);
const char *Loc_GetLocationNameText(const char *canonicalEnglish);

#define LOC_UI_HEALTH Loc_GetText(LOC_TEXT_UI_HEALTH)
#define LOC_UI_OXYGEN Loc_GetText(LOC_TEXT_UI_OXYGEN)
#define LOC_UI_STAGE Loc_GetText(LOC_TEXT_UI_STAGE)
#define LOC_UI_OBJECTIVE Loc_GetText(LOC_TEXT_UI_OBJECTIVE)
#define LOC_UI_DAY Loc_GetText(LOC_TEXT_UI_DAY)
#define LOC_UI_EVENT Loc_GetText(LOC_TEXT_UI_EVENT)
#define LOC_UI_MENU_START Loc_GetText(LOC_TEXT_UI_MENU_START)
#define LOC_UI_MENU_LOAD Loc_GetText(LOC_TEXT_UI_MENU_LOAD)
#define LOC_UI_MENU_SETTINGS Loc_GetText(LOC_TEXT_UI_MENU_SETTINGS)
#define LOC_UI_MENU_EXIT Loc_GetText(LOC_TEXT_UI_MENU_EXIT)
#define LOC_UI_PAUSE_TITLE Loc_GetText(LOC_TEXT_UI_PAUSE_TITLE)
#define LOC_UI_PAUSE_CONTINUE Loc_GetText(LOC_TEXT_UI_PAUSE_CONTINUE)
#define LOC_UI_PAUSE_SAVE Loc_GetText(LOC_TEXT_UI_PAUSE_SAVE)
#define LOC_UI_PAUSE_LOAD Loc_GetText(LOC_TEXT_UI_PAUSE_LOAD)
#define LOC_UI_PAUSE_SETTINGS Loc_GetText(LOC_TEXT_UI_PAUSE_SETTINGS)
#define LOC_UI_PAUSE_MENU Loc_GetText(LOC_TEXT_UI_PAUSE_MENU)
#define LOC_UI_COMM_TITLE Loc_GetText(LOC_TEXT_UI_COMM_TITLE)
#define LOC_UI_CRAFT_TITLE Loc_GetText(LOC_TEXT_UI_CRAFT_TITLE)
#define LOC_UI_HELP_TITLE Loc_GetText(LOC_TEXT_UI_HELP_TITLE)
#define LOC_UI_HELP_HINT Loc_GetText(LOC_TEXT_UI_HELP_HINT)
#define LOC_UI_SETTINGS_TITLE Loc_GetText(LOC_TEXT_UI_SETTINGS_TITLE)
#define LOC_UI_RECIPE_LOCKED Loc_GetText(LOC_TEXT_UI_RECIPE_LOCKED)
#define LOC_UI_SAVE_SUCCESS Loc_GetText(LOC_TEXT_UI_SAVE_SUCCESS)
#define LOC_UI_SAVE_FAILED Loc_GetText(LOC_TEXT_UI_SAVE_FAILED)
#define LOC_UI_SAVE_MISSING Loc_GetText(LOC_TEXT_UI_SAVE_MISSING)
#define LOC_UI_SAVE_DELETED Loc_GetText(LOC_TEXT_UI_SAVE_DELETED)
#define LOC_UI_SAVE_DELETE_FAILED Loc_GetText(LOC_TEXT_UI_SAVE_DELETE_FAILED)
#define LOC_UI_END_EXIT Loc_GetText(LOC_TEXT_UI_END_EXIT)
#define LOC_UI_CLOSE Loc_GetText(LOC_TEXT_UI_CLOSE)
#define LOC_UI_PRESS_ESC_CLOSE Loc_GetText(LOC_TEXT_UI_PRESS_ESC_CLOSE)
#define LOC_UI_PRESS_ESC_RETURN Loc_GetText(LOC_TEXT_UI_PRESS_ESC_RETURN)
#define LOC_UI_LANGUAGE Loc_GetText(LOC_TEXT_UI_LANGUAGE)

SCL_EXTERN_C_END

#endif
