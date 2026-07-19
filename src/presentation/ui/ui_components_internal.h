#ifndef UI_COMPONENTS_INTERNAL_H
#define UI_COMPONENTS_INTERNAL_H

#include "ui_system.h"
#include "ui_theme.h"

typedef enum UIFrameStyle {
    UI_FRAME_STANDARD = 0,
    UI_FRAME_COMPACT,
    UI_FRAME_DANGER
} UIFrameStyle;

typedef enum UIActionStyle {
    UI_ACTION_PRIMARY = 0,
    UI_ACTION_SECONDARY,
    UI_ACTION_WARNING,
    UI_ACTION_DANGER,
    UI_ACTION_GHOST
} UIActionStyle;

void UIComponents_DrawFrontEndButton(const AssetBundle *assets,
                                     Rectangle rect,
                                     const char *label,
                                     bool enabled,
                                     bool keyboardSelected,
                                     bool primary,
                                     float scale);
void UIComponents_DrawSettingsSliderRow(const AssetBundle *assets,
                                        const GameSettings *settings,
                                        int sliderIndex,
                                        const char *title,
                                        const char *description,
                                        int screenWidth,
                                        int screenHeight);
void UIComponents_DrawDiamond(Vector2 center, float radius, Color fill, Color outline);
void UIComponents_DrawScrim(int screenWidth, int screenHeight, unsigned char alpha);
void UIComponents_DrawFrame(Rectangle rect, float scale, UIFrameStyle style, bool strongShadow);
void UIComponents_DrawActionButton(const AssetBundle *assets,
                                   Rectangle rect,
                                   const char *label,
                                   bool enabled,
                                   bool keyboardSelected,
                                   UIActionStyle style,
                                   float scale);
void UIComponents_DrawCompactButton(const AssetBundle *assets,
                                    Rectangle rect,
                                    const char *label,
                                    bool enabled,
                                    bool keyboardSelected,
                                    UIActionStyle style,
                                    float scale);
void UIComponents_DrawCard(Rectangle rect, bool selected, bool enabled, float scale, Color accent);
void UIComponents_DrawBadge(const AssetBundle *assets,
                            Rectangle rect,
                            const char *label,
                            Color accent,
                            bool enabled,
                            float scale);
void UIComponents_DrawDivider(Rectangle rect, Color accent, float scale);
void UIComponents_DrawPanelHeader(const AssetBundle *assets,
                                  Rectangle panel,
                                  const char *eyebrow,
                                  const char *title,
                                  const char *hint,
                                  float scale,
                                  Color accent);

#endif
