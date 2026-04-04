#include "assets.h"
#include <string.h>

static Font LoadUIFont(void) {
    const char *fontPaths[] = {
        "/System/Library/Fonts/SFNS.ttf",
        "/System/Library/Fonts/HelveticaNeue.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Avenir Next.ttc"
    };
    const int fontBaseSize = 42;
    int pathIndex;

    for (pathIndex = 0; pathIndex < (int)(sizeof(fontPaths) / sizeof(fontPaths[0])); pathIndex++) {
        if (FileExists(fontPaths[pathIndex])) {
            Font font;

            font = LoadFontEx(fontPaths[pathIndex], fontBaseSize, NULL, 0);
            if (font.texture.id != 0) {
                SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
                return font;
            }
        }
    }

    return (Font){0};
}

static TextureAsset LoadOptionalTexture(const char *path) {
    TextureAsset asset;

    memset(&asset, 0, sizeof(asset));
    if (FileExists(path)) {
        asset.texture = LoadTexture(path);
        asset.loaded = asset.texture.id != 0;
    }
    return asset;
}

static void UnloadTextureAsset(TextureAsset *asset) {
    if (asset->loaded) {
        UnloadTexture(asset->texture);
    }

    memset(asset, 0, sizeof(*asset));
}

void Assets_Load(AssetBundle *assets) {
    memset(assets, 0, sizeof(*assets));

    assets->player = LoadOptionalTexture("resources/images/player_astronaut.png");
    assets->loxiTerminal = LoadOptionalTexture("resources/images/npc_loxi_terminal.png");
    assets->tileBaseFloor = LoadOptionalTexture("resources/images/tile_base_floor.png");
    assets->tileForestGround = LoadOptionalTexture("resources/images/tile_forest_ground.png");
    assets->tileSwampGround = LoadOptionalTexture("resources/images/tile_swamp_ground.png");
    assets->tileRuinsFloor = LoadOptionalTexture("resources/images/tile_ruins_floor.png");
    assets->tileTree = LoadOptionalTexture("resources/images/tile_tree_dense.png");
    assets->tileRock = LoadOptionalTexture("resources/images/tile_rock_large.png");
    assets->tileWorkbench = LoadOptionalTexture("resources/images/tile_workbench.png");
    assets->tileSignalTower = LoadOptionalTexture("resources/images/tile_signal_tower.png");
    assets->boss = LoadOptionalTexture("resources/images/mob_final_boss.png");
    assets->uiFont = LoadUIFont();
    assets->uiFontLoaded = assets->uiFont.texture.id != 0;
}

void Assets_Unload(AssetBundle *assets) {
    UnloadTextureAsset(&assets->player);
    UnloadTextureAsset(&assets->loxiTerminal);
    UnloadTextureAsset(&assets->tileBaseFloor);
    UnloadTextureAsset(&assets->tileForestGround);
    UnloadTextureAsset(&assets->tileSwampGround);
    UnloadTextureAsset(&assets->tileRuinsFloor);
    UnloadTextureAsset(&assets->tileTree);
    UnloadTextureAsset(&assets->tileRock);
    UnloadTextureAsset(&assets->tileWorkbench);
    UnloadTextureAsset(&assets->tileSignalTower);
    UnloadTextureAsset(&assets->boss);

    if (assets->uiFontLoaded) {
        UnloadFont(assets->uiFont);
        assets->uiFont = (Font){0};
        assets->uiFontLoaded = false;
    }
}
