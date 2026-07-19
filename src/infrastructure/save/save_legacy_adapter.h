#ifndef SAVE_LEGACY_ADAPTER_H
#define SAVE_LEGACY_ADAPTER_H

#include <stdbool.h>
#include <stddef.h>

#include "save_system.h"

/* Internal compatibility bridge for decoding historical save-file layouts. */

SCL_EXTERN_C_BEGIN

bool SaveLegacyAdapter_IsSupportedMagic(const char *magic);
bool SaveLegacyAdapter_LoadSnapshot(const char *path, const char *magic, size_t fileSize, SaveSnapshot *snapshot);

SCL_EXTERN_C_END

#endif
