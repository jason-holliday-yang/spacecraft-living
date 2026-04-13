#ifndef ASSETS_INTERNAL_H
#define ASSETS_INTERNAL_H

#include "assets.h"

Font AssetsInternal_LoadUIFont(void);
TextureAsset AssetsInternal_LoadOptionalTextureFitted(const char *path, int maxWidth, int maxHeight);
TextureAsset AssetsInternal_LoadOptionalTextureFittedPreserveFrame(const char *path, int maxWidth, int maxHeight);
TextureAsset AssetsInternal_LoadOptionalTextureSheetFitted(const char *path, Rectangle *frames);
void AssetsInternal_UnloadTextureAsset(TextureAsset *asset);
void AssetsInternal_LoadNarrativeAssets(AssetBundle *assets);
void AssetsInternal_UnloadNarrativeAssets(AssetBundle *assets);
void AssetsInternal_LoadGameplayAssets(AssetBundle *assets,
                                       int singleTileSize,
                                       int stationWidth,
                                       int stationHeight,
                                       int worldInteractiveSize,
                                       int uiIconSize,
                                       int monsterSize);
void AssetsInternal_UnloadGameplayAssets(AssetBundle *assets);

#endif
