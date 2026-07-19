#include "map.h"
#include "resource_path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *ReadWholeFile(const char *path) {
    FILE *file;
    long size;
    char *buffer;

    file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }
    if (fseek(file, 0, SEEK_END) != 0 || (size = ftell(file)) <= 0 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }
    buffer = (char *)malloc((size_t)size + 1U);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }
    if (fread(buffer, 1, (size_t)size, file) != (size_t)size) {
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

static char *ReadResolvedFile(const char *path) {
    char resolvedPath[2048];

    if (path == NULL || path[0] == '\0') {
        return NULL;
    }
    if (ResourcePath_Resolve(path, resolvedPath, sizeof(resolvedPath))) {
        return ReadWholeFile(resolvedPath);
    }
    return ReadWholeFile(path);
}

static bool ResourceExists(const char *path) {
    char resolvedPath[2048];
    FILE *file;

    if (path == NULL || path[0] == '\0') {
        return false;
    }
    if (ResourcePath_Resolve(path, resolvedPath, sizeof(resolvedPath))) {
        return true;
    }
    file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

static const char *FindMatching(const char *start, char openCharacter, char closeCharacter) {
    const char *cursor;
    int depth;
    bool inString;
    bool escaped;

    if (start == NULL || *start != openCharacter) {
        return NULL;
    }
    depth = 0;
    inString = false;
    escaped = false;
    for (cursor = start; *cursor != '\0'; cursor++) {
        if (inString) {
            if (escaped) {
                escaped = false;
            } else if (*cursor == '\\') {
                escaped = true;
            } else if (*cursor == '"') {
                inString = false;
            }
            continue;
        }
        if (*cursor == '"') {
            inString = true;
        } else if (*cursor == openCharacter) {
            depth++;
        } else if (*cursor == closeCharacter && --depth == 0) {
            return cursor;
        }
    }
    return NULL;
}

static bool ParseStringField(const char *start,
                             const char *end,
                             const char *fieldName,
                             char *buffer,
                             size_t bufferSize) {
    char needle[80];
    const char *field;
    const char *colon;
    const char *valueStart;
    const char *valueEnd;
    size_t length;

    if (start == NULL || end == NULL || fieldName == NULL || buffer == NULL || bufferSize == 0) {
        return false;
    }
    if (snprintf(needle, sizeof(needle), "\"%s\"", fieldName) >= (int)sizeof(needle)) {
        return false;
    }
    field = strstr(start, needle);
    if (field == NULL || field >= end) {
        return false;
    }
    colon = strchr(field + strlen(needle), ':');
    if (colon == NULL || colon >= end) {
        return false;
    }
    valueStart = strchr(colon, '"');
    if (valueStart == NULL || valueStart >= end) {
        return false;
    }
    valueStart++;
    valueEnd = strchr(valueStart, '"');
    if (valueEnd == NULL || valueEnd > end) {
        return false;
    }
    length = (size_t)(valueEnd - valueStart);
    if (length == 0 || length >= bufferSize) {
        return false;
    }
    memcpy(buffer, valueStart, length);
    buffer[length] = '\0';
    return true;
}

static bool ParseBoolField(const char *start, const char *end, const char *fieldName, bool *value) {
    char needle[80];
    const char *field;
    const char *colon;

    if (start == NULL || end == NULL || fieldName == NULL || value == NULL) {
        return false;
    }
    if (snprintf(needle, sizeof(needle), "\"%s\"", fieldName) >= (int)sizeof(needle)) {
        return false;
    }
    field = strstr(start, needle);
    if (field == NULL || field >= end) {
        return false;
    }
    colon = strchr(field + strlen(needle), ':');
    if (colon == NULL || colon >= end) {
        return false;
    }
    colon++;
    while (colon < end && (*colon == ' ' || *colon == '\t' || *colon == '\r' || *colon == '\n')) {
        colon++;
    }
    if (strncmp(colon, "true", 4) == 0) {
        *value = true;
        return true;
    }
    if (strncmp(colon, "false", 5) == 0) {
        *value = false;
        return true;
    }
    return false;
}

MapKind MapKind_FromString(const char *name) {
    if (name == NULL) {
        return MAP_KIND_UNKNOWN;
    }
    if (strcmp(name, "interior") == 0) {
        return MAP_KIND_INTERIOR;
    }
    if (strcmp(name, "exterior") == 0) {
        return MAP_KIND_EXTERIOR;
    }
    if (strcmp(name, "legacy") == 0) {
        return MAP_KIND_LEGACY;
    }
    if (strcmp(name, "test") == 0) {
        return MAP_KIND_TEST;
    }
    return MAP_KIND_UNKNOWN;
}

const char *MapKind_ToString(MapKind kind) {
    switch (kind) {
        case MAP_KIND_INTERIOR:
            return "interior";
        case MAP_KIND_EXTERIOR:
            return "exterior";
        case MAP_KIND_LEGACY:
            return "legacy";
        case MAP_KIND_TEST:
            return "test";
        case MAP_KIND_UNKNOWN:
        default:
            return "unknown";
    }
}

const MapCatalogEntry *MapCatalog_Find(const MapCatalog *catalog, const char *mapId) {
    int index;

    if (catalog == NULL || mapId == NULL || mapId[0] == '\0') {
        return NULL;
    }
    for (index = 0; index < catalog->entryCount; index++) {
        if (strcmp(catalog->entries[index].id, mapId) == 0) {
            return &catalog->entries[index];
        }
    }
    return NULL;
}

const char *MapCatalog_GetLoadError(const MapCatalog *catalog) {
    if (catalog == NULL || catalog->loadError[0] == '\0') {
        return "";
    }
    return catalog->loadError;
}

bool MapCatalog_Load(MapCatalog *catalog, const char *relativePath) {
    char *text;
    const char *mapsField;
    const char *arrayStart;
    const char *arrayEnd;
    const char *cursor;

    if (catalog == NULL || relativePath == NULL || relativePath[0] == '\0') {
        return false;
    }
    memset(catalog, 0, sizeof(*catalog));
    snprintf(catalog->sourcePath, sizeof(catalog->sourcePath), "%s", relativePath);
    text = ReadResolvedFile(relativePath);
    if (text == NULL) {
        snprintf(catalog->loadError, sizeof(catalog->loadError),
                 "Unable to load map catalog: %s", relativePath);
        return false;
    }

    ParseStringField(text, text + strlen(text), "defaultMapId",
                     catalog->defaultMapId, sizeof(catalog->defaultMapId));
    mapsField = strstr(text, "\"maps\"");
    arrayStart = mapsField != NULL ? strchr(mapsField, '[') : NULL;
    arrayEnd = FindMatching(arrayStart, '[', ']');
    if (arrayStart == NULL || arrayEnd == NULL) {
        snprintf(catalog->loadError, sizeof(catalog->loadError),
                 "Map catalog %s does not contain a valid maps array", relativePath);
        free(text);
        return false;
    }

    cursor = arrayStart + 1;
    while (cursor < arrayEnd) {
        const char *objectStart;
        const char *objectEnd;
        MapCatalogEntry *entry;
        char kindName[32];
        int existingIndex;

        objectStart = strchr(cursor, '{');
        if (objectStart == NULL || objectStart >= arrayEnd) {
            break;
        }
        objectEnd = FindMatching(objectStart, '{', '}');
        if (objectEnd == NULL || objectEnd > arrayEnd || catalog->entryCount >= MAX_MAP_CATALOG_ENTRIES) {
            snprintf(catalog->loadError, sizeof(catalog->loadError),
                     "Map catalog %s contains too many or malformed entries", relativePath);
            free(text);
            return false;
        }

        entry = &catalog->entries[catalog->entryCount];
        memset(entry, 0, sizeof(*entry));
        entry->minimapEnabled = true;
        if (!ParseStringField(objectStart, objectEnd, "id", entry->id, sizeof(entry->id))
            || !ParseStringField(objectStart, objectEnd, "file", entry->file, sizeof(entry->file))
            || !ParseStringField(objectStart, objectEnd, "kind", kindName, sizeof(kindName))
            || !ParseStringField(objectStart, objectEnd, "defaultSpawn", entry->defaultSpawn, sizeof(entry->defaultSpawn))
            || !ParseStringField(objectStart, objectEnd, "respawnAnchor", entry->respawnAnchor, sizeof(entry->respawnAnchor))) {
            snprintf(catalog->loadError, sizeof(catalog->loadError),
                     "Map catalog %s contains an entry with missing required fields", relativePath);
            free(text);
            return false;
        }
        ParseBoolField(objectStart, objectEnd, "minimapEnabled", &entry->minimapEnabled);
        entry->kind = MapKind_FromString(kindName);
        if (entry->kind == MAP_KIND_UNKNOWN) {
            snprintf(catalog->loadError, sizeof(catalog->loadError),
                     "Map catalog %s contains unknown kind '%s' for map '%s'",
                     relativePath, kindName, entry->id);
            free(text);
            return false;
        }
        for (existingIndex = 0; existingIndex < catalog->entryCount; existingIndex++) {
            if (strcmp(catalog->entries[existingIndex].id, entry->id) == 0) {
                snprintf(catalog->loadError, sizeof(catalog->loadError),
                         "Map catalog %s contains duplicate map ID '%s'", relativePath, entry->id);
                free(text);
                return false;
            }
        }
        if (!ResourceExists(entry->file)) {
            snprintf(catalog->loadError, sizeof(catalog->loadError),
                     "Map catalog %s references missing map '%s' at %s",
                     relativePath, entry->id, entry->file);
            free(text);
            return false;
        }
        catalog->entryCount++;
        cursor = objectEnd + 1;
    }

    if (catalog->entryCount == 0) {
        snprintf(catalog->loadError, sizeof(catalog->loadError),
                 "Map catalog %s contains no maps", relativePath);
        free(text);
        return false;
    }
    if (catalog->defaultMapId[0] == '\0') {
        snprintf(catalog->defaultMapId, sizeof(catalog->defaultMapId), "%s", catalog->entries[0].id);
    }
    if (MapCatalog_Find(catalog, catalog->defaultMapId) == NULL) {
        snprintf(catalog->loadError, sizeof(catalog->loadError),
                 "Map catalog %s default map ID '%s' is not registered",
                 relativePath, catalog->defaultMapId);
        free(text);
        return false;
    }

    catalog->loaded = true;
    free(text);
    return true;
}
