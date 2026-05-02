#include "map.h"
#include "localization.h"
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

static bool MatchesActiveNode(const TaskSystem *tasks, ResourceType resourceType, int gridX, int gridY) {
    int index;

    for (index = 0; index < tasks->nodeCount; index++) {
        if (tasks->nodes[index].active
            && tasks->nodes[index].type == resourceType
            && tasks->nodes[index].gridX == gridX
            && tasks->nodes[index].gridY == gridY) {
            return true;
        }
    }

    return false;
}

static void PrepareEndingBranch(TaskSystem *tasks) {
    int index;

    tasks->stage = 7;
    tasks->ending = ENDING_NONE;
    tasks->selectedEndingRoute = ENDING_NONE;
    tasks->endingArchiveReviewed = true;
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
    Require(tasks.logCount > 0, "task content should seed logs for communicator checks");
    Require(strstr(Tasks_GetLogDetailText(&tasks.logs[0]), "Keep the breathing corridor alive") != NULL,
            "log detail text should expose the expanded crash-survival reconstruction copy");

    player.gridX = WORKBENCH_X;
    player.gridY = WORKBENCH_Y;

    tasks.stage = 7;
    tasks.bossDefeated = false;
    player.hasSignalAmplifier = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover West Frontier, South Collapse, and the remaining mainline logs before choosing an ending with Loxi.") == 0,
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
    Require(strstr(tasks.communicator, "evidence sweep") != NULL,
            "stage 7 base communicator guidance should explain that the run is still assembling the final archive");

    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover West Frontier, South Collapse, and the remaining mainline logs before choosing an ending with Loxi.") == 0,
            "crafting late-game gear should not skip the archive gate before the branch point opens");
    Require(strstr(tasks.communicator, "archive") != NULL,
            "stage 7 base guidance should keep archive completion visible even if the amplifier is already in hand");

    player.hasSignalAmplifier = false;
    tasks.bossDefeated = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "Recover West Frontier, South Collapse, and the remaining mainline logs before choosing an ending with Loxi.") == 0,
            "boss progress alone should not bypass the ship-side ending branch point");

    tasks.stage = 1;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 1 should expose a marker for the opening ship archive route");
    Require(markerX == tasks.logs[0].gridX && markerY == tasks.logs[0].gridY,
            "stage 1 marker should point to the first uncollected ship archive before the initial oxygen repair");
    tasks.logs[0].collected = true;
    tasks.logs[1].collected = true;
    tasks.logs[2].collected = true;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 1 should still expose a marker after the opening ship archive set is recovered");
    Require(markerX == OXYGEN_CONSOLE_X && markerY == OXYGEN_CONSOLE_Y,
            "stage 1 marker should return to the oxygen console once the opening ship archive set is complete");
    tasks.logs[0].collected = false;
    tasks.logs[1].collected = false;
    tasks.logs[2].collected = false;

    tasks.stage = 2;
    tasks.logs[0].collected = true;
    tasks.logs[1].collected = true;
    tasks.logs[2].collected = true;
    player.hasGlowStick = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 2 should expose a workshop marker once the ship-intro archive set is finished");
    Require(markerX == WORKBENCH_X && markerY == WORKBENCH_Y,
            "stage 2 should point to the workshop before the Glow Stick is crafted");
    Require(strstr(tasks.objective, "Glow Stick") != NULL,
            "stage 2 objective should still call out the Glow Stick before it is crafted");

    player.hasGlowStick = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 2 should still expose a marker after the Glow Stick is crafted");
    Require(markerX == OXYGEN_CONSOLE_X && markerY == OXYGEN_CONSOLE_Y,
            "stage 2 should return the marker to the oxygen console once the Glow Stick is ready");
    Require(strstr(tasks.objective, "Glow Stick") == NULL
                && strstr(tasks.objective, "oxygen repair") != NULL,
            "stage 2 objective should stop asking for the Glow Stick once it is already crafted");
    tasks.logs[0].collected = false;
    tasks.logs[1].collected = false;
    tasks.logs[2].collected = false;
    player.hasGlowStick = false;

    Require(strcmp(Tasks_GetStageName(4), "West Confirmation") == 0,
            "stage text lookup should return the cleaner player-facing chapter title");
    Require(strcmp(Tasks_GetEndingTitle(ENDING_PEACEFUL), "Peaceful Rescue") == 0,
            "ending title lookup should still return the expected label");
    Require(strstr(Tasks_GetEndingBody(ENDING_SETTLEMENT), "wreck became a home") != NULL,
            "ending body lookup should still expose the expected settlement text");
    Require(strstr(Tasks_GetEndingBody(ENDING_FAILURE), "Collapse outran repair") != NULL,
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
    Require(strstr(tasks.communicator, "crash clue") != NULL
                || strstr(tasks.communicator, "wreck clue") != NULL,
            "stage 4 base guidance should keep crash clue progression visible");

    tasks.stage = 5;
    player.gridX = CRASH_CLUE_X;
    player.gridY = CRASH_CLUE_Y;
    Tasks_Update(&tasks, &map, &player, 0.0f);
    Require(strstr(tasks.communicator, "Protective Fiber") != NULL || strstr(tasks.communicator, "Energy Core") != NULL,
            "stage 5 communicator guidance should now explain the wreck-extraction material requirements");
    Require(strstr(tasks.communicator, "Junk Metal") != NULL || strstr(tasks.communicator, "east route") != NULL,
            "stage 5 communicator guidance should keep the east salvage run context visible");
    player.resources[RESOURCE_ENERGY_CORE] = 0;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 5 should expose an east-route marker before the core is collected");
    Require(markerX == ENERGY_CORE_NODE_X && markerY == ENERGY_CORE_NODE_Y,
            "stage 5 marker should point toward the deep-east salvage route before the extraction bundle is complete");
    player.resources[RESOURCE_JUNK_METAL] = 1;
    player.resources[RESOURCE_PROTECTIVE_FIBER] = 1;
    player.resources[RESOURCE_ENERGY_CRYSTAL] = 1;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 5 should redirect the marker back to the wreck once the extraction bundle is ready");
    Require(markerX == CRASH_CLUE_X && markerY == CRASH_CLUE_Y,
            "stage 5 marker should point back to the wreck once the extraction materials are collected");
    player.resources[RESOURCE_JUNK_METAL] = 0;
    player.resources[RESOURCE_PROTECTIVE_FIBER] = 0;
    player.resources[RESOURCE_ENERGY_CRYSTAL] = 0;
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
    player.gridX = EXTERIOR_X(70);
    player.gridY = EXTERIOR_Y(74);
    player.health = INITIAL_HEALTH;
    player.oxygen = INITIAL_OXYGEN;
    player.poison = 0.0f;
    Player_ClearAllStatuses(&player);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Forest cover") != NULL
                || strstr(tasks.communicator, "route choice") != NULL,
            "forest field note should teach cover and route choice without a crouch command");

    tasks.stage = 6;
    player.gridX = EXTERIOR_X(111);
    player.gridY = EXTERIOR_Y(48);
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

    player.gridX = EXTERIOR_X(111);
    player.gridY = EXTERIOR_Y(30);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Current Area: Spore Swamp") != NULL,
            "east-route communicator should keep the broader swamp area name stable");
    Require(strstr(tasks.communicator, "Deep Gate") != NULL,
            "communicator should expose the deep-gate location name for the east route transition");

    player.gridX = EXTERIOR_X(102);
    player.gridY = EXTERIOR_Y(62);
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
    Require(strstr(tasks.objective, "northwest ruins") != NULL
                || strstr(tasks.objective, "guardian") != NULL
                || strstr(tasks.objective, "西北遗迹") != NULL,
            "heroic route objective should now send the player toward the northwest guardian fight");
    Require(strstr(tasks.communicator, "Current Area: Ruins") != NULL,
            "north-route communicator should use the same ruins area label as the map overlay");
    Require(strstr(tasks.communicator, "Plateau") != NULL || strstr(tasks.communicator, "Signal Tower") != NULL,
            "ruins field note should explain endgame threat in the plateau route");
    Require(strstr(tasks.communicator, "Signal Tower Plateau") != NULL,
            "communicator should expose the sub-location name for the northern endgame route");

    Loc_SetLanguage(GAME_LANGUAGE_ZH_CN);
    PrepareEndingBranch(&tasks);
    tasks.logs[5].collected = false;
    tasks.logs[10].collected = false;
    tasks.logs[12].collected = false;
    tasks.bossDefeated = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "先补回林冠交接记录，再回洛希处。强行救援这条路，至少该在看清西线接力之后再被正式选下。") == 0,
            "stage 7 ending-choice objective should point to the next missing route unlock in chinese when no ending is unlocked yet");

    PrepareEndingBranch(&tasks);
    tasks.bossDefeated = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "返回洛希处，在已解锁的结局中作出选择。") == 0,
            "stage 7 ending-choice objective should stay fully localized in chinese when multiple endings are unlocked");

    PrepareEndingBranch(&tasks);
    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should remain selectable for chinese localization checks");
    tasks.bossDefeated = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "已选择强行救援路线。先在西北遗迹击败守卫，再返回信号塔。") == 0,
            "stage 7 chosen-route objective should stay fully localized in chinese");

    PrepareEndingBranch(&tasks);
    tasks.selectedEndingRoute = ENDING_SETTLEMENT;
    tasks.bossDefeated = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strcmp(tasks.objective, "已选择定居路线。先去西北遗迹击败守卫，再确认你要留下来。") == 0,
            "stage 7 settlement objective should keep pointing to the northwest ruins until the guardian is actually defeated");
    Loc_SetLanguage(GAME_LANGUAGE_EN);

    tasks.monolithsLit = 2;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.objective, "northwest ruins") != NULL
                || strstr(tasks.objective, "guardian") != NULL
                || strstr(tasks.objective, "西北遗迹") != NULL,
            "heroic route objective should still point to the northwest ruins when monolith prep is partial");
    Require(strstr(tasks.communicator, "guardian") != NULL
                || strstr(tasks.communicator, "northwest ruins") != NULL
                || strstr(tasks.communicator, "西北遗迹") != NULL,
            "stage 7 communicator should explain that partial monolith prep carries into the northwest guardian fight");

    tasks.monolithsLit = 3;
    tasks.monolithActivated[0] = true;
    tasks.monolithActivated[1] = true;
    tasks.monolithActivated[2] = true;
    tasks.monolithPuzzle.solved = true;
    player.gridX = MONOLITH_B_X;
    player.gridY = MONOLITH_B_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.objective, "northwest ruins") != NULL
                || strstr(tasks.objective, "guardian") != NULL
                || strstr(tasks.objective, "西北遗迹") != NULL,
            "heroic route objective should keep the northwest ruins as the final combat destination after monolith prep");
    Require(strstr(tasks.communicator, "Heroic route chosen") != NULL
                || strstr(tasks.communicator, "guardian") != NULL
                || strstr(tasks.communicator, "northwest ruins") != NULL
                || strstr(tasks.communicator, "西北遗迹") != NULL,
            "fully lit monolith guidance should explain the completed heroic prep payoff");

    tasks.monsterCount = 1;
    tasks.monsters[0].active = true;
    tasks.monsters[0].type = MONSTER_FINAL_BOSS;
    tasks.monsters[0].gridX = BOSS_ARENA_BOSS_X;
    tasks.monsters[0].gridY = BOSS_ARENA_BOSS_Y;
    tasks.monsters[0].area = MAP_AREA_BOSS_ARENA;
    tasks.monsters[0].maxHealth = 220.0f;
    tasks.monsters[0].health = 220.0f;
    tasks.monsters[0].currentAttack = BOSS_ATTACK_CHARGE;
    tasks.monsters[0].attackTelegraph = 1.2f;
    player.gridX = BOSS_ARENA_PLAYER_ENTRY_X;
    player.gridY = BOSS_ARENA_PLAYER_ENTRY_Y;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.objective, "Defeat the guardian") != NULL
                || strstr(tasks.objective, "northwest ruins") != NULL
                || strstr(tasks.objective, "西北遗迹") != NULL,
            "heroic route objective should switch to defeating the guardian once the player reaches the boss zone");
    Require(strstr(tasks.communicator, "Northwest Ruins") != NULL
                || strstr(tasks.communicator, "northwest ruins") != NULL
                || strstr(tasks.communicator, "西北遗迹") != NULL,
            "communicator should expose the guardian fight location once the player reaches the boss zone");
    Require(strstr(tasks.communicator, "guardian") != NULL || strstr(tasks.communicator, "oxygen margin") != NULL,
            "northwest-ruins communicator should stay focused on the boss fight once the player enters it");

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

    tasks.monsters[0].health = 60.0f;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Final Phase") != NULL
                || strstr(tasks.communicator, "last phase") != NULL
                || strstr(tasks.communicator, "最终阶段") != NULL,
            "northwest-ruins guidance should explicitly call out the last phase once the fight reaches its final stretch");

    tasks.stage = 6;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 0;
    player.gridX = PLAYER_START_X;
    player.gridY = PLAYER_START_Y;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 6 should still expose a north-route objective marker");
    Require(MatchesActiveNode(&tasks, RESOURCE_RELIC_FRAGMENT, markerX, markerY),
            "stage 6 should guide the player toward a live relic-fragment pickup before the set is complete");

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

    tasks.logs[5].collected = false;
    tasks.logs[10].collected = false;
    tasks.logs[12].collected = false;
    tasks.bossDefeated = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 7 should still expose a marker when no ending unlock is ready after the archive review");
    Require(markerX == tasks.logs[5].gridX && markerY == tasks.logs[5].gridY,
            "stage 7 should point to the next missing ending-route archive instead of sending the player back to Loxi too early");
    PrepareEndingBranch(&tasks);

    Require(Tasks_SelectEndingRoute(&tasks, ENDING_HEROIC),
            "heroic route should remain selectable for marker tests");
    tasks.monolithActivated[1] = true;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 7 should point the heroic route directly to the guardian inside the world map");
    Require(markerX == BOSS_ARENA_BOSS_X && markerY == BOSS_ARENA_BOSS_Y,
            "stage 7 heroic marker should now advance straight to the northwest boss location");

    player.gridX = BOSS_ARENA_PLAYER_ENTRY_X;
    player.gridY = BOSS_ARENA_PLAYER_ENTRY_Y;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "stage 7 should keep the marker on the guardian once the player is inside the boss zone");
    Require(markerX == BOSS_ARENA_BOSS_X && markerY == BOSS_ARENA_BOSS_Y,
            "stage 7 heroic marker should point to the in-world boss once the fight begins");

    player.gridX = WORKBENCH_X;
    player.gridY = WORKBENCH_Y;
    tasks.selectedEndingRoute = ENDING_NONE;
    PrepareEndingBranch(&tasks);
    tasks.selectedEndingRoute = ENDING_PEACEFUL;
    tasks.bossDefeated = false;
    player.resources[RESOURCE_RELIC_FRAGMENT] = 0;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "forced peaceful-route guidance should still expose a marker before the amplifier is crafted");
    Require(markerX == EXTERIOR_X(64) && markerY == EXTERIOR_Y(27),
            "forced peaceful-route guidance should point back to the ruins approach while relic fragments are still missing");
    player.resources[RESOURCE_RELIC_FRAGMENT] = 3;
    Require(Tasks_GetObjectiveMarker(&tasks, &player, &markerX, &markerY),
            "forced peaceful-route guidance should retarget the marker to the workshop once fragments are ready");
    Require(markerX == WORKBENCH_X && markerY == WORKBENCH_Y,
            "forced peaceful-route guidance should point to the workshop when the fragment set is ready but the amplifier is not built");
    player.hasSignalAmplifier = true;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Peaceful route chosen") != NULL || strstr(tasks.communicator, "Signal Amplifier") != NULL,
            "peaceful-route guidance should keep amplifier route visible");
    Require(strstr(tasks.communicator, "tower") != NULL || strstr(tasks.communicator, "Signal Amplifier") != NULL,
            "stage 7 base guidance should explain the peaceful branch-specific next step");

    player.hasSignalAmplifier = false;
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "workshop") != NULL || strstr(tasks.communicator, "fragments") != NULL,
            "peaceful-route base guidance should send the player to the workshop once the fragment set is ready");
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
    Require(strstr(tasks.communicator, "timeline") != NULL
                || strstr(tasks.communicator, "west and south") != NULL
                || strstr(tasks.communicator, "west crew trail") != NULL
                || strstr(tasks.communicator, "south facility record") != NULL
                || strstr(tasks.communicator, "fit together") != NULL
                || strstr(tasks.communicator, "connect") != NULL
                || strstr(tasks.communicator, "clear account") != NULL,
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

    player.gridX = EXTERIOR_X(41);
    player.gridY = EXTERIOR_Y(67);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Canopy Hollow") != NULL,
            "communicator should expose the Canopy Hollow location name for west W3");
    Require(strstr(tasks.communicator, "W3") == NULL,
            "Canopy Hollow guidance should not expose shorthand progression codes");

    player.gridX = EXTERIOR_X(26);
    player.gridY = EXTERIOR_Y(90);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Echo Basin") != NULL,
            "communicator should expose the Echo Basin location name for west W4");

    player.gridX = EXTERIOR_X(45);
    player.gridY = EXTERIOR_Y(84);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Last Camp") != NULL,
            "communicator should expose the Last Camp location name for west W5");

    tasks.southS3Started = false;
    tasks.southS3Completed = true;
    tasks.southS4Completed = true;
    tasks.westW5Completed = true;
    tasks.southS5Completed = true;
    player.gridX = EXTERIOR_X(106);
    player.gridY = EXTERIOR_Y(100);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Service Shafts") != NULL,
            "communicator should expose the Service Shafts location name for south S3");
    Require(strstr(tasks.communicator, "S5") == NULL && strstr(tasks.communicator, "X3") == NULL,
            "communicator should keep shorthand hidden after the final west and south passes are archived");

    player.gridX = EXTERIOR_X(117);
    player.gridY = EXTERIOR_Y(95);
    Tasks_UpdateObjective(&tasks, &player);
    Require(strstr(tasks.communicator, "Purifier Ring") != NULL,
            "communicator should expose the Purifier Ring location name for south S4");

    player.gridX = EXTERIOR_X(122);
    player.gridY = EXTERIOR_Y(102);
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
    Require((strstr(tasks.communicator, "heroic rescue") != NULL && strstr(tasks.communicator, "peaceful rescue") != NULL)
                || strstr(tasks.communicator, "choose the final route") != NULL
                || strstr(tasks.communicator, "Return to Loxi") != NULL
                || strstr(tasks.communicator, "final route") != NULL,
            "stage 7 base communicator should now explain the three-route comparison with explicit player-facing route names");

    puts("task_text smoke ok");
    return 0;
}
