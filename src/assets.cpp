#include "assets_internal.h"

#include "config.h"

#include <cstring>

void Assets_Load(AssetBundle *assets) {
    int singleTileSize;
    int stationWidth;
    int stationHeight;
    int worldInteractiveSize;
    int uiIconSize;
    int monsterSize;

    std::memset(assets, 0, sizeof(*assets));

    singleTileSize = TILE_SIZE;
    stationWidth = TILE_SIZE * STATION_FOOTPRINT_WIDTH;
    stationHeight = TILE_SIZE * STATION_FOOTPRINT_HEIGHT;
    worldInteractiveSize = TILE_SIZE * WORLD_INTERACTIVE_FOOTPRINT_SIZE;
    uiIconSize = TILE_SIZE + 24;
    monsterSize = TILE_SIZE * MONSTER_FOOTPRINT_SIZE;

    assets->player = AssetsInternal_LoadOptionalTextureSheetFitted(
        "resources/images/my_astronaut.png",
        assets->playerSpriteFrames
    );
    assets->playerIsSpriteSheet = assets->player.loaded;
    if (!assets->player.loaded) {
        assets->player = AssetsInternal_LoadOptionalTextureFitted("resources/images/player_astronaut.png", singleTileSize, singleTileSize);
        assets->playerIsSpriteSheet = false;
    }

    AssetsInternal_LoadNarrativeAssets(assets);
    AssetsInternal_LoadGameplayAssets(assets,
                                      singleTileSize,
                                      stationWidth,
                                      stationHeight,
                                      worldInteractiveSize,
                                      uiIconSize,
                                      monsterSize);

    assets->uiFont = AssetsInternal_LoadUIFont();
    assets->uiFontLoaded = assets->uiFont.texture.id != 0;
}

void Assets_Unload(AssetBundle *assets) {
    AssetsInternal_UnloadTextureAsset(&assets->player);
    AssetsInternal_UnloadNarrativeAssets(assets);
    AssetsInternal_UnloadGameplayAssets(assets);

    if (assets->uiFontLoaded) {
        UnloadFont(assets->uiFont);
        assets->uiFont = Font{};
        assets->uiFontLoaded = false;
    }
}
