#include "map.h"
#include "player.h"
#include "task_system.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool condition, const char *message) {
    if (condition) {
        return;
    }

    fprintf(stderr, "task_text_smoke failed: %s\n", message);
    exit(1);
}

static void PrepareEndingBranch(TaskSystem *tasks) {
    int index;

    tasks->stage = 7;
    tasks->ending = ENDING_NONE;
    tasks->selectedEndingRoute = ENDING_NONE;
    tasks->signalTowerActivated = false;
    tasks->oxygenRepairLevel = 2;
    tasks->commRepairLevel = 1;
    tasks->energyRepairLevel = 1;
    tasks->crashClueFound = true;
    tasks->amplifierUnlocked = true;
    tasks->westW5Completed = true;
    tasks->southS5Completed = true;
    for (index = 0; index < tasks->logCount; index++) {
        tasks->logs[index].collected = true;
        tasks->logs[index].active = true;
    }
}

int main(void) {
    GameMap map;
    Player player;
    TaskSystem tasks;
    int markerX;
    int markerY;

    Map_Init(&map);
    Player_Init(&player);
    Tasks_Init(&tasks, &map);

    player.gridX = WORKBENCH_X;
    player.gridY = WORKBENCH_Y;

    tasks.stage = 7;
    tasks.bossDefeated = false;
    player.hasSignalAmplifier = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.") == 0,
            "stage 7 objective should now prioritize finishing the archive before any ending route can be chosen");
    Require(strstr(tasks.communicator, "Current Area: Ship Base") != NULL,
            "communicator should include the current area label");
    Require(strstr(tasks.communicator, "Location: Workshop") != NULL,
            "communicator should include the more specific location label inside the current area");
    Require(strstr(tasks.communicator, "Loxi Tip:") != NULL,
            "communicator should include the richer Loxi guidance line");
    Require(strstr(tasks.communicator, "Field Note:") != NULL,
            "communicator should include the new field-note line");
    Require(strstr(tasks.communicator, "Route Board:") == NULL,
            "communicator should no longer include the old west-south route board line");
    Require(strstr(tasks.communicator, "W1") == NULL && strstr(tasks.communicator, "S1") == NULL && strstr(tasks.communicator, "X1") == NULL,
            "communicator should strip shorthand route codes from player-facing text");
    Require(strstr(tasks.communicator, "archive phase") != NULL,
            "stage 7 base communicator guidance should explain that the run is still assembling the final archive");

    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.") == 0,
            "crafting late-game gear should not skip the archive gate before the branch point opens");
    Require(strstr(tasks.communicator, "archive") != NULL,
            "stage 7 base guidance should keep archive completion visible even if the amplifier is already in hand");

    player.hasSignalAmplifier = false;
    tasks.bossDefeated = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.") == 0,
            "boss progress alone should not bypass the ship-side ending branch point");

    Require(strcmp(Tasks_GetStageName(4), "Rising Risk") == 0,
            "stage text lookup should return the cleaner player-facing chapter title");
    Require(strcmp(Tasks_GetEndingTitle(ENDING_PEACEFUL), "Peaceful Rescue") == 0,
            "ending title lookup should still return the expected label");
    Require(strstr(Tasks_GetEndingBody(ENDING_SETTLEMENT), "long-term home") != NULL,
            "ending body lookup should still expose the expected settlement text");
    Require(strstr(Tasks_GetEndingBody(ENDING_FAILURE), "Repeated collapses") != NULL,
            "failure ending text should match the broader death tolerance rules");

    tasks.stage = 3;
    player.gridX = LOXI_TERMINAL_X;
    player.gridY = LOXI_TERMINAL_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "comm relay") != NULL || strstr(tasks.communicator, "east airlock") != NULL,
            "stage 3 base guidance should keep east-relay setup visible");

    tasks.stage = 4;
    player.gridX = WORKBENCH_X;
    player.gridY = WORKBENCH_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Laser Gun and Protection Suit") != NULL,
            "stage 4 base guidance should keep the weapon and suit tied together before the crash clue");
    Require(strstr(tasks.communicator, "crash clue") != NULL,
            "stage 4 base guidance should keep crash clue progression visible");

    tasks.stage = 5;
    player.gridX = CRASH_CLUE_X;
    player.gridY = CRASH_CLUE_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Rope") != NULL || strstr(tasks.communicator, "Camp") != NULL,
            "stage 5 communicator guidance should explain the rope and camp support tools");
    Require(strstr(tasks.communicator, "east route") != NULL || strstr(tasks.communicator, "Energy Core") != NULL,
            "stage 5 communicator guidance should keep east qualification context visible");
    player.resources[RESOURCE_ENERGY_CORE] = 0;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 5 should expose an east-route marker before the core is collected");
    Require(markerX == EXTERIOR_X(112) && markerY == EXTERIOR_Y(56),
            "stage 5 should point toward the deep-swamp objective shelf before the energy core is secured");
    player.resources[RESOURCE_ENERGY_CORE] = 1;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 5 should still expose a return marker after the core is collected");
    Require(markerX == ENERGY_CONSOLE_X && markerY == ENERGY_CONSOLE_Y,
            "stage 5 should return the marker to the energy console once the core is secured");
    player.gridX = ENERGY_CONSOLE_X;
    player.gridY = ENERGY_CONSOLE_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "power bay") != NULL || strstr(tasks.communicator, "Energy Core") != NULL,
            "stage 5 base guidance should explain that bringing the Energy Core home must convert into power progress");

    tasks.stage = 2;
    player.gridX = EXTERIOR_X(50);
    player.gridY = EXTERIOR_Y(90);
    player.crouching = false;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    player.pressure = 0.0f;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Crouch") != NULL || strstr(tasks.communicator, "stealth") != NULL,
            "forest field note should teach crouch stealth rule");

    player.crouching = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "reduces detection") != NULL || strstr(tasks.communicator, "crouching") != NULL,
            "forest field note should react once crouch stealth is active");

    tasks.stage = 6;
    player.gridX = EXTERIOR_X(112);
    player.gridY = EXTERIOR_Y(72);
    player.crouching = false;
    player.hasProtectionSuit = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Protection Suit") != NULL
                || strstr(tasks.communicator, "deep swamp") != NULL
                || strstr(tasks.communicator, "Deep Basin") != NULL,
            "deep-swamp field note should explain poison counterplay before the suit is crafted");

    player.hasProtectionSuit = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Protection Suit") != NULL
                || strstr(tasks.communicator, "dangerous") != NULL
                || strstr(tasks.communicator, "Deep Basin") != NULL,
            "deep-swamp field note should react after the protection suit is available");

    player.gridX = EXTERIOR_X(112);
    player.gridY = EXTERIOR_Y(56);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Current Area: Spore Swamp") != NULL,
            "east-route communicator should keep the broader swamp area name stable");
    Require(strstr(tasks.communicator, "Deep Gate") != NULL,
            "communicator should expose the deep-gate location name for the east route transition");

    player.gridX = EXTERIOR_X(106);
    player.gridY = EXTERIOR_Y(84);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Current Area: Spore Swamp") != NULL,
            "outer-swamp communicator should share the same broad area label as other east-route shelves");
    Require(strstr(tasks.communicator, "Flooded Detour") != NULL,
            "communicator should expose the flooded-detour location name for the outer swamp route split");

    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should be selectable once the archive is complete");
    tasks.bossDefeated = false;
    tasks.monolithsLit = 0;
    player.gridX = SIGNAL_TOWER_X;
    player.gridY = SIGNAL_TOWER_Y + 6;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Current Area: Ruins") != NULL,
            "north-route communicator should use the same ruins area label as the map overlay");
    Require(strstr(tasks.communicator, "Plateau") != NULL || strstr(tasks.communicator, "Signal Tower") != NULL,
            "ruins field note should explain endgame threat in the plateau route");
    Require(strstr(tasks.communicator, "Signal Tower Plateau") != NULL,
            "communicator should expose the sub-location name for the northern endgame route");

    tasks.monolithsLit = 2;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.objective, "Light remaining monoliths") != NULL || strstr(tasks.objective, "remaining monoliths") != NULL,
            "heroic route objective should tighten around the remaining ring prep once partial progress exists");
    Require(strstr(tasks.communicator, "Light remaining monoliths") != NULL || strstr(tasks.communicator, "remaining") != NULL,
            "stage 7 communicator should reflect that the ring is nearly complete");

    tasks.monolithsLit = 3;
    tasks.monolithActivated[0] = true;
    tasks.monolithActivated[1] = true;
    tasks.monolithActivated[2] = true;
    tasks.monolithPuzzle.solved = true;
    player.gridX = MONOLITH_B_X;
    player.gridY = MONOLITH_B_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.objective, "ring is complete") != NULL || strstr(tasks.objective, "Defeat the guardian") != NULL,
            "heroic route objective should flip into the guardian push once the ring is complete");
    Require(strstr(tasks.communicator, "Heroic route chosen") != NULL
                || strstr(tasks.communicator, "guardian") != NULL
                || strstr(tasks.communicator, "Signal Tower") != NULL,
            "fully lit monolith guidance should explain the completed heroic prep payoff");

    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = SIGNAL_TOWER_X;
    tasks.monsters[0].gridY = SIGNAL_TOWER_Y + 10;
    tasks.monsters[0].maxHealth = 220.0f;
    tasks.monsters[0].health = 220.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_CHARGE;
    tasks.monsters[0].attackTelegraph = 1.2f;
    player.gridX = SIGNAL_TOWER_X;
    player.gridY = SIGNAL_TOWER_Y + 6;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Signal Tower") != NULL || strstr(tasks.communicator, "Plateau") != NULL,
            "tower communicator should stay relevant during the final climb");

    tasks.monsters[0].attackTelegraph = 0.0f;
    tasks.monsters[0].phaseTriggered = true;
    tasks.monsters[0].health = 120.0f;
    tasks.monolithsLit = 0;
    tasks.monolithActivated[0] = false;
    tasks.monolithActivated[1] = false;
    tasks.monolithActivated[2] = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "guardian") != NULL
                || strstr(tasks.communicator, "tower") != NULL
                || strstr(tasks.communicator, "Heroic route chosen") != NULL,
            "tower communicator should explain the ongoing chosen-route pressure in the final phase");

    tasks.stage = 6;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 0;
    player.gridX = PLAYER_START_X;
    player.gridY = PLAYER_START_Y;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 6 should still expose a north-route objective marker");
    Require(markerX == EXTERIOR_X(64) && markerY == EXTERIOR_Y(27),
            "stage 6 should guide the player to the north ruins approach before fragments are collected");

    player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 6 should still expose a return marker after fragment collection");
    Require(markerX == LOXI_TERMINAL_X && markerY == LOXI_TERMINAL_Y,
            "stage 6 should return the marker to Loxi after enough relic fragments are collected");

    tasks.stage = 7;
    tasks.bossDefeated = false;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 0;
    player.hasSignalAmplifier = false;
    tasks.selectedEndingRoute = ENDING_NONE;
    PrepareEndingBranch(&tasks);
    tasks.monolithActivated[0] = false;
    tasks.monolithActivated[1] = false;
    tasks.monolithActivated[2] = false;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 7 should expose a Loxi marker once the archive is complete but the ending is not chosen");
    Require(markerX == LOXI_TERMINAL_X && markerY == LOXI_TERMINAL_Y,
            "stage 7 should guide the player back to Loxi for the ending branch point before route-specific objectives resume");

    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should remain selectable for marker tests");
    tasks.monolithActivated[1] = true;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 7 should continue exposing a monolith marker while the sequence is incomplete");
    Require(markerX == MONOLITH_A_X && markerY == MONOLITH_A_Y,
            "stage 7 should advance the marker to the next unresolved monolith");

    player.gridX = WORKBENCH_X;
    player.gridY = WORKBENCH_Y;
    tasks.selectedEndingRoute = ENDING_NONE;
    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_PEACEFUL),
            "peaceful route should remain selectable for base guidance tests");
    tasks.bossDefeated = false;
    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Peaceful route chosen") != NULL || strstr(tasks.communicator, "Signal Amplifier") != NULL,
            "peaceful-route guidance should keep amplifier route visible");
    Require(strstr(tasks.communicator, "tower") != NULL || strstr(tasks.communicator, "Signal Amplifier") != NULL,
            "stage 7 base guidance should explain the peaceful branch-specific next step");

    player.hasSignalAmplifier = false;
    tasks.selectedEndingRoute = ENDING_NONE;
    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should remain selectable for late base guidance tests");
    tasks.bossDefeated = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Heroic route chosen") != NULL || strstr(tasks.communicator, "Signal Tower") != NULL,
            "stage 7 base guidance should explain that post-boss heroic flow now returns directly to the tower");

    tasks.stage = 6;
    tasks.bossDefeated = false;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Loxi") != NULL || strstr(tasks.communicator, "fragments") != NULL,
            "stage 6 base guidance should explain that returning fragments to Loxi turns the ruins into a readable endgame plan");

    tasks.stage = 4;
    tasks.commRepairLevel = 1;
    player.gridX = EXTERIOR_X(24);
    player.gridY = EXTERIOR_Y(72);
    player.hasSignalAmplifier = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "West Frontier") != NULL,
            "communicator should expose the west reserved frontier location name");
    Require(strstr(tasks.communicator, "W1") == NULL,
            "west frontier guidance should no longer expose shorthand route framing");

    tasks.stage = 5;
    tasks.energyRepairLevel = 1;
    player.gridX = EXTERIOR_X(82);
    player.gridY = EXTERIOR_Y(96);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "South Collapse") != NULL,
            "communicator should expose the south reserved frontier location name");
    Require(strstr(tasks.communicator, "S1") == NULL,
            "south frontier guidance should no longer expose shorthand route framing");

    tasks.westW1Started = true;
    tasks.westW1Completed = false;
    tasks.southS1Started = true;
    tasks.southS1Completed = true;
    player.gridX = WORKBENCH_X;
    player.gridY = WORKBENCH_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W1") == NULL && strstr(tasks.communicator, "S2") == NULL,
            "base communicator should keep route shorthand hidden during mid-chain progression");

    tasks.westW1Completed = true;
    tasks.westW2Started = false;
    tasks.westW2Completed = false;
    tasks.southS1Completed = true;
    tasks.southS2Started = false;
    tasks.southS2Completed = false;
    tasks.westW3Started = false;
    tasks.westW3Completed = false;
    tasks.westW4Started = false;
    tasks.westW4Completed = false;
    tasks.westW5Started = false;
    tasks.westW5Completed = false;
    tasks.southS3Started = false;
    tasks.southS3Completed = false;
    tasks.southS4Started = false;
    tasks.southS4Completed = false;
    tasks.southS5Started = false;
    tasks.southS5Completed = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W2") == NULL && strstr(tasks.communicator, "S2") == NULL,
            "communicator should keep shorthand hidden once later passes unlock");

    tasks.westW2Started = true;
    tasks.southS2Started = true;
    tasks.southS2Completed = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W2") == NULL && strstr(tasks.communicator, "S3") == NULL,
            "communicator should keep shorthand hidden while chain progression updates");

    tasks.westW2Completed = true;
    tasks.westW3Started = false;
    tasks.westW3Completed = false;
    tasks.southS3Started = true;
    tasks.southS3Completed = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W3") == NULL && strstr(tasks.communicator, "S3") == NULL,
            "communicator should keep shorthand hidden during third-pass progression");

    tasks.westW3Completed = true;
    tasks.westW4Started = false;
    tasks.westW4Completed = false;
    tasks.southS3Completed = true;
    tasks.southS4Started = false;
    tasks.southS4Completed = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W4") == NULL && strstr(tasks.communicator, "S4") == NULL && strstr(tasks.communicator, "X1") == NULL,
            "communicator should keep shorthand hidden once cross-route context unlocks");
    tasks.stage = 6;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "timeline") != NULL || strstr(tasks.communicator, "west and south") != NULL,
            "communicator should explain that early cross-route context now aligns the west and south record without exposing shorthand");

    tasks.westW4Started = true;
    tasks.southS4Started = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W4") == NULL && strstr(tasks.communicator, "S4") == NULL,
            "communicator should keep shorthand hidden while fourth-pass progression updates");

    tasks.westW4Completed = true;
    tasks.westW5Started = false;
    tasks.westW5Completed = false;
    tasks.southS4Completed = true;
    tasks.southS5Started = false;
    tasks.southS5Completed = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "W5") == NULL && strstr(tasks.communicator, "S5") == NULL && strstr(tasks.communicator, "X2") == NULL,
            "communicator should keep shorthand hidden during late-chain progression");

    player.gridX = EXTERIOR_X(44);
    player.gridY = EXTERIOR_Y(68);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Canopy Hollow") != NULL,
            "communicator should expose the Canopy Hollow location name for west W3");
    Require(strstr(tasks.communicator, "W3") == NULL,
            "Canopy Hollow guidance should not expose shorthand progression codes");

    player.gridX = EXTERIOR_X(48);
    player.gridY = EXTERIOR_Y(78);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Echo Basin") != NULL,
            "communicator should expose the Echo Basin location name for west W4");

    player.gridX = EXTERIOR_X(48);
    player.gridY = EXTERIOR_Y(88);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Last Camp") != NULL,
            "communicator should expose the Last Camp location name for west W5");

    tasks.southS3Started = false;
    tasks.southS3Completed = true;
    tasks.southS4Completed = true;
    tasks.westW5Completed = true;
    tasks.southS5Completed = true;
    player.gridX = EXTERIOR_X(108);
    player.gridY = EXTERIOR_Y(96);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Service Shafts") != NULL,
            "communicator should expose the Service Shafts location name for south S3");
    Require(strstr(tasks.communicator, "S5") == NULL && strstr(tasks.communicator, "X3") == NULL,
            "communicator should keep shorthand hidden after the final west and south passes are archived");

    player.gridX = EXTERIOR_X(114);
    player.gridY = EXTERIOR_Y(96);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Purifier Ring") != NULL,
            "communicator should expose the Purifier Ring location name for south S4");

    player.gridX = EXTERIOR_X(122);
    player.gridY = EXTERIOR_Y(96);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Root Vault") != NULL,
            "communicator should expose the Root Vault location name for south S5");

    PrepareEndingBranch(&tasks);
    tasks.bossDefeated = false;
    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.objective, "choose the final route") != NULL || strstr(tasks.objective, "Return to Loxi") != NULL,
            "stage 7 objective should send the player back to the Loxi branch point once the archive is complete");
    Require(strstr(tasks.objective, "X3") == NULL,
            "stage 7 objective should no longer expose shorthand cross-route codes");
    Require(strstr(tasks.communicator, "heroic rescue") != NULL && strstr(tasks.communicator, "peaceful rescue") != NULL,
            "stage 7 base communicator should now explain the three-route comparison with explicit player-facing route names");

    puts("task_text smoke ok");
    return 0;
}
