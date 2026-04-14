#include "assets_internal.h"

#include "config.h"
#include "localization.h"
#include "resource_path.h"

#include <cmath>
#include <cstring>

static int MinInt(int a, int b) {
    return a < b ? a : b;
}

static int MaxInt(int a, int b) {
    return a > b ? a : b;
}

Font AssetsInternal_LoadUIFont(void) {
    const char *fontPaths[] = {
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Hiragino Sans GB.ttc",
        "/System/Library/Fonts/STHeiti Light.ttc",
        "/System/Library/Fonts/STHeiti Medium.ttc",
        "/System/Library/Fonts/HelveticaNeue.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Avenir Next.ttc",
        "/System/Library/Fonts/SFNS.ttf"
    };
    const int fontBaseSize = 42;
    int codepointCount;
    int *codepoints;
    int pathIndex;

    codepointCount = 0;
    codepoints = LoadCodepoints(Loc_GetUIFontSampleText(), &codepointCount);

    for (pathIndex = 0; pathIndex < (int)(sizeof(fontPaths) / sizeof(fontPaths[0])); pathIndex++) {
        if (FileExists(fontPaths[pathIndex])) {
            Font font;

            font = LoadFontEx(fontPaths[pathIndex], fontBaseSize, codepoints, codepointCount);
            if (font.texture.id != 0) {
                SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
                if (codepoints != NULL) {
                    UnloadCodepoints(codepoints);
                }
                return font;
            }
        }
    }

    if (codepoints != NULL) {
        UnloadCodepoints(codepoints);
    }
    return Font{};
}

static TextureAsset LoadOptionalTextureFittedInternal(const char *path,
                                                      int maxWidth,
                                                      int maxHeight,
                                                      bool trimTransparentFrame) {
    TextureAsset asset;
    char resolvedPath[2048];

    std::memset(&asset, 0, sizeof(asset));
    if (ResourcePath_Resolve(path, resolvedPath, sizeof(resolvedPath))) {
        Image image;

        image = LoadImage(resolvedPath);
        if (image.data != NULL) {
            if (trimTransparentFrame) {
                ImageAlphaCrop(&image, 0.02f);
            }
            if (maxWidth > 0 && maxHeight > 0 && (image.width > maxWidth || image.height > maxHeight)) {
                float scale;
                int targetWidth;
                int targetHeight;

                scale = fminf((float)maxWidth / (float)image.width, (float)maxHeight / (float)image.height);
                targetWidth = (int)((float)image.width * scale + 0.5f);
                targetHeight = (int)((float)image.height * scale + 0.5f);
                if (targetWidth < 1) {
                    targetWidth = 1;
                }
                if (targetHeight < 1) {
                    targetHeight = 1;
                }
                ImageResize(&image, targetWidth, targetHeight);
            }

            asset.texture = LoadTextureFromImage(image);
            asset.loaded = asset.texture.id != 0;
            UnloadImage(image);
        }
    }

    if (asset.loaded) {
        SetTextureFilter(asset.texture, TEXTURE_FILTER_BILINEAR);
    }

    return asset;
}

TextureAsset AssetsInternal_LoadOptionalTextureFitted(const char *path, int maxWidth, int maxHeight) {
    return LoadOptionalTextureFittedInternal(path, maxWidth, maxHeight, true);
}

TextureAsset AssetsInternal_LoadOptionalTextureFittedPreserveFrame(const char *path, int maxWidth, int maxHeight) {
    return LoadOptionalTextureFittedInternal(path, maxWidth, maxHeight, false);
}

static void InitializeDefaultSpriteFrames(Rectangle *frames, int textureWidth, int textureHeight) {
    int rowIndex;
    int columnIndex;
    float frameWidth;
    float frameHeight;

    frameWidth = (float)textureWidth / (float)PLAYER_SPRITE_SHEET_COLS;
    frameHeight = (float)textureHeight / (float)PLAYER_SPRITE_SHEET_ROWS;
    for (rowIndex = 0; rowIndex < PLAYER_SPRITE_SHEET_ROWS; rowIndex++) {
        for (columnIndex = 0; columnIndex < PLAYER_SPRITE_SHEET_COLS; columnIndex++) {
            frames[rowIndex * PLAYER_SPRITE_SHEET_COLS + columnIndex] = Rectangle{
                frameWidth * (float)columnIndex,
                frameHeight * (float)rowIndex,
                frameWidth,
                frameHeight
            };
        }
    }
}

static void NormalizeSpriteSheetFrames(const Image *image, Rectangle *frames) {
    int rowIndex;
    float baseCellWidth;
    float baseCellHeight;

    baseCellWidth = (float)image->width / (float)PLAYER_SPRITE_SHEET_COLS;
    baseCellHeight = (float)image->height / (float)PLAYER_SPRITE_SHEET_ROWS;

    for (rowIndex = 0; rowIndex < PLAYER_SPRITE_SHEET_ROWS; rowIndex++) {
        int columnIndex;
        float targetWidth;
        float targetHeight;

        targetWidth = 0.0f;
        targetHeight = 0.0f;
        for (columnIndex = 0; columnIndex < PLAYER_SPRITE_SHEET_COLS; columnIndex++) {
            Rectangle frame;

            frame = frames[rowIndex * PLAYER_SPRITE_SHEET_COLS + columnIndex];
            if (frame.width > targetWidth) {
                targetWidth = frame.width;
            }
            if (frame.height > targetHeight) {
                targetHeight = frame.height;
            }
        }

        targetWidth = fminf(targetWidth + 8.0f, baseCellWidth);
        targetHeight = fminf(targetHeight + 8.0f, baseCellHeight);

        for (columnIndex = 0; columnIndex < PLAYER_SPRITE_SHEET_COLS; columnIndex++) {
            float cellLeft;
            float cellTop;
            float cellWidth;
            float cellHeight;
            float left;
            float top;

            cellLeft = baseCellWidth * (float)columnIndex;
            cellTop = baseCellHeight * (float)rowIndex;
            cellWidth = baseCellWidth;
            cellHeight = baseCellHeight;
            if (columnIndex == PLAYER_SPRITE_SHEET_COLS - 1) {
                cellWidth = (float)image->width - cellLeft;
            }
            if (rowIndex == PLAYER_SPRITE_SHEET_ROWS - 1) {
                cellHeight = (float)image->height - cellTop;
            }

            left = cellLeft + (cellWidth - targetWidth) * 0.5f;
            top = cellTop + (cellHeight - targetHeight) * 0.5f;
            if (left < cellLeft) {
                left = cellLeft;
            }
            if (top < cellTop) {
                top = cellTop;
            }
            if (left + targetWidth > cellLeft + cellWidth) {
                left = cellLeft + cellWidth - targetWidth;
            }
            if (top + targetHeight > cellTop + cellHeight) {
                top = cellTop + cellHeight - targetHeight;
            }

            frames[rowIndex * PLAYER_SPRITE_SHEET_COLS + columnIndex] = Rectangle{
                left,
                top,
                targetWidth,
                targetHeight
            };
        }
    }
}

static void AnalyzeSpriteSheetFrames(const Image *image, Rectangle *frames) {
    int rowIndex;
    int columnIndex;
    float frameWidth;
    float frameHeight;

    frameWidth = (float)image->width / (float)PLAYER_SPRITE_SHEET_COLS;
    frameHeight = (float)image->height / (float)PLAYER_SPRITE_SHEET_ROWS;

    for (rowIndex = 0; rowIndex < PLAYER_SPRITE_SHEET_ROWS; rowIndex++) {
        for (columnIndex = 0; columnIndex < PLAYER_SPRITE_SHEET_COLS; columnIndex++) {
            int left;
            int top;
            int right;
            int bottom;
            int x;
            int y;
            int cellLeft;
            int cellTop;
            int cellRight;
            int cellBottom;

            cellLeft = (int)(frameWidth * (float)columnIndex);
            cellTop = (int)(frameHeight * (float)rowIndex);
            cellRight = (int)(frameWidth * (float)(columnIndex + 1)) - 1;
            cellBottom = (int)(frameHeight * (float)(rowIndex + 1)) - 1;
            if (columnIndex == PLAYER_SPRITE_SHEET_COLS - 1) {
                cellRight = image->width - 1;
            }
            if (rowIndex == PLAYER_SPRITE_SHEET_ROWS - 1) {
                cellBottom = image->height - 1;
            }

            left = cellRight;
            top = cellBottom;
            right = cellLeft;
            bottom = cellTop;

            for (y = cellTop; y <= cellBottom; y++) {
                for (x = cellLeft; x <= cellRight; x++) {
                    Color pixel;

                    pixel = GetImageColor(*image, x, y);
                    if (pixel.a <= 12) {
                        continue;
                    }

                    if (x < left) {
                        left = x;
                    }
                    if (y < top) {
                        top = y;
                    }
                    if (x > right) {
                        right = x;
                    }
                    if (y > bottom) {
                        bottom = y;
                    }
                }
            }

            if (right < left || bottom < top) {
                frames[rowIndex * PLAYER_SPRITE_SHEET_COLS + columnIndex] = Rectangle{
                    (float)cellLeft,
                    (float)cellTop,
                    (float)(cellRight - cellLeft + 1),
                    (float)(cellBottom - cellTop + 1)
                };
                continue;
            }

            left = MaxInt(cellLeft, left - 4);
            top = MaxInt(cellTop, top - 4);
            right = MinInt(cellRight, right + 4);
            bottom = MinInt(cellBottom, bottom + 4);
            frames[rowIndex * PLAYER_SPRITE_SHEET_COLS + columnIndex] = Rectangle{
                (float)left,
                (float)top,
                (float)(right - left + 1),
                (float)(bottom - top + 1)
            };
        }
    }

    NormalizeSpriteSheetFrames(image, frames);
}

TextureAsset AssetsInternal_LoadOptionalTextureSheetFitted(const char *path, Rectangle *frames) {
    TextureAsset asset;
    char resolvedPath[2048];

    std::memset(&asset, 0, sizeof(asset));
    InitializeDefaultSpriteFrames(frames, TILE_SIZE * PLAYER_SPRITE_SHEET_COLS, TILE_SIZE * PLAYER_SPRITE_SHEET_ROWS);
    if (ResourcePath_Resolve(path, resolvedPath, sizeof(resolvedPath))) {
        Image image;

        image = LoadImage(resolvedPath);
        if (image.data != NULL) {
            AnalyzeSpriteSheetFrames(&image, frames);
            asset.texture = LoadTextureFromImage(image);
            asset.loaded = asset.texture.id != 0;
            UnloadImage(image);
        }
    }

    if (asset.loaded) {
        SetTextureFilter(asset.texture, TEXTURE_FILTER_BILINEAR);
    }

    return asset;
}

void AssetsInternal_UnloadTextureAsset(TextureAsset *asset) {
    if (asset->loaded) {
        UnloadTexture(asset->texture);
    }

    std::memset(asset, 0, sizeof(*asset));
}
