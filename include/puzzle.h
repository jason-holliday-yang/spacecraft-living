#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdbool.h>
#include <stddef.h>
#include "c_compat.h"

/* Public lightweight puzzle state and monolith-sequence helper APIs. */

SCL_EXTERN_C_BEGIN

#define MAX_PUZZLE_STEPS 3

typedef enum PuzzleType {
    PUZZLE_NONE = 0,
    PUZZLE_MONOLITH_ORDER,
    PUZZLE_RESOURCE_COMBINATION,
    PUZZLE_POSITION_SEQUENCE
} PuzzleType;

typedef struct MonolithPuzzle {
    bool active;
    bool solved;
    int correctOrder[MAX_PUZZLE_STEPS];
    int playerSequence[MAX_PUZZLE_STEPS];
    int currentStep;
    int attempts;
    char hint[256];
    char solutionClue[128];
} MonolithPuzzle;

void Puzzle_Init(MonolithPuzzle *puzzle);
bool Puzzle_TryActivate(MonolithPuzzle *puzzle, int monolithIndex);
bool Puzzle_CheckSolved(const MonolithPuzzle *puzzle);
void Puzzle_GetHint(const MonolithPuzzle *puzzle, char *buffer, size_t bufferSize);
void Puzzle_Reset(MonolithPuzzle *puzzle);
const char *Puzzle_GetStatusName(const MonolithPuzzle *puzzle);

SCL_EXTERN_C_END

#endif
