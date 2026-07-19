#ifndef SAVE_LEGACY_RUNTIME_H
#define SAVE_LEGACY_RUNTIME_H

#include "save_system.h"

/* Shared compatibility helpers for deriving current runtime state from legacy save data. */

void SaveLegacy_ApplyDerivedSurvivalFields(SaveSnapshot *snapshot);
void SaveLegacy_SeedFallbackClearedAirlockTiles(SaveSnapshot *snapshot);

#endif
