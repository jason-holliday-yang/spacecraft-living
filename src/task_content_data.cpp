#include "task_content.h"

#include <array>

namespace {

const std::array<TaskNodeSeed, 71> kNodeSeeds = {{
    {RESOURCE_WOOD, EXTERIOR_X(55), EXTERIOR_Y(53), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(59), EXTERIOR_Y(66), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(67), EXTERIOR_Y(78), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(82), EXTERIOR_Y(79), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(96), EXTERIOR_Y(77), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(47), EXTERIOR_Y(59), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(52), EXTERIOR_Y(42), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(93), EXTERIOR_Y(30), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(104), EXTERIOR_Y(65), 2, 4, false},
    {RESOURCE_WOOD, EXTERIOR_X(68), EXTERIOR_Y(96), 2, 4, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(58), EXTERIOR_Y(54), 2, 4, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(62), EXTERIOR_Y(69), 2, 4, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(106), EXTERIOR_Y(56), 1, 3, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(30), EXTERIOR_Y(80), 1, 3, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(47), EXTERIOR_Y(67), 1, 3, false},
    {RESOURCE_METAL_SCRAP, EXTERIOR_X(105), EXTERIOR_Y(98), 1, 3, false},
    {RESOURCE_FRUIT, EXTERIOR_X(98), EXTERIOR_Y(48), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(107), EXTERIOR_Y(64), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(110), EXTERIOR_Y(80), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(109), EXTERIOR_Y(96), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(116), EXTERIOR_Y(103), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(45), EXTERIOR_Y(76), 2, 4, false},
    {RESOURCE_FRUIT, EXTERIOR_X(47), EXTERIOR_Y(87), 1, 3, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(70), EXTERIOR_Y(80), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(84), EXTERIOR_Y(80), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(98), EXTERIOR_Y(40), 2, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(99), EXTERIOR_Y(86), 2, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(61), EXTERIOR_Y(94), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(39), EXTERIOR_Y(82), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(43), EXTERIOR_Y(68), 1, 4, false},
    {RESOURCE_GLOW_MOSS, EXTERIOR_X(94), EXTERIOR_Y(100), 1, 4, false},
    {RESOURCE_ORE, EXTERIOR_X(73), EXTERIOR_Y(79), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(91), EXTERIOR_Y(82), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(105), EXTERIOR_Y(37), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(106), EXTERIOR_Y(38), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(76), EXTERIOR_Y(100), 1, 3, false},
    {RESOURCE_ORE, EXTERIOR_X(90), EXTERIOR_Y(98), 1, 3, false},
    {RESOURCE_SPECIAL_FUNGUS, EXTERIOR_X(100), EXTERIOR_Y(44), 1, 3, false},
    {RESOURCE_SPECIAL_FUNGUS, EXTERIOR_X(108), EXTERIOR_Y(74), 1, 3, false},
    {RESOURCE_SPECIAL_FUNGUS, EXTERIOR_X(112), EXTERIOR_Y(98), 1, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(98), EXTERIOR_Y(50), 2, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(106), EXTERIOR_Y(70), 2, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(109), EXTERIOR_Y(100), 2, 3, false},
    {RESOURCE_ALIEN_VINE, EXTERIOR_X(44), EXTERIOR_Y(83), 1, 3, false},
    {RESOURCE_SHELL_FRUIT, EXTERIOR_X(102), EXTERIOR_Y(67), 1, 3, false},
    {RESOURCE_SHELL_FRUIT, EXTERIOR_X(110), EXTERIOR_Y(103), 1, 3, false},
    {RESOURCE_SHELL_FRUIT, EXTERIOR_X(48), EXTERIOR_Y(76), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(96), EXTERIOR_Y(56), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(101), EXTERIOR_Y(90), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(113), EXTERIOR_Y(50), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(36), EXTERIOR_Y(72), 1, 3, false},
    {RESOURCE_JUNK_METAL, EXTERIOR_X(48), EXTERIOR_Y(90), 1, 2, false},
    {RESOURCE_ENERGY_CRYSTAL, EXTERIOR_X(112), EXTERIOR_Y(56), 1, 2, false},
    {RESOURCE_ENERGY_CRYSTAL, EXTERIOR_X(114), EXTERIOR_Y(86), 1, 2, false},
    {RESOURCE_ENERGY_CRYSTAL, EXTERIOR_X(120), EXTERIOR_Y(98), 1, 2, false},
    {RESOURCE_CALM_MUSHROOM, EXTERIOR_X(111), EXTERIOR_Y(71), 1, 2, false},
    {RESOURCE_CALM_MUSHROOM, EXTERIOR_X(116), EXTERIOR_Y(95), 1, 2, false},
    {RESOURCE_PROTECTIVE_FIBER, EXTERIOR_X(109), EXTERIOR_Y(81), 1, 2, false},
    {RESOURCE_PROTECTIVE_FIBER, EXTERIOR_X(114), EXTERIOR_Y(101), 1, 2, false},
    {RESOURCE_PROTECTIVE_FIBER, EXTERIOR_X(123), EXTERIOR_Y(102), 1, 2, false},
    {RESOURCE_RELIC_FRAGMENT, EXTERIOR_X(70), EXTERIOR_Y(18), 1, 2, true},
    {RESOURCE_RELIC_FRAGMENT, EXTERIOR_X(86), EXTERIOR_Y(12), 1, 2, true},
    {RESOURCE_RELIC_FRAGMENT, EXTERIOR_X(88), EXTERIOR_Y(18), 1, 2, true},
    {RESOURCE_ALIEN_SLIME, EXTERIOR_X(60), EXTERIOR_Y(23), 1, 2, true},
    {RESOURCE_ALIEN_SLIME, EXTERIOR_X(73), EXTERIOR_Y(14), 1, 2, true},
    {RESOURCE_WOOD, EXTERIOR_X(24), EXTERIOR_Y(72), 2, 4, false},
    {RESOURCE_WOOD, SHIP_CARGO_HOLD_X + 1, SHIP_CARGO_HOLD_Y + 1, 2, 0, false},
    {RESOURCE_WOOD, SHIP_CREW_QUARTERS_X + 3, SHIP_CREW_QUARTERS_Y + 1, 2, 0, false},
    {RESOURCE_GLOW_MOSS, SHIP_AIRLOCK_LINK_X + 2, SHIP_AIRLOCK_LINK_Y + 2, 3, 0, false},
    {RESOURCE_METAL_SCRAP, SHIP_WORKSHOP_X + 3, SHIP_WORKSHOP_Y + 3, 2, 0, false},
    {RESOURCE_ORE, SHIP_POWER_BAY_X + 3, SHIP_POWER_BAY_Y + 3, 1, 0, false}
}};

const std::array<TaskMonsterSeed, 8> kMonsterSeeds = {{
    {MONSTER_THORN_LARVA, EXTERIOR_X(26), EXTERIOR_Y(74), 4},
    {MONSTER_WING_BUG, EXTERIOR_X(37), EXTERIOR_Y(76), 4},
    {MONSTER_RAPTOR, EXTERIOR_X(44), EXTERIOR_Y(69), 4},
    {MONSTER_SWAMP_STALKER, EXTERIOR_X(91), EXTERIOR_Y(98), 5},
    {MONSTER_SENTINEL_JELLY, EXTERIOR_X(114), EXTERIOR_Y(98), 6},
    {MONSTER_FOG_WORM, EXTERIOR_X(121), EXTERIOR_Y(100), 6},
    {MONSTER_RELIC_GUARD, EXTERIOR_X(74), EXTERIOR_Y(18), 7},
    {MONSTER_FINAL_BOSS, EXTERIOR_X(84), EXTERIOR_Y(20), 7}
}};

const std::array<TaskLogSeed, 14> kLogSeeds = {{
    {
        SHIP_CARGO_HOLD_X + 3,
        SHIP_CARGO_HOLD_Y + 3,
        SHIP_LOG_MAINLINE,
        0,
        "Ship Log 01: Impact Protocol",
        "Day 1: Impact did not kill us. The atmosphere did the rest. Oxygen lines ruptured through three compartments, and Loxi forced every remaining watt into keeping one corridor habitable. We are alive only because the ship chose triage over comfort. If this record survives, remember the order of needs: air first, panic later.",
        "Permanent max health +6"
    },
    {
        SHIP_CREW_QUARTERS_X + 1,
        SHIP_CREW_QUARTERS_Y + 2,
        SHIP_LOG_MAINLINE,
        1,
        "Ship Log 02: Split Roster",
        "Day 3: We stopped calling it a search party and started calling it a split. One group stayed close to the ship to keep air, power, and comms alive. Another pushed west with burst antenna parts. A third marked the east edge after spotting structured stone under the swamp growth. We are not wandering. We are widening the odds that somebody learns what this planet is.",
        "Permanent attack power +4"
    },
    {
        SHIP_DIAGNOSTICS_X + 1,
        SHIP_DIAGNOSTICS_Y + 2,
        SHIP_LOG_MAINLINE,
        2,
        "Ship Log 03: Pattern, Not Wilderness",
        "Day 7: The growth rings, fog pulses, and stone harmonics repeat on a schedule. This is not wilderness in the human sense. Someone engineered a living maintenance field and then walked away before it finished learning how to heal itself. Loxi thinks the monoliths, purifier structures, and tower signal are parts of the same unfinished response.",
        "Instant oxygen +18"
    },
    {
        EXTERIOR_X(24),
        EXTERIOR_Y(63),
        SHIP_LOG_MAINLINE,
        3,
        "Field Record: West Signal Fragment 01",
        "Emergency stakes end here. Beyond this point the west trail was intentional: two crew carried burst antenna parts and left coded pings so the next runner could follow without shouting into the canopy. Their final note is blunt. Do not treat the west route like salvage. Read the path, copy the signal, and leave before the tower can triangulate you.",
        "Instant oxygen +22"
    },
    {
        EXTERIOR_X(34),
        EXTERIOR_Y(72),
        SHIP_LOG_MAINLINE,
        0,
        "Field Record: Survey Break Anchor Notes",
        "Anchor pair A/B restored. The painted arrows were reversed on purpose. Scouts went farther west with the antenna load, but the calibration case and facility timestamps were handed south to a second crew. This was not a panic split. It was a relay. If one route vanished, the other still carried the truth forward.",
        "Permanent max health +6"
    },
    {
        EXTERIOR_X(41),
        EXTERIOR_Y(68),
        SHIP_LOG_SUPPLEMENTAL,
        1,
        "Field Record: Canopy Handoff Record",
        "Observation perch still maintained after impact day plus nine. Wind vanes were trimmed, reflective tags reset, and food portions counted with almost absurd care. One entry confirms the handoff: two crew stayed west to watch the canopy, one turned back south carrying purifier notes and tower timing data. Leaving was duty. Staying was duty too.",
        "Permanent attack power +4"
    },
    {
        EXTERIOR_X(47),
        EXTERIOR_Y(72),
        SHIP_LOG_MAINLINE,
        2,
        "Field Record: Echo Basin Topology Sketch",
        "Three-point echo reconstruction complete. Last transmissions did not end in a wipeout. One courier reached the southern facility threshold and pushed through a packet linking tower behavior, vent timing, and monolith resonance. The important conclusion is almost comforting: the crews kept coordinating after the crash. We lost contact, not intent.",
        "Instant oxygen +18"
    },
    {
        EXTERIOR_X(48),
        EXTERIOR_Y(86),
        SHIP_LOG_SUPPLEMENTAL,
        1,
        "Field Record: Last Camp Testament",
        "Final camp roll call lists three positions, not one. First: light the tower and leave before the system closes again. Second: stabilize the environment so rescue does not become another collapse. Third: stay and turn survival into stewardship. No one writes as if only one answer is pure. They write as if truth should survive, even if the crew does not.",
        "Permanent attack power +4"
    },
    {
        EXTERIOR_X(94),
        EXTERIOR_Y(57),
        SHIP_LOG_MAINLINE,
        3,
        "Crash Recorder: Black Box Residue",
        "Recovered after the residue scan matched Loxi's filter tables. The eastern wreck did not simply fall here. Its final corrections were trying to avoid the same tower lattice the western scouts later mapped from the canopy. The crash clue matters because it proves the crew was already fighting a system-wide failure, not a random storm. What happened in the forest, the swamp, and the ruins was one collapse seen from different angles.",
        "Instant oxygen +22"
    },
    {
        EXTERIOR_X(80),
        EXTERIOR_Y(98),
        SHIP_LOG_MAINLINE,
        2,
        "Facility Record: Purifier Outage Memo",
        "South outer ring outage report. Toxic backflow rose after the crash because the maintenance chain failed, not because the fog suddenly became alive with hatred. The purifier ring was already unstable. The impact simply removed the people who kept it honest. Stone nodes on the north side are listed as dampers, not generators.",
        "Instant oxygen +18"
    },
    {
        EXTERIOR_X(92),
        EXTERIOR_Y(98),
        SHIP_LOG_SUPPLEMENTAL,
        0,
        "Facility Record: Vent Calibration Handover",
        "Vent gallery handover, unsigned. West-route crew promised a quiet signal window; south-route crew promised cleaner air in return. They were trying to buy each other time. The note ends with a warning that if the vents drift again, every northern push will feel harsher than it should. Environment and route pressure are the same problem viewed from different doors.",
        "Permanent max health +6"
    },
    {
        EXTERIOR_X(106),
        EXTERIOR_Y(99),
        SHIP_LOG_MAINLINE,
        1,
        "Facility Record: Service Shaft Sync Record",
        "Main shaft synchronization log. Ship systems, purifier controls, and the buried tower lattice share a maintenance backbone. Reopening these shafts does more than shorten a walk; it proves the crash site and ruins were always part of one machine room. Every restored loop turns scattered survival into deliberate access.",
        "Permanent attack power +4"
    },
    {
        EXTERIOR_X(114),
        EXTERIOR_Y(99),
        SHIP_LOG_SUPPLEMENTAL,
        3,
        "Facility Record: Purifier Ring Control Brief",
        "Control brief for purification, sealing, and oxygen recirculation. Restore the ring in that order and the whole region becomes less hostile. One buried annotation matters more than the rest: monolith endpoints are not the source of the danger. They are the field controls holding a larger instability in check. If you understand that, the tower stops looking like a weapon and starts looking like a choice.",
        "Instant oxygen +22"
    },
    {
        EXTERIOR_X(122),
        EXTERIOR_Y(102),
        SHIP_LOG_MAINLINE,
        2,
        "Facility Record: Root Vault Core Dossier",
        "Root Vault core archive. Tower signal routing, purifier control, and monolith resonance terminate in the same buried lattice. The crash did not awaken a simple enemy; it broke a maintenance network that was already struggling to contain its own world. That changes the final question. Rescue by force, rescue by understanding, or stay and inherit the work are all possible. None of them are innocent anymore, but all of them are informed.",
        "Instant oxygen +18"
    }
}};

const std::array<TaskMonsterSpec, 8> kMonsterSpecs = {{
    {28.0f, 3.0f, 8.0f,  {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {18.0f, 1.0f, 5.0f,  {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {52.0f, 6.0f, 12.0f, {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {48.0f, 5.0f, 10.0f, {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, false},
    {72.0f, 5.0f, 10.0f, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, false},
    {64.0f, 5.0f, 11.0f, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}, false},
    {92.0f, 10.0f, 15.0f,{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, false},
    {220.0f, 15.0f, 20.0f,{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, true}
}};

const std::array<TaskLogRewardSpec, 5> kLogRewardSpecs = {{
    {6.0f, 0.0f, 0.0f, 0.0f, {0}},
    {0.0f, 4.0f, 0.0f, 0.0f, {0}},
    {0.0f, 0.0f, 18.0f, 0.0f, {0}},
    {0.0f, 0.0f, 22.0f, 0.0f, {0}},
    {0.0f, 0.0f, 0.0f, 0.0f, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
}};

const std::array<EventType, EVENT_COUNT> kEventCycle = {{
    EVENT_HARVEST,
    EVENT_CALM_BEASTS,
    EVENT_CLEAR_SKY,
    EVENT_SPORE_STORM,
    EVENT_MONSTER_FRENZY,
    EVENT_DEVICE_FAULT
}};

template <typename T>
bool CopySeedAt(const T &items, int index, typename T::value_type *output) {
    if (output == NULL || index < 0 || index >= static_cast<int>(items.size())) {
        return false;
    }

    *output = items[static_cast<std::size_t>(index)];
    return true;
}

}  // namespace

int TasksContent_GetNodeSeedCount(void) {
    return static_cast<int>(kNodeSeeds.size());
}

bool TasksContent_GetNodeSeed(int index, TaskNodeSeed *seed) {
    return CopySeedAt(kNodeSeeds, index, seed);
}

int TasksContent_GetMonsterSeedCount(void) {
    return static_cast<int>(kMonsterSeeds.size());
}

bool TasksContent_GetMonsterSeed(int index, TaskMonsterSeed *seed) {
    return CopySeedAt(kMonsterSeeds, index, seed);
}

int TasksContent_GetLogSeedCount(void) {
    return static_cast<int>(kLogSeeds.size());
}

bool TasksContent_GetLogSeed(int index, TaskLogSeed *seed) {
    return CopySeedAt(kLogSeeds, index, seed);
}

bool TasksContent_GetMonsterSpec(MonsterType type, TaskMonsterSpec *spec) {
    const int monsterIndex = (int)type;

    if (spec == NULL || monsterIndex < 0 || monsterIndex >= static_cast<int>(kMonsterSpecs.size())) {
        return false;
    }

    *spec = kMonsterSpecs[static_cast<std::size_t>(monsterIndex)];
    return true;
}

bool TasksContent_GetLogRewardSpec(int rewardKind, TaskLogRewardSpec *spec) {
    if (spec == NULL || rewardKind < 0 || rewardKind >= static_cast<int>(kLogRewardSpecs.size())) {
        return false;
    }

    *spec = kLogRewardSpecs[static_cast<std::size_t>(rewardKind)];
    return true;
}

EventType TasksContent_GetDailyEvent(int dayCount) {
    if (dayCount < 0) {
        dayCount = 0;
    }

    return kEventCycle[static_cast<std::size_t>(dayCount % static_cast<int>(kEventCycle.size()))];
}
