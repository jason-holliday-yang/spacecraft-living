#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include "raylib.h"

typedef struct TextureAsset {
    Texture2D texture;
    bool loaded;
} TextureAsset;

typedef struct AssetBundle {
    TextureAsset player;
    TextureAsset loxiTerminal;
    TextureAsset tileBaseFloor;
    TextureAsset tileForestGround;
    TextureAsset tileSwampGround;
    TextureAsset tileRuinsFloor;
    TextureAsset tileTree;
    TextureAsset tileRock;
    TextureAsset tileWorkbench;
    TextureAsset tileSignalTower;
    TextureAsset boss;
    Font uiFont;
    bool uiFontLoaded;
} AssetBundle;

void Assets_Load(AssetBundle *assets);
void Assets_Unload(AssetBundle *assets);

#endif
