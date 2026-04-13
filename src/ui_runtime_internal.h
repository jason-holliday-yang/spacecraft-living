#ifndef UI_RUNTIME_INTERNAL_H
#define UI_RUNTIME_INTERNAL_H

#include "ui_system.h"

/* Internal shared UI drawing/runtime primitives used by the UI slice files. */

SCL_EXTERN_C_BEGIN

float UIRuntime_GetScale(int screenWidth, int screenHeight);
void UIRuntime_DrawText(const AssetBundle *assets, const char *text, Vector2 position, float fontSize, Color tint);
Vector2 UIRuntime_MeasureText(const AssetBundle *assets, const char *text, float fontSize);
void UIRuntime_DrawTextureAssetFitted(const TextureAsset *asset, Rectangle rect, Color tint);
void UIRuntime_DrawWrappedText(const AssetBundle *assets, const char *text, Rectangle rect, float fontSize, float lineSpacing, Color tint);
int UIRuntime_CountWrappedTextLines(const AssetBundle *assets, const char *text, float fontSize, float maxWidth, int maxLines);
void UIRuntime_DrawPanel(Rectangle rect, Color fill, Color outline);
void UIRuntime_DrawBackdrop(int screenWidth, int screenHeight, float elapsedSeconds);
void UIRuntime_DrawButton(const AssetBundle *assets, Rectangle rect, const char *label, bool enabled);
bool UIRuntime_DrawHudStatusBar(const AssetBundle *assets,
                                const Player *player,
                                Rectangle rect,
                                int screenWidth,
                                int screenHeight,
                                PlayerStatusType *hoveredStatus);
void UIRuntime_DrawHudStatusTooltip(const AssetBundle *assets,
                                    const Player *player,
                                    PlayerStatusType status,
                                    Rectangle anchorRect,
                                    int screenWidth,
                                    int screenHeight);
void UIRuntime_DrawHudShortcut(const AssetBundle *assets,
                               Rectangle rect,
                               const char *label,
                               const char *subtitle,
                               const char *keyLabel,
                               int iconKind,
                               Color primary,
                               Color secondary);

SCL_EXTERN_C_END

#endif
