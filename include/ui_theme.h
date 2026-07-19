#ifndef UI_THEME_H
#define UI_THEME_H

#include "c_compat.h"
#include "raylib.h"

SCL_EXTERN_C_BEGIN

typedef struct UITheme {
    Color abyss;
    Color backdropTop;
    Color backdropBottom;
    Color surface;
    Color surfaceRaised;
    Color surfaceHover;
    Color surfaceDisabled;
    Color border;
    Color borderSoft;
    Color signalBlue;
    Color signalCyan;
    Color iceWhite;
    Color textPrimary;
    Color textSecondary;
    Color textMuted;
    Color positive;
    Color archiveGold;
    Color warning;
    Color danger;
    Color disabled;
} UITheme;

const UITheme *UITheme_Get(void);

SCL_EXTERN_C_END

#endif
