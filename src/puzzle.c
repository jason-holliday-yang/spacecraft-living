#include "puzzle.h"
#include <string.h>
#include <stdio.h>

void Puzzle_Init(MonolithPuzzle *puzzle) {
    if (!puzzle) return;
    
    puzzle->active = false;
    puzzle->solved = false;
    puzzle->currentStep = 0;
    puzzle->attempts = 0;
    
    for (int i = 0; i < MAX_PUZZLE_STEPS; i++) {
        puzzle->correctOrder[i] = -1;
        puzzle->playerSequence[i] = -1;
    }
    
    snprintf(puzzle->hint, sizeof(puzzle->hint), 
        "The ancient monoliths resonate in a specific sequence. "
        "Observe their positions and the whispers of the stars.");
    
    snprintf(puzzle->solutionClue, sizeof(puzzle->solutionClue),
        "A-B-C: East, North, South");
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
    if (!puzzle || !buffer || bufferSize == 0) return;
    
    if (puzzle->solved) {
        snprintf(buffer, bufferSize, "The monoliths are fully activated and humming with power.");
        return;
    }
    
    if (puzzle->currentStep > 0 && puzzle->currentStep < MAX_PUZZLE_STEPS) {
        snprintf(buffer, bufferSize, 
            "You have activated %d monolith(s). The sequence feels %s.",
            puzzle->currentStep,
            (puzzle->currentStep == 1) ? "incomplete" : "almost complete");
        return;
    }
    
    snprintf(buffer, bufferSize, "%s", puzzle->hint);
}

void Puzzle_Reset(MonolithPuzzle *puzzle) {
    if (!puzzle) return;
    
    puzzle->currentStep = 0;
    for (int i = 0; i < MAX_PUZZLE_STEPS; i++) {
        puzzle->playerSequence[i] = -1;
    }
}

const char *Puzzle_GetStatusName(const MonolithPuzzle *puzzle) {
    if (!puzzle) return "Unknown";
    
    if (puzzle->solved) {
        return "Solved";
    }
    
    if (puzzle->active) {
        if (puzzle->currentStep == 0) {
            return "Active - Awaiting Input";
        } else if (puzzle->currentStep < MAX_PUZZLE_STEPS) {
            return "Active - In Progress";
        } else {
            return Puzzle_CheckSolved(puzzle) ? "Solved" : "Failed - Resetting";
        }
    }
    
    return "Inactive";
}
