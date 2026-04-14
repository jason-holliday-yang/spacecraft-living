#include "puzzle.h"

#include "localization.h"

#include <cstdio>

void Puzzle_Init(MonolithPuzzle *puzzle) {
    if (!puzzle) {
        return;
    }

    puzzle->active = false;
    puzzle->solved = false;
    puzzle->currentStep = 0;
    puzzle->attempts = 0;

    for (int i = 0; i < MAX_PUZZLE_STEPS; i++) {
        puzzle->correctOrder[i] = -1;
        puzzle->playerSequence[i] = -1;
    }

    snprintf(
        puzzle->hint,
        sizeof(puzzle->hint),
        "%s",
        Loc_PickLiteral("The ancient monoliths resonate in a specific sequence. Observe their positions and the whispers of the stars.",
                        "古老石碑会按照特定顺序共鸣。观察它们的位置，以及群星留下的低语。"));

    snprintf(
        puzzle->solutionClue,
        sizeof(puzzle->solutionClue),
        "%s",
        Loc_PickLiteral("A-B-C: East, North, South", "A-B-C：东、北、南"));
}

bool Puzzle_TryActivate(MonolithPuzzle *puzzle, int monolithIndex) {
    if (!puzzle || !puzzle->active || puzzle->solved) {
        return false;
    }

    if (monolithIndex < 0 || monolithIndex >= MAX_PUZZLE_STEPS) {
        return false;
    }

    puzzle->playerSequence[puzzle->currentStep] = monolithIndex;
    puzzle->currentStep++;

    if (puzzle->currentStep >= MAX_PUZZLE_STEPS) {
        puzzle->attempts++;
        return Puzzle_CheckSolved(puzzle);
    }

    return true;
}

bool Puzzle_CheckSolved(const MonolithPuzzle *puzzle) {
    if (!puzzle || puzzle->currentStep < MAX_PUZZLE_STEPS) {
        return false;
    }

    for (int i = 0; i < MAX_PUZZLE_STEPS; i++) {
        if (puzzle->playerSequence[i] != puzzle->correctOrder[i]) {
            return false;
        }
    }

    return true;
}

void Puzzle_GetHint(const MonolithPuzzle *puzzle, char *buffer, size_t bufferSize) {
    if (!puzzle || !buffer || bufferSize == 0) {
        return;
    }

    if (puzzle->solved) {
        snprintf(buffer, bufferSize, "%s", Loc_PickLiteral("The monoliths are fully activated and humming with power.", "石碑已经全部激活，正持续低鸣着力量。"));
        return;
    }

    if (puzzle->currentStep > 0 && puzzle->currentStep < MAX_PUZZLE_STEPS) {
        snprintf(
            buffer,
            bufferSize,
            "%s %d %s。%s %s。",
            Loc_PickLiteral("You have activated", "你已经激活了"),
            puzzle->currentStep,
            Loc_PickLiteral("monolith(s)", "座石碑"),
            Loc_PickLiteral("The sequence feels", "当前序列看起来"),
            (puzzle->currentStep == 1) ? Loc_PickLiteral("incomplete", "仍不完整") : Loc_PickLiteral("almost complete", "已经接近完成"));
        return;
    }

    snprintf(buffer, bufferSize, "%s", puzzle->hint);
}

void Puzzle_Reset(MonolithPuzzle *puzzle) {
    if (!puzzle) {
        return;
    }

    puzzle->currentStep = 0;
    for (int i = 0; i < MAX_PUZZLE_STEPS; i++) {
        puzzle->playerSequence[i] = -1;
    }
}

const char *Puzzle_GetStatusName(const MonolithPuzzle *puzzle) {
    if (!puzzle) {
        return Loc_PickLiteral("Unknown", "未知");
    }

    if (puzzle->solved) {
        return Loc_PickLiteral("Solved", "已解开");
    }

    if (puzzle->active) {
        if (puzzle->currentStep == 0) {
            return Loc_PickLiteral("Active - Awaiting Input", "激活中 - 等待输入");
        } else if (puzzle->currentStep < MAX_PUZZLE_STEPS) {
            return Loc_PickLiteral("Active - In Progress", "激活中 - 进行中");
        } else {
            return Puzzle_CheckSolved(puzzle) ? Loc_PickLiteral("Solved", "已解开") : Loc_PickLiteral("Failed - Resetting", "失败 - 正在重置");
        }
    }

    return Loc_PickLiteral("Inactive", "未激活");
}
