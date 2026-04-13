#ifndef UI_STORY_INTERNAL_H
#define UI_STORY_INTERNAL_H

#include "ui_system.h"

typedef struct IntroSlideDef {
    const char *eyebrow;
    const char *title;
    const char *body;
    Color accent;
    Color fallbackTop;
    Color fallbackBottom;
} IntroSlideDef;

const IntroSlideDef *UIStory_GetIntroSlideDef(int slideIndex);
const IntroSlideDef *UIStory_GetStorySceneDef(StoryScene scene);
const TextureAsset *UIStory_GetStorySceneTexture(const AssetBundle *assets, StoryScene scene);
void UIStory_DrawBackdropTexture(const TextureAsset *asset,
                                 const IntroSlideDef *slide,
                                 int slideIndex,
                                 int screenWidth,
                                 int screenHeight,
                                 float elapsed,
                                 float scale);
void UIStory_DrawNarrativePanel(const AssetBundle *assets,
                                const IntroSlideDef *slide,
                                Rectangle panel,
                                Rectangle bodyRect,
                                Vector2 eyebrowPosition,
                                Vector2 titlePosition,
                                const char *hint,
                                float scale,
                                int screenWidth,
                                int screenHeight);

#endif
