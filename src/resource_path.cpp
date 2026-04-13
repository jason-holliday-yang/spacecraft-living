#include "resource_path.h"

#include <stdio.h>
#include <string.h>
#include <filesystem>
#include <string>
#include <system_error>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace {

namespace fs = std::filesystem;

bool CopyPath(char *buffer, size_t bufferSize, const std::string &path) {
    if (buffer == NULL || bufferSize == 0) {
        return false;
    }

    buffer[0] = '\0';
    if (snprintf(buffer, bufferSize, "%s", path.c_str()) >= (int)bufferSize) {
        return false;
    }

    return true;
}

bool GetExecutablePath(fs::path *path) {
    if (path == NULL) {
        return false;
    }

#if defined(__APPLE__)
    char pathBuffer[2048];
    uint32_t size;

    size = (uint32_t)sizeof(pathBuffer);
    if (_NSGetExecutablePath(pathBuffer, &size) != 0) {
        return false;
    }
    *path = fs::path(pathBuffer);
#elif defined(_WIN32)
    char pathBuffer[2048];
    DWORD length;

    length = GetModuleFileNameA(NULL, pathBuffer, (DWORD)sizeof(pathBuffer));
    if (length == 0 || length >= sizeof(pathBuffer)) {
        return false;
    }
    pathBuffer[length] = '\0';
    *path = fs::path(pathBuffer);
#else
    char pathBuffer[2048];
    ssize_t length;

    length = readlink("/proc/self/exe", pathBuffer, sizeof(pathBuffer) - 1);
    if (length <= 0 || length >= (ssize_t)sizeof(pathBuffer)) {
        return false;
    }
    pathBuffer[length] = '\0';
    *path = fs::path(pathBuffer);
#endif

    return true;
}

bool GetExecutableDirectory(fs::path *directory) {
    fs::path executablePath;

    if (directory == NULL || !GetExecutablePath(&executablePath)) {
        return false;
    }

    *directory = executablePath.parent_path();
    return true;
}

bool CopyResolvedPath(char *buffer, size_t bufferSize, const fs::path &path) {
    std::error_code error;
    fs::path normalized;

    if (buffer == NULL || bufferSize == 0) {
        return false;
    }

    normalized = fs::weakly_canonical(path, error);
    if (error) {
        normalized = path.lexically_normal();
    }
    return CopyPath(buffer, bufferSize, normalized.string());
}

bool PathExists(const fs::path &path) {
    std::error_code error;

    return fs::exists(path, error) && !error;
}

}  // namespace

bool ResourcePath_Resolve(const char *relativePath, char *buffer, size_t bufferSize) {
    fs::path executableDir;
    fs::path searchDir;
    int depth;

    if (relativePath == NULL || relativePath[0] == '\0' || buffer == NULL || bufferSize == 0) {
        return false;
    }

    if (PathExists(relativePath)) {
        return CopyResolvedPath(buffer, bufferSize, fs::path(relativePath));
    }

    if (!GetExecutableDirectory(&executableDir)) {
        return false;
    }
    searchDir = executableDir;

    for (depth = 0; depth < 7; depth++) {
        fs::path candidate;

        candidate = searchDir / relativePath;
        if (PathExists(candidate)) {
            return CopyResolvedPath(buffer, bufferSize, candidate);
        }

        if (!searchDir.has_parent_path()) {
            break;
        }
        if (searchDir == searchDir.parent_path()) {
            break;
        }
        searchDir = searchDir.parent_path();
    }

    return false;
}
