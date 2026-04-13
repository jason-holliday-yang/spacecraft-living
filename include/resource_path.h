#ifndef RESOURCE_PATH_H
#define RESOURCE_PATH_H

#include <stdbool.h>
#include <stddef.h>
#include "c_compat.h"

/* Public helper for resolving runtime resource paths with executable-relative fallbacks. */

SCL_EXTERN_C_BEGIN

bool ResourcePath_Resolve(const char *relativePath, char *buffer, size_t bufferSize);

SCL_EXTERN_C_END

#endif
