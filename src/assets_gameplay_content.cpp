#include "assets_internal.h"

#include "config.h"

#include <cmath>

void AssetsInternal_LoadGameplayAssets(AssetBundle *assets,
                                       int singleTileSize,
                                       int stationWidth,
                                       int stationHeight,
                                       int worldInteractiveSize,
                                       int uiIconSize,
                                       int monsterSize) {
    const int shipFurnitureSize = TILE_SIZE * SHIP_FURNITURE_FOOTPRINT_SIZE;
    const int airlockDoorHeight = TILE_SIZE * AIRLOCK_DOOR_HEIGHT;

    assets->loxiTerminal = AssetsInternal_LoadOptionalTextureFitted("resources/images/npc_loxi_terminal.png", stationWidth, stationHeight);
    assets->tileBaseFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_base_floor.png", singleTileSize, singleTileSize);
    assets->tileShipCorridorFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_ship_corridor_floor.png", singleTileSize, singleTileSize);
    assets->tileShipRoomFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_ship_room_floor.png", singleTileSize, singleTileSize);
    assets->tileForestGround = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_forest_ground.png", singleTileSize, singleTileSize);
    assets->tileSwampGround = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_swamp_ground.png", singleTileSize, singleTileSize);
    assets->tileSwampOuterGround = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_swamp_outer_ground.png", singleTileSize, singleTileSize);
    assets->tileSwampDeepGround = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_swamp_deep_ground.png", singleTileSize, singleTileSize);
    assets->tileEchoBasinFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_echo_basin_floor.png", singleTileSize, singleTileSize);
    assets->tileRuinsFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_ruins_floor.png", singleTileSize, singleTileSize);
    assets->tileRuinsApproachFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_ruins_path.png", singleTileSize, singleTileSize);
    assets->tileRuinsRingFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_ruins_ring_floor.png", singleTileSize, singleTileSize);
    assets->tileTowerPlateauFloor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_tower_plateau_floor.png", singleTileSize, singleTileSize);
    assets->tileTree = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_tree_dense.png", singleTileSize, singleTileSize);
    assets->tileRock = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_rock_large.png", singleTileSize, singleTileSize);
    assets->tileTechTable = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_tech_table.png", shipFurnitureSize, shipFurnitureSize);
    assets->tileStorageLocker = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_storage_locker.png", shipFurnitureSize, shipFurnitureSize);
    assets->tileBunk = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_bunk.png", shipFurnitureSize, shipFurnitureSize);
    assets->tileWorkbench = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_workbench.png", stationWidth, stationHeight);
    assets->tileOxygenConsole = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_oxygen_console.png", stationWidth, stationHeight);
    assets->tileAirlockConsole = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_airlock_console.png", stationWidth, stationHeight);
    assets->tileAirlockDoor = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_airlock_door.png", singleTileSize, airlockDoorHeight);
    assets->tileEnergyConsole = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_energy_console.png", stationWidth, stationHeight);
    assets->tileCommRelay = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_comm_relay.png", worldInteractiveSize, worldInteractiveSize);
    assets->tileSignalTower = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_signal_tower.png", worldInteractiveSize, worldInteractiveSize);
    assets->tileCrashClue = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_crash_clue.png", worldInteractiveSize, worldInteractiveSize);
    assets->tileMonolith = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_monolith.png", worldInteractiveSize, worldInteractiveSize);
    assets->tileBarrierSwamp = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_barrier_swamp.png", singleTileSize, singleTileSize);
    assets->tileBarrierDeep = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_barrier_deep.png", singleTileSize, singleTileSize);
    assets->tileBarrierRuins = AssetsInternal_LoadOptionalTextureFitted("resources/images/tile_barrier_ruins.png", singleTileSize, singleTileSize);
    assets->nodeWood = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_wood.png", singleTileSize, singleTileSize);
    assets->nodeOre = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_ore.png", singleTileSize, singleTileSize);
    assets->nodeMetalScrap = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_metal_scrap.png", singleTileSize, singleTileSize);
    assets->nodeFruit = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_fruit.png", singleTileSize, singleTileSize);
    assets->nodeSpecialFungus = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_special_fungus.png", singleTileSize, singleTileSize);
    assets->nodeEnergyCore = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_energy_core.png", singleTileSize, singleTileSize);
    assets->nodeGlowMoss = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_glow_moss.png", singleTileSize, singleTileSize);
    assets->nodeAlienVine = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_alien_vine.png", singleTileSize, singleTileSize);
    assets->nodeShellFruit = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_shell_fruit.png", singleTileSize, singleTileSize);
    assets->nodeJunkMetal = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_junk_metal.png", singleTileSize, singleTileSize);
    assets->nodeEnergyCrystal = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_energy_crystal.png", singleTileSize, singleTileSize);
    assets->nodeCalmMushroom = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_calm_mushroom.png", singleTileSize, singleTileSize);
    assets->nodeProtectiveFiber = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_protective_fiber.png", singleTileSize, singleTileSize);
    assets->nodeRelicFragment = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_relic_fragment.png", singleTileSize, singleTileSize);
    assets->nodeBossScale = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_boss_scale.png", singleTileSize, singleTileSize);
    assets->nodeAlienSlime = AssetsInternal_LoadOptionalTextureFitted("resources/images/node_alien_slime.png", singleTileSize, singleTileSize);
    assets->iconGlowStick = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_glow_stick.png", uiIconSize, uiIconSize);
    assets->iconRope = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_rope.png", uiIconSize, uiIconSize);
    assets->iconReinforcedMetal = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_reinforced_metal.png", uiIconSize, uiIconSize);
    assets->iconLaserGun = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_laser_gun.png", uiIconSize, uiIconSize);
    assets->iconProtectionSuit = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_protection_suit.png", uiIconSize, uiIconSize);
    assets->iconSignalAmplifier = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_signal_amplifier.png", uiIconSize, uiIconSize);
    assets->iconFieldCamp = AssetsInternal_LoadOptionalTextureFitted("resources/images/icon_field_camp.png", uiIconSize, uiIconSize);
    assets->statusPoisoned = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_poisoned.png", uiIconSize, uiIconSize);
    assets->statusOxygenLeak = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_oxygen_leak.png", uiIconSize, uiIconSize);
    assets->statusLowOxygen = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_low_oxygen.png", uiIconSize, uiIconSize);
    assets->statusSuffocating = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_suffocating.png", uiIconSize, uiIconSize);
    assets->statusCriticalCondition = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_critical_condition.png", uiIconSize, uiIconSize);
    assets->statusFiltered = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_filtered.png", uiIconSize, uiIconSize);
    assets->statusOxygenReserve = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_oxygen_reserve.png", uiIconSize, uiIconSize);
    assets->statusCampRecovery = AssetsInternal_LoadOptionalTextureFitted("resources/images/status_camp_recovery.png", uiIconSize, uiIconSize);
    assets->mobThornLarva = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_thorn_larva.png", monsterSize, monsterSize);
    assets->mobWingBug = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_wing_bug.png", monsterSize, monsterSize);
    assets->mobRaptor = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_raptor.png", monsterSize, monsterSize);
    assets->mobSwampStalker = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_swamp_stalker.png", monsterSize, monsterSize);
    assets->mobSentinelJelly = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_sentinel_jelly.png", monsterSize, monsterSize);
    assets->mobFogWorm = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_fog_worm.png", monsterSize, monsterSize);
    assets->mobRelicGuard = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_relic_guard.png", monsterSize, monsterSize);
    assets->boss = AssetsInternal_LoadOptionalTextureFitted("resources/images/mob_final_boss.png",
                                                            monsterSize,
                                                            monsterSize);
}

void AssetsInternal_UnloadGameplayAssets(AssetBundle *assets) {
    AssetsInternal_UnloadTextureAsset(&assets->loxiTerminal);
    AssetsInternal_UnloadTextureAsset(&assets->tileBaseFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileShipCorridorFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileShipRoomFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileForestGround);
    AssetsInternal_UnloadTextureAsset(&assets->tileSwampGround);
    AssetsInternal_UnloadTextureAsset(&assets->tileSwampOuterGround);
    AssetsInternal_UnloadTextureAsset(&assets->tileSwampDeepGround);
    AssetsInternal_UnloadTextureAsset(&assets->tileEchoBasinFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileRuinsFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileRuinsApproachFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileRuinsRingFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileTowerPlateauFloor);
    AssetsInternal_UnloadTextureAsset(&assets->tileTree);
    AssetsInternal_UnloadTextureAsset(&assets->tileRock);
    AssetsInternal_UnloadTextureAsset(&assets->tileTechTable);
    AssetsInternal_UnloadTextureAsset(&assets->tileStorageLocker);
    AssetsInternal_UnloadTextureAsset(&assets->tileBunk);
    AssetsInternal_UnloadTextureAsset(&assets->tileWorkbench);
    AssetsInternal_UnloadTextureAsset(&assets->tileOxygenConsole);
    AssetsInternal_UnloadTextureAsset(&assets->tileAirlockConsole);
    AssetsInternal_UnloadTextureAsset(&assets->tileAirlockDoor);
    AssetsInternal_UnloadTextureAsset(&assets->tileEnergyConsole);
    AssetsInternal_UnloadTextureAsset(&assets->tileCommRelay);
    AssetsInternal_UnloadTextureAsset(&assets->tileSignalTower);
    AssetsInternal_UnloadTextureAsset(&assets->tileCrashClue);
    AssetsInternal_UnloadTextureAsset(&assets->tileMonolith);
    AssetsInternal_UnloadTextureAsset(&assets->tileBarrierSwamp);
    AssetsInternal_UnloadTextureAsset(&assets->tileBarrierDeep);
    AssetsInternal_UnloadTextureAsset(&assets->tileBarrierRuins);
    AssetsInternal_UnloadTextureAsset(&assets->nodeWood);
    AssetsInternal_UnloadTextureAsset(&assets->nodeOre);
    AssetsInternal_UnloadTextureAsset(&assets->nodeMetalScrap);
    AssetsInternal_UnloadTextureAsset(&assets->nodeFruit);
    AssetsInternal_UnloadTextureAsset(&assets->nodeSpecialFungus);
    AssetsInternal_UnloadTextureAsset(&assets->nodeEnergyCore);
    AssetsInternal_UnloadTextureAsset(&assets->nodeGlowMoss);
    AssetsInternal_UnloadTextureAsset(&assets->nodeAlienVine);
    AssetsInternal_UnloadTextureAsset(&assets->nodeShellFruit);
    AssetsInternal_UnloadTextureAsset(&assets->nodeJunkMetal);
    AssetsInternal_UnloadTextureAsset(&assets->nodeEnergyCrystal);
    AssetsInternal_UnloadTextureAsset(&assets->nodeCalmMushroom);
    AssetsInternal_UnloadTextureAsset(&assets->nodeProtectiveFiber);
    AssetsInternal_UnloadTextureAsset(&assets->nodeRelicFragment);
    AssetsInternal_UnloadTextureAsset(&assets->nodeBossScale);
    AssetsInternal_UnloadTextureAsset(&assets->nodeAlienSlime);
    AssetsInternal_UnloadTextureAsset(&assets->iconGlowStick);
    AssetsInternal_UnloadTextureAsset(&assets->iconRope);
    AssetsInternal_UnloadTextureAsset(&assets->iconReinforcedMetal);
    AssetsInternal_UnloadTextureAsset(&assets->iconLaserGun);
    AssetsInternal_UnloadTextureAsset(&assets->iconProtectionSuit);
    AssetsInternal_UnloadTextureAsset(&assets->iconSignalAmplifier);
    AssetsInternal_UnloadTextureAsset(&assets->iconFieldCamp);
    AssetsInternal_UnloadTextureAsset(&assets->statusPoisoned);
    AssetsInternal_UnloadTextureAsset(&assets->statusOxygenLeak);
    AssetsInternal_UnloadTextureAsset(&assets->statusLowOxygen);
    AssetsInternal_UnloadTextureAsset(&assets->statusSuffocating);
    AssetsInternal_UnloadTextureAsset(&assets->statusCriticalCondition);
    AssetsInternal_UnloadTextureAsset(&assets->statusFiltered);
    AssetsInternal_UnloadTextureAsset(&assets->statusOxygenReserve);
    AssetsInternal_UnloadTextureAsset(&assets->statusCampRecovery);
    AssetsInternal_UnloadTextureAsset(&assets->mobThornLarva);
    AssetsInternal_UnloadTextureAsset(&assets->mobWingBug);
    AssetsInternal_UnloadTextureAsset(&assets->mobRaptor);
    AssetsInternal_UnloadTextureAsset(&assets->mobSwampStalker);
    AssetsInternal_UnloadTextureAsset(&assets->mobSentinelJelly);
    AssetsInternal_UnloadTextureAsset(&assets->mobFogWorm);
    AssetsInternal_UnloadTextureAsset(&assets->mobRelicGuard);
    AssetsInternal_UnloadTextureAsset(&assets->boss);
}
