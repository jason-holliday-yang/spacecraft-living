#include "persistence_platform.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

namespace {

namespace fs = std::filesystem;

bool CopyString(char *buffer, size_t bufferSize, const std::string &value) {
    if (buffer == NULL || bufferSize == 0) {
        return false;
    }

    buffer[0] = '\0';
    if (snprintf(buffer, bufferSize, "%s", value.c_str()) >= (int)bufferSize) {
        return false;
    }

    return true;
}

bool BuildAtomicTempPath(const char *path, char *buffer, size_t bufferSize) {
    if (path == NULL || path[0] == '\0' || buffer == NULL || bufferSize == 0) {
        return false;
    }

    buffer[0] = '\0';
    if (snprintf(buffer, bufferSize, "%s.tmp", path) >= (int)bufferSize) {
        return false;
    }

    return true;
}

}  // namespace

bool PersistencePlatform_GetAppDataDirectory(const char *appName, char *buffer, size_t bufferSize) {
    const char *home;
    fs::path targetPath;

    if (appName == NULL || appName[0] == '\0') {
        return false;
    }

    home = getenv("HOME");
    if (home == NULL || home[0] == '\0') {
        targetPath = fs::path(".") / (std::string(appName) + "Data");
    } else {
        targetPath = fs::path(home) / "Library" / "Application Support" / appName;
    }

    return CopyString(buffer, bufferSize, targetPath.lexically_normal().string());
}

bool PersistencePlatform_EnsureDirectoryTree(const char *path) {
    std::error_code error;
    fs::path targetPath;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    targetPath = fs::path(path);
    if (fs::exists(targetPath, error)) {
        return !error && fs::is_directory(targetPath, error);
    }

    error.clear();
    return fs::create_directories(targetPath, error) || (!error && fs::exists(targetPath, error));
}

bool PersistencePlatform_FileExists(const char *path) {
    std::error_code error;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    return fs::exists(fs::path(path), error) && !error;
}

bool PersistencePlatform_MoveFile(const char *sourcePath, const char *targetPath) {
    std::error_code error;

    if (sourcePath == NULL || sourcePath[0] == '\0' || targetPath == NULL || targetPath[0] == '\0') {
        return false;
    }

    fs::rename(fs::path(sourcePath), fs::path(targetPath), error);
    return !error;
}

bool PersistencePlatform_DeleteFile(const char *path) {
    std::error_code error;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    return fs::remove(fs::path(path), error) && !error;
}

bool PersistencePlatform_DeleteDirectoryTree(const char *path) {
    std::error_code error;
    fs::path targetPath;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    targetPath = fs::path(path);
    if (!fs::exists(targetPath, error)) {
        return !error;
    }
    if (error) {
        return false;
    }

    fs::remove_all(targetPath, error);
    return !error;
}

bool PersistencePlatform_WriteFileAtomically(const char *path, const void *data, size_t size) {
    FILE *file;
    char tempPath[1024];

    if (path == NULL || path[0] == '\0' || data == NULL) {
        return false;
    }

    if (!BuildAtomicTempPath(path, tempPath, sizeof(tempPath))) {
        return false;
    }

    file = fopen(tempPath, "wb");
    if (file == NULL) {
        return false;
    }

    if (fwrite(data, size, 1, file) != 1) {
        fclose(file);
        PersistencePlatform_DeleteFile(tempPath);
        return false;
    }

    if (fflush(file) != 0) {
        fclose(file);
        PersistencePlatform_DeleteFile(tempPath);
        return false;
    }

    if (fclose(file) != 0) {
        PersistencePlatform_DeleteFile(tempPath);
        return false;
    }

    if (!PersistencePlatform_MoveFile(tempPath, path)) {
        PersistencePlatform_DeleteFile(tempPath);
        return false;
    }

    return true;
}

size_t PersistencePlatform_ReadFileAtMost(const char *path, void *data, size_t dataSize, size_t *fileSizeOut) {
    FILE *file;
    long fileSize;
    size_t bytesToRead;
    size_t bytesRead;

    if (path == NULL || path[0] == '\0' || data == NULL || dataSize == 0) {
        return 0;
    }

    file = fopen(path, "rb");
    if (file == NULL) {
        return 0;
    }

    memset(data, 0, dataSize);
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return 0;
    }

    fileSize = ftell(file);
    if (fileSize < 0 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return 0;
    }

    if (fileSizeOut != NULL) {
        *fileSizeOut = (size_t)fileSize;
    }

    bytesToRead = (size_t)fileSize;
    if (bytesToRead > dataSize) {
        bytesToRead = dataSize;
    }

    bytesRead = fread(data, 1, bytesToRead, file);
    if (bytesRead != bytesToRead) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return bytesRead;
}
