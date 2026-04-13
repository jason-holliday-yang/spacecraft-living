#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include "c_compat.h"
#include "config.h"
#include "raylib.h"

/* Public C ABI for optional texture/font loading and the shared runtime asset bundle. */

SCL_EXTERN_C_BEGIN

typedef struct TextureAsset {
    Texture2D texture;
    bool loaded;
} TextureAsset;

#define INTRO_CUTSCENE_SLIDE_COUNT 5
#define STORY_MAIN_SCENE_COUNT 13
#define STORY_LOG_SCENE_COUNT 3
#define STORY_ENDING_SCENE_COUNT 4

typedef struct AssetBundle {
    TextureAsset player;
    bool playerIsSpriteSheet;
    Rectangle playerSpriteFrames[PLAYER_SPRITE_SHEET_ROWS * PLAYER_SPRITE_SHEET_COLS];
    TextureAsset introSlides[INTRO_CUTSCENE_SLIDE_COUNT];
    TextureAsset storyMainScenes[STORY_MAIN_SCENE_COUNT];
    TextureAsset storyLogScenes[STORY_LOG_SCENE_COUNT];
    TextureAsset storyEndingScenes[STORY_ENDING_SCENE_COUNT];
    TextureAsset loxiTerminal;
    TextureAsset tileBaseFloor;
    TextureAsset tileShipCorridorFloor;
    TextureAsset tileShipRoomFloor;
    TextureAsset tileForestGround;
    TextureAsset tileSwampGround;
    TextureAsset tileSwampOuterGround;
    TextureAsset tileSwampDeepGround;
    TextureAsset tileRuinsFloor;
    TextureAsset tileRuinsApproachFloor;
    TextureAsset tileRuinsRingFloor;
    TextureAsset tileTowerPlateauFloor;
    TextureAsset tileTree;
    TextureAsset tileRock;
    TextureAsset tileTechTable;
    TextureAsset tileStorageLocker;
    TextureAsset tileBunk;
    TextureAsset tileWorkbench;
    TextureAsset tileOxygenConsole;
    TextureAsset tileAirlockConsole;
    TextureAsset tileAirlockDoor;
    TextureAsset tileEnergyConsole;
    TextureAsset tileCommRelay;
    TextureAsset tileSignalTower;
    TextureAsset tileCrashClue;
    TextureAsset tileMonolith;
    TextureAsset tileBarrierSwamp;
    TextureAsset tileBarrierDeep;
    TextureAsset tileBarrierRuins;
    TextureAsset nodeWood;
    TextureAsset nodeOre;
    TextureAsset nodeMetalScrap;
    TextureAsset nodeFruit;
    TextureAsset nodeSpecialFungus;
    TextureAsset nodeEnergyCore;
    TextureAsset nodeGlowMoss;
    TextureAsset nodeAlienVine;
    TextureAsset nodeShellFruit;
    TextureAsset nodeJunkMetal;
    TextureAsset nodeEnergyCrystal;
    TextureAsset nodeCalmMushroom;
    TextureAsset nodeProtectiveFiber;
    TextureAsset nodeRelicFragment;
    TextureAsset nodeBossScale;
    TextureAsset nodeAlienSlime;
    TextureAsset iconGlowStick;
    TextureAsset iconRope;
    TextureAsset iconReinforcedMetal;
    TextureAsset iconLaserGun;
    TextureAsset iconProtectionSuit;
    TextureAsset iconSignalAmplifier;
    TextureAsset iconFieldCamp;
    TextureAsset mobThornLarva;
    TextureAsset mobWingBug;
    TextureAsset mobRaptor;
    TextureAsset mobSwampStalker;
    TextureAsset mobSentinelJelly;
    TextureAsset mobFogWorm;
    TextureAsset mobRelicGuard;
    TextureAsset boss;
    Font uiFont;
    bool uiFontLoaded;
} AssetBundle;

void Assets_Load(AssetBundle *assets);
void Assets_Unload(AssetBundle *assets);

SCL_EXTERN_C_END

#endif
