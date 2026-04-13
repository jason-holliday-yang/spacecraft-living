#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include "c_compat.h"

/* Public UI-facing localized string constants and small text helpers. */

SCL_EXTERN_C_BEGIN

#define LOC_UI_HEALTH "Health"
#define LOC_UI_OXYGEN "Oxygen"
#define LOC_UI_STAGE "Stage"
#define LOC_UI_OBJECTIVE "Objective"
#define LOC_UI_DAY "Cycle"
#define LOC_UI_EVENT "Event"
#define LOC_UI_MENU_START "Start Game"
#define LOC_UI_MENU_LOAD "Load"
#define LOC_UI_MENU_SETTINGS "Settings"
#define LOC_UI_MENU_EXIT "Exit"
#define LOC_UI_PAUSE_TITLE "Paused"
#define LOC_UI_PAUSE_CONTINUE "Resume"
#define LOC_UI_PAUSE_SAVE "Save"
#define LOC_UI_PAUSE_LOAD "Load"
#define LOC_UI_PAUSE_SETTINGS "Settings"
#define LOC_UI_PAUSE_MENU "Main Menu"
#define LOC_UI_COMM_TITLE "Portable Communicator"
#define LOC_UI_CRAFT_TITLE "Crafting"
#define LOC_UI_HELP_TITLE "Controls"
#define LOC_UI_HELP_HINT "Press the matching key to close"
#define LOC_UI_SETTINGS_TITLE "Settings"
#define LOC_UI_RECIPE_LOCKED "Locked"
#define LOC_UI_SAVE_SUCCESS "Progress saved"
#define LOC_UI_SAVE_FAILED "Save failed"
#define LOC_UI_SAVE_MISSING "No save file found"
#define LOC_UI_SAVE_DELETED "Save deleted"
#define LOC_UI_SAVE_DELETE_FAILED "Delete failed"
#define LOC_UI_END_EXIT "Press ESC to close"

const char *Loc_GetUIFontSampleText(void);

SCL_EXTERN_C_END

#endif
