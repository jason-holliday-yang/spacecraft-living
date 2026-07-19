#ifndef UI_THEME_INTERNAL_H
#define UI_THEME_INTERNAL_H

#include "assets.h"
#include "ui_theme.h"

/* Typography styling that is shared by themed frontend drawing slices. */

void UITheme_DrawTitleText(const AssetBundle *assets,
                           const char *text,
                           Vector2 center,
                           float fontSize,
                           float scale,
                           Color fill);

#endif
