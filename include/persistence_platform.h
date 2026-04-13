#ifndef PERSISTENCE_PLATFORM_H
#define PERSISTENCE_PLATFORM_H

#include <stdbool.h>
#include <stddef.h>

#include "c_compat.h"

/* Public C ABI for platform file/path helpers used by save and settings systems. */

SCL_EXTERN_C_BEGIN

bool PersistencePlatform_GetAppDataDirectory(const char *appName, char *buffer, size_t bufferSize);
bool PersistencePlatform_EnsureDirectoryTree(const char *path);
bool PersistencePlatform_FileExists(const char *path);
bool PersistencePlatform_MoveFile(const char *sourcePath, const char *targetPath);
bool PersistencePlatform_DeleteFile(const char *path);
bool PersistencePlatform_DeleteDirectoryTree(const char *path);
bool PersistencePlatform_WriteFileAtomically(const char *path, const void *data, size_t size);
size_t PersistencePlatform_ReadFileAtMost(const char *path, void *data, size_t dataSize, size_t *fileSizeOut);

SCL_EXTERN_C_END

#endif
