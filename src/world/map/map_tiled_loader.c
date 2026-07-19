#include "map_loader_internal.h"
#include "player.h"
#include "resource_path.h"
#include "task_system.h"

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
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    size = ftell(file);
    if (size <= 0) {
        fclose(file);
        return NULL;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
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

static const char *FindPreviousDataArray(const char *text, const char *namePosition) {
    const char *cursor;
    const char *lastData;

    cursor = text;
    lastData = NULL;
    while (cursor != NULL && cursor < namePosition) {
        const char *next;

        next = strstr(cursor, "\"data\"");
        if (next == NULL || next >= namePosition) {
            break;
        }
        lastData = next;
        cursor = next + 6;
    }

    if (lastData == NULL) {
        return NULL;
    }
    return strchr(lastData, '[');
}

static const char *FindLayerDataArray(const char *text, const char *layerName) {
    char needle[96];
    const char *namePosition;
    const char *scanStart;
    const char *dataPosition;

    if (snprintf(needle, sizeof(needle), "\"name\": \"%s\"", layerName) >= (int)sizeof(needle)) {
        return NULL;
    }
    namePosition = strstr(text, needle);
    if (namePosition == NULL) {
        return NULL;
    }

    scanStart = namePosition;
    while (scanStart > text && *scanStart != '{') {
        scanStart--;
    }
    dataPosition = strstr(scanStart, "\"data\"");
    if (dataPosition != NULL && dataPosition < namePosition) {
        return strchr(dataPosition, '[');
    }

    return FindPreviousDataArray(text, namePosition);
}

static bool HasNamedLayer(const char *text, const char *layerName) {
    char needle[96];

    if (text == NULL || layerName == NULL
        || snprintf(needle, sizeof(needle), "\"name\": \"%s\"", layerName) >= (int)sizeof(needle)) {
        return false;
    }
    return strstr(text, needle) != NULL;
}

static bool ReadNextInt(const char **cursor, int *value) {
    const char *p;
    int sign;
    int result;
    bool hasDigit;

    if (cursor == NULL || *cursor == NULL || value == NULL) {
        return false;
    }

    p = *cursor;
    while (*p != '\0' && *p != '-' && (*p < '0' || *p > '9')) {
        if (*p == ']') {
            return false;
        }
        p++;
    }

    sign = 1;
    if (*p == '-') {
        sign = -1;
        p++;
    }

    result = 0;
    hasDigit = false;
    while (*p >= '0' && *p <= '9') {
        hasDigit = true;
        result = result * 10 + (*p - '0');
        p++;
    }

    if (!hasDigit) {
        return false;
    }

    *value = result * sign;
    *cursor = p;
    return true;
}



static const char *FindMatchingBrace(const char *start) {
    const char *cursor;
    int depth;

    if (start == NULL || *start != '{') {
        return NULL;
    }

    cursor = start;
    depth = 0;
    while (*cursor != '\0') {
        if (*cursor == '{') {
            depth++;
        } else if (*cursor == '}') {
            depth--;
            if (depth == 0) {
                return cursor;
            }
        }
        cursor++;
    }
    return NULL;
}

static const char *FindObjectStart(const char *text, const char *position) {
    const char *cursor;

    if (text == NULL || position == NULL || position < text) {
        return NULL;
    }

    cursor = position;
    while (cursor >= text) {
        if (*cursor == '{') {
            const char *objectEnd;

            objectEnd = FindMatchingBrace(cursor);
            if (objectEnd != NULL && objectEnd >= position) {
                return cursor;
            }
        }
        if (cursor == text) {
            break;
        }
        cursor--;
    }
    return NULL;
}

static bool ParseStringFieldInRange(const char *start, const char *end, const char *fieldName, char *buffer, size_t bufferSize) {
    char needle[64];
    const char *field;
    const char *quote;
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
    quote = strchr(field + strlen(needle), ':');
    if (quote == NULL || quote >= end) {
        return false;
    }
    quote = strchr(quote, '"');
    if (quote == NULL || quote >= end) {
        return false;
    }
    valueStart = quote + 1;
    valueEnd = strchr(valueStart, '"');
    if (valueEnd == NULL || valueEnd > end) {
        return false;
    }

    length = (size_t)(valueEnd - valueStart);
    if (length >= bufferSize) {
        length = bufferSize - 1;
    }
    memcpy(buffer, valueStart, length);
    buffer[length] = '\0';
    return true;
}

static bool ParseNumberFieldInRange(const char *start, const char *end, const char *fieldName, double *value) {
    char needle[64];
    const char *field;
    const char *colon;
    char *numberEnd;

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
    *value = strtod(colon + 1, &numberEnd);
    return numberEnd != colon + 1 && numberEnd <= end;
}


static const char *FindPropertyObject(const char *start, const char *end, const char *propertyName) {
    char needle[96];
    const char *namePosition;
    const char *propertyStart;
    const char *propertyEnd;

    if (start == NULL || end == NULL || propertyName == NULL) {
        return NULL;
    }
    if (snprintf(needle, sizeof(needle), "\"name\": \"%s\"", propertyName) >= (int)sizeof(needle)) {
        return NULL;
    }
    namePosition = strstr(start, needle);
    if (namePosition == NULL || namePosition >= end) {
        return NULL;
    }
    propertyStart = FindObjectStart(start, namePosition);
    propertyEnd = FindMatchingBrace(propertyStart);
    if (propertyStart == NULL || propertyEnd == NULL || propertyStart < start || propertyEnd > end) {
        return NULL;
    }
    return propertyStart;
}


static bool ParsePropertyStringInRange(const char *start, const char *end, const char *propertyName, char *buffer, size_t bufferSize) {
    const char *propertyStart;
    const char *propertyEnd;

    if (buffer == NULL || bufferSize == 0) {
        return false;
    }
    propertyStart = FindPropertyObject(start, end, propertyName);
    propertyEnd = FindMatchingBrace(propertyStart);
    if (propertyStart == NULL || propertyEnd == NULL) {
        return false;
    }
    return ParseStringFieldInRange(propertyStart, propertyEnd, "value", buffer, bufferSize);
}

static bool ParsePropertyNumberInRange(const char *start, const char *end, const char *propertyName, int *value) {
    const char *propertyStart;
    const char *propertyEnd;
    double number;

    if (value == NULL) {
        return false;
    }
    propertyStart = FindPropertyObject(start, end, propertyName);
    propertyEnd = FindMatchingBrace(propertyStart);
    if (propertyStart == NULL || propertyEnd == NULL) {
        return false;
    }
    if (!ParseNumberFieldInRange(propertyStart, propertyEnd, "value", &number)) {
        return false;
    }
    *value = (int)number;
    return true;
}

static bool ParsePropertyBoolInRange(const char *start, const char *end, const char *propertyName, bool *value) {
    const char *propertyStart;
    const char *propertyEnd;
    const char *field;
    const char *colon;

    if (value == NULL) {
        return false;
    }
    propertyStart = FindPropertyObject(start, end, propertyName);
    propertyEnd = FindMatchingBrace(propertyStart);
    if (propertyStart == NULL || propertyEnd == NULL) {
        return false;
    }
    field = strstr(propertyStart, "\"value\"");
    if (field == NULL || field >= propertyEnd) {
        return false;
    }
    colon = strchr(field, ':');
    if (colon == NULL || colon >= propertyEnd) {
        return false;
    }
    colon++;
    while (colon < propertyEnd && (*colon == ' ' || *colon == '\t' || *colon == '\r' || *colon == '\n')) {
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

static bool AnchorFromName(const char *name, MapAnchor *anchor) {
    int index;

    if (name == NULL || anchor == NULL) {
        return false;
    }

    for (index = 0; index < MAP_ANCHOR_COUNT; index++) {
        MapAnchor candidate;

        candidate = (MapAnchor)index;
        if (strcmp(name, Map_GetAnchorName(candidate)) == 0) {
            *anchor = candidate;
            return true;
        }
    }
    return false;
}

static bool LoadAnchorObjects(const char *text, GameMap *map) {
    const char *cursor;
    bool loadedAny;

    if (text == NULL || map == NULL) {
        return false;
    }

    cursor = text;
    loadedAny = false;
    while ((cursor = strstr(cursor, "\"type\": \"anchor\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        char name[80];
        double pixelX;
        double pixelY;
        int gridX;
        int gridY;
        MapAnchor anchor;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            cursor += 1;
            continue;
        }

        if (ParseStringFieldInRange(objectStart, objectEnd, "name", name, sizeof(name))
            && ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            && ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            && AnchorFromName(name, &anchor)) {
            gridX = (int)((pixelX / (double)TILE_SIZE) + 0.5);
            gridY = (int)((pixelY / (double)TILE_SIZE) + 0.5);
            if (Map_IsWithinMapBounds(map, gridX, gridY)) {
                map->anchorSet[anchor] = true;
                map->anchorX[anchor] = gridX;
                map->anchorY[anchor] = gridY;
                loadedAny = true;
            }
        }

        cursor = objectEnd + 1;
    }

    map->hasAnchorLayer = loadedAny;
    return loadedAny;
}


static bool LoadResourceObjects(const char *text, GameMap *map) {
    const char *cursor;
    bool loadedAny;

    if (text == NULL || map == NULL) {
        return false;
    }

    cursor = text;
    loadedAny = false;
    map->resourceSeedCount = 0;
    while ((cursor = strstr(cursor, "\"type\": \"resource\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        int resourceType;
        int baseYield;
        int respawnsRemaining;
        bool special;
        int gridX;
        int gridY;
        MapResourceSeed *seed;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            cursor += 1;
            continue;
        }

        special = false;
        if (ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            && ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "resourceType", &resourceType)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "baseYield", &baseYield)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "respawnsRemaining", &respawnsRemaining)) {
            ParsePropertyBoolInRange(objectStart, objectEnd, "special", &special);
            gridX = (int)((pixelX / (double)TILE_SIZE) + 0.5);
            gridY = (int)((pixelY / (double)TILE_SIZE) + 0.5);
            if (Map_IsWithinMapBounds(map, gridX, gridY)
                && resourceType >= 0
                && resourceType < RESOURCE_COUNT
                && map->resourceSeedCount < MAX_RESOURCE_NODES) {
                seed = &map->resourceSeeds[map->resourceSeedCount++];
                seed->resourceType = resourceType;
                seed->gridX = gridX;
                seed->gridY = gridY;
                seed->baseYield = baseYield;
                seed->respawnsRemaining = respawnsRemaining;
                seed->special = special;
                loadedAny = true;
            }
        }

        cursor = objectEnd + 1;
    }

    map->hasResourceLayer = loadedAny;
    return loadedAny;
}

static bool LoadMonsterObjects(const char *text, GameMap *map) {
    const char *cursor;
    bool loadedAny;

    if (text == NULL || map == NULL) {
        return false;
    }

    cursor = text;
    loadedAny = false;
    map->monsterSeedCount = 0;
    while ((cursor = strstr(cursor, "\"type\": \"monster\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        int monsterType;
        int unlockStage;
        int gridX;
        int gridY;
        MapMonsterSeed *seed;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            cursor += 1;
            continue;
        }

        if (ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            && ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "monsterType", &monsterType)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "unlockStage", &unlockStage)) {
            gridX = (int)((pixelX / (double)TILE_SIZE) + 0.5);
            gridY = (int)((pixelY / (double)TILE_SIZE) + 0.5);
            if (Map_IsWithinMapBounds(map, gridX, gridY)
                && monsterType >= MONSTER_THORN_LARVA
                && monsterType <= MONSTER_FINAL_BOSS
                && map->monsterSeedCount < MAX_MONSTERS) {
                seed = &map->monsterSeeds[map->monsterSeedCount++];
                seed->monsterType = monsterType;
                seed->gridX = gridX;
                seed->gridY = gridY;
                seed->unlockStage = unlockStage;
                loadedAny = true;
            }
        }

        cursor = objectEnd + 1;
    }

    map->hasMonsterLayer = loadedAny;
    return loadedAny;
}

static bool LoadLogObjects(const char *text, GameMap *map) {
    const char *cursor;
    bool loadedAny;

    if (text == NULL || map == NULL) {
        return false;
    }

    cursor = text;
    loadedAny = false;
    map->logSeedCount = 0;
    while ((cursor = strstr(cursor, "\"type\": \"log\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        int sourceIndex;
        int logCategory;
        int gridX;
        int gridY;
        MapLogSeed *seed;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            cursor += 1;
            continue;
        }

        if (ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            && ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "sourceIndex", &sourceIndex)
            && ParsePropertyNumberInRange(objectStart, objectEnd, "logCategory", &logCategory)) {
            gridX = (int)((pixelX / (double)TILE_SIZE) + 0.5);
            gridY = (int)((pixelY / (double)TILE_SIZE) + 0.5);
            if (Map_IsWithinMapBounds(map, gridX, gridY)
                && sourceIndex >= 0
                && logCategory >= SHIP_LOG_MAINLINE
                && logCategory <= SHIP_LOG_SUPPLEMENTAL
                && map->logSeedCount < MAX_LOGS) {
                seed = &map->logSeeds[map->logSeedCount++];
                seed->sourceIndex = sourceIndex;
                seed->logCategory = logCategory;
                seed->gridX = gridX;
                seed->gridY = gridY;
                loadedAny = true;
            }
        }

        cursor = objectEnd + 1;
    }

    map->hasLogLayer = loadedAny;
    return loadedAny;
}



static bool LoadUnlockObjects(const char *text, GameMap *map) {
    const char *cursor;
    bool loadedAny;

    if (text == NULL || map == NULL) {
        return false;
    }

    cursor = text;
    loadedAny = false;
    map->unlockCount = 0;
    while ((cursor = strstr(cursor, "\"type\": \"unlock\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        double pixelWidth;
        double pixelHeight;
        int clearsTile;
        char unlockId[MAP_UNLOCK_ID_MAX];
        char unlockType[MAP_UNLOCK_TYPE_MAX];
        MapUnlock *unlock;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            cursor += 1;
            continue;
        }

        clearsTile = TILE_VOID;
        unlockId[0] = '\0';
        unlockType[0] = '\0';
        ParseStringFieldInRange(objectStart, objectEnd, "name", unlockId, sizeof(unlockId));
        ParsePropertyStringInRange(objectStart, objectEnd, "unlockId", unlockId, sizeof(unlockId));
        ParsePropertyStringInRange(objectStart, objectEnd, "unlockType", unlockType, sizeof(unlockType));
        ParsePropertyNumberInRange(objectStart, objectEnd, "clearsTile", &clearsTile);

        if (unlockId[0] != '\0'
            && unlockType[0] != '\0'
            && ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            && ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            && ParseNumberFieldInRange(objectStart, objectEnd, "width", &pixelWidth)
            && ParseNumberFieldInRange(objectStart, objectEnd, "height", &pixelHeight)
            && pixelWidth > 0.0
            && pixelHeight > 0.0
            && clearsTile >= TILE_VOID
            && clearsTile <= TILE_ENTITY_BLOCKER
            && map->unlockCount < MAX_MAP_UNLOCKS) {
            unlock = &map->unlocks[map->unlockCount++];
            snprintf(unlock->id, sizeof(unlock->id), "%s", unlockId);
            snprintf(unlock->type, sizeof(unlock->type), "%s", unlockType);
            unlock->gridX = (int)(pixelX / (double)TILE_SIZE);
            unlock->gridY = (int)(pixelY / (double)TILE_SIZE);
            unlock->width = (int)((pixelWidth / (double)TILE_SIZE) + 0.5);
            unlock->height = (int)((pixelHeight / (double)TILE_SIZE) + 0.5);
            unlock->clearsTile = (TileType)clearsTile;
            loadedAny = true;
        }

        cursor = objectEnd + 1;
    }

    map->hasUnlockLayer = loadedAny;
    return loadedAny;
}

static bool LoadRegionObjects(const char *text, GameMap *map) {
    const char *cursor;
    bool loadedAny;

    if (text == NULL || map == NULL) {
        return false;
    }

    cursor = text;
    loadedAny = false;
    map->regionCount = 0;
    while ((cursor = strstr(cursor, "\"type\": \"region\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        double pixelWidth;
        double pixelHeight;
        int priority;
        int areaValue;
        char regionName[MAP_REGION_NAME_MAX];
        MapRegion *region;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            cursor += 1;
            continue;
        }

        priority = 0;
        areaValue = MAP_AREA_UNKNOWN;
        regionName[0] = '\0';
        ParseStringFieldInRange(objectStart, objectEnd, "name", regionName, sizeof(regionName));
        ParsePropertyStringInRange(objectStart, objectEnd, "regionName", regionName, sizeof(regionName));
        ParsePropertyNumberInRange(objectStart, objectEnd, "priority", &priority);
        ParsePropertyNumberInRange(objectStart, objectEnd, "area", &areaValue);

        if (regionName[0] != '\0'
            && ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            && ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            && ParseNumberFieldInRange(objectStart, objectEnd, "width", &pixelWidth)
            && ParseNumberFieldInRange(objectStart, objectEnd, "height", &pixelHeight)
            && pixelWidth > 0.0
            && pixelHeight > 0.0
            && map->regionCount < MAX_MAP_REGIONS) {
            region = &map->regions[map->regionCount++];
            snprintf(region->name, sizeof(region->name), "%s", regionName);
            region->gridX = (int)(pixelX / (double)TILE_SIZE);
            region->gridY = (int)(pixelY / (double)TILE_SIZE);
            region->width = (int)((pixelWidth / (double)TILE_SIZE) + 0.5);
            region->height = (int)((pixelHeight / (double)TILE_SIZE) + 0.5);
            region->priority = priority;
            region->area = (areaValue >= MAP_AREA_BASE && areaValue <= MAP_AREA_UNKNOWN) ? (MapArea)areaValue : MAP_AREA_UNKNOWN;
            loadedAny = true;
        }

        cursor = objectEnd + 1;
    }

    map->hasRegionLayer = loadedAny;
    return loadedAny;
}

static bool PortalIdExists(const GameMap *map, const char *portalId) {
    int index;

    for (index = 0; index < map->portalCount; index++) {
        if (strcmp(map->portals[index].portalId, portalId) == 0) {
            return true;
        }
    }
    return false;
}

static bool LoadPortalObjects(const char *text, GameMap *map) {
    const char *cursor;

    if (text == NULL || map == NULL || !HasNamedLayer(text, "Portals")) {
        return false;
    }

    map->hasPortalLayer = true;
    map->portalCount = 0;
    cursor = text;
    while ((cursor = strstr(cursor, "\"type\": \"portal\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        double pixelWidth;
        double pixelHeight;
        MapPortal portal;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            return false;
        }
        memset(&portal, 0, sizeof(portal));
        portal.interactionRadius = 1;
        if (!ParsePropertyStringInRange(objectStart, objectEnd, "portalId", portal.portalId, sizeof(portal.portalId))) {
            ParseStringFieldInRange(objectStart, objectEnd, "name", portal.portalId, sizeof(portal.portalId));
        }
        ParsePropertyStringInRange(objectStart, objectEnd, "targetMapId", portal.targetMapId, sizeof(portal.targetMapId));
        ParsePropertyStringInRange(objectStart, objectEnd, "targetAnchor", portal.targetAnchor, sizeof(portal.targetAnchor));
        ParsePropertyStringInRange(objectStart, objectEnd, "transitionId", portal.transitionId, sizeof(portal.transitionId));
        ParsePropertyStringInRange(objectStart, objectEnd, "promptKey", portal.promptKey, sizeof(portal.promptKey));
        ParsePropertyStringInRange(objectStart, objectEnd, "requiresFlag", portal.requiresFlag, sizeof(portal.requiresFlag));
        ParsePropertyNumberInRange(objectStart, objectEnd, "interactionRadius", &portal.interactionRadius);

        if (portal.portalId[0] == '\0'
            || portal.targetMapId[0] == '\0'
            || portal.targetAnchor[0] == '\0'
            || portal.transitionId[0] == '\0'
            || portal.promptKey[0] == '\0'
            || portal.interactionRadius < 0
            || PortalIdExists(map, portal.portalId)
            || map->portalCount >= MAX_MAP_PORTALS
            || !ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            || !ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            || !ParseNumberFieldInRange(objectStart, objectEnd, "width", &pixelWidth)
            || !ParseNumberFieldInRange(objectStart, objectEnd, "height", &pixelHeight)
            || pixelWidth <= 0.0
            || pixelHeight <= 0.0) {
            return false;
        }
        portal.gridX = (int)(pixelX / (double)TILE_SIZE);
        portal.gridY = (int)(pixelY / (double)TILE_SIZE);
        portal.width = (int)((pixelWidth / (double)TILE_SIZE) + 0.5);
        portal.height = (int)((pixelHeight / (double)TILE_SIZE) + 0.5);
        if (!Map_IsWithinMapBounds(map, portal.gridX, portal.gridY)
            || portal.width <= 0
            || portal.height <= 0
            || portal.gridX + portal.width > map->width
            || portal.gridY + portal.height > map->height) {
            return false;
        }
        map->portals[map->portalCount++] = portal;
        cursor = objectEnd + 1;
    }
    return true;
}

static bool EntityIdExists(const GameMap *map, const char *entityId) {
    int index;

    for (index = 0; index < map->entityCount; index++) {
        if (strcmp(map->entities[index].entityId, entityId) == 0) {
            return true;
        }
    }
    return false;
}

static bool LoadEntityObjects(const char *text, GameMap *map) {
    const char *cursor;

    if (text == NULL || map == NULL || !HasNamedLayer(text, "Entities")) {
        return false;
    }

    map->hasEntityLayer = true;
    map->entityCount = 0;
    cursor = text;
    while ((cursor = strstr(cursor, "\"type\": \"entity\"")) != NULL) {
        const char *objectStart;
        const char *objectEnd;
        double pixelX;
        double pixelY;
        double pixelWidth;
        double pixelHeight;
        MapEntity entity;

        objectStart = FindObjectStart(text, cursor);
        objectEnd = FindMatchingBrace(objectStart);
        if (objectStart == NULL || objectEnd == NULL) {
            return false;
        }
        memset(&entity, 0, sizeof(entity));
        if (!ParsePropertyStringInRange(objectStart, objectEnd, "entityId", entity.entityId, sizeof(entity.entityId))) {
            ParseStringFieldInRange(objectStart, objectEnd, "name", entity.entityId, sizeof(entity.entityId));
        }
        ParsePropertyStringInRange(objectStart, objectEnd, "entityType", entity.entityType, sizeof(entity.entityType));
        ParsePropertyStringInRange(objectStart, objectEnd, "textureKey", entity.textureKey, sizeof(entity.textureKey));
        ParsePropertyStringInRange(objectStart, objectEnd, "originAnchor", entity.originAnchor, sizeof(entity.originAnchor));
        ParsePropertyStringInRange(objectStart, objectEnd, "portalId", entity.portalId, sizeof(entity.portalId));
        ParsePropertyStringInRange(objectStart, objectEnd, "drawLayer", entity.drawLayer, sizeof(entity.drawLayer));
        ParsePropertyBoolInRange(objectStart, objectEnd, "blocksMovement", &entity.blocksMovement);
        if (entity.entityId[0] == '\0'
            || entity.entityType[0] == '\0'
            || entity.textureKey[0] == '\0'
            || entity.originAnchor[0] == '\0'
            || entity.drawLayer[0] == '\0'
            || EntityIdExists(map, entity.entityId)
            || map->entityCount >= MAX_MAP_ENTITIES
            || !ParseNumberFieldInRange(objectStart, objectEnd, "x", &pixelX)
            || !ParseNumberFieldInRange(objectStart, objectEnd, "y", &pixelY)
            || !ParseNumberFieldInRange(objectStart, objectEnd, "width", &pixelWidth)
            || !ParseNumberFieldInRange(objectStart, objectEnd, "height", &pixelHeight)
            || pixelWidth <= 0.0
            || pixelHeight <= 0.0) {
            return false;
        }
        entity.gridX = (int)(pixelX / (double)TILE_SIZE);
        entity.gridY = (int)(pixelY / (double)TILE_SIZE);
        entity.width = (int)((pixelWidth / (double)TILE_SIZE) + 0.5);
        entity.height = (int)((pixelHeight / (double)TILE_SIZE) + 0.5);
        if (!Map_IsWithinMapBounds(map, entity.gridX, entity.gridY)
            || entity.width <= 0
            || entity.height <= 0
            || entity.gridX + entity.width > map->width
            || entity.gridY + entity.height > map->height) {
            return false;
        }
        map->entities[map->entityCount++] = entity;
        cursor = objectEnd + 1;
    }
    return true;
}

static bool ValidateEntityReferences(const GameMap *map) {
    int index;

    if (map == NULL) {
        return false;
    }
    for (index = 0; index < map->entityCount; index++) {
        const MapEntity *entity;
        MapAnchor originAnchor;

        entity = &map->entities[index];
        if (!AnchorFromName(entity->originAnchor, &originAnchor)
            || !map->anchorSet[originAnchor]
            || (entity->portalId[0] != '\0' && !PortalIdExists(map, entity->portalId))) {
            return false;
        }
    }
    return true;
}

static bool LoadAreaLayer(const char *text, const char *layerName, GameMap *map) {
    const char *cursor;
    int row;
    int column;

    cursor = FindLayerDataArray(text, layerName);
    if (cursor == NULL) {
        return false;
    }

    for (row = 0; row < map->height; row++) {
        for (column = 0; column < map->width; column++) {
            int gid;

            if (!ReadNextInt(&cursor, &gid)) {
                return false;
            }
            if (gid <= 0) {
                map->areaTiles[row][column] = MAP_AREA_UNKNOWN;
            } else {
                int areaValue;

                areaValue = gid - 101;
                if (areaValue < MAP_AREA_BASE || areaValue > MAP_AREA_UNKNOWN) {
                    return false;
                }
                map->areaTiles[row][column] = (MapArea)areaValue;
            }
        }
    }

    return true;
}

static bool LoadHazardLayer(const char *text, const char *layerName, GameMap *map) {
    const char *cursor;
    int row;
    int column;

    cursor = FindLayerDataArray(text, layerName);
    if (cursor == NULL) {
        return false;
    }

    for (row = 0; row < map->height; row++) {
        for (column = 0; column < map->width; column++) {
            int gid;

            if (!ReadNextInt(&cursor, &gid)) {
                return false;
            }
            if (gid <= 0) {
                map->hazardTiles[row][column] = HAZARD_NONE;
            } else {
                int hazardValue;

                hazardValue = gid - 201;
                if (hazardValue < HAZARD_NONE || hazardValue > HAZARD_POISON) {
                    return false;
                }
                map->hazardTiles[row][column] = (HazardType)hazardValue;
            }
        }
    }

    return true;
}

static bool LoadDecorLayer(const char *text, const char *layerName, GameMap *map) {
    const char *cursor;
    int row;
    int column;

    cursor = FindLayerDataArray(text, layerName);
    if (cursor == NULL) {
        return false;
    }

    for (row = 0; row < map->height; row++) {
        for (column = 0; column < map->width; column++) {
            int gid;
            int decorValue;

            if (!ReadNextInt(&cursor, &gid)) {
                return false;
            }
            if (gid <= 0) {
                map->decorTiles[row][column] = MAP_DECOR_NONE;
                continue;
            }
            decorValue = gid - 300;
            if (decorValue < MAP_DECOR_DEBRIS || decorValue > MAP_DECOR_WARNING_LIGHT) {
                return false;
            }
            map->decorTiles[row][column] = (MapDecorType)decorValue;
        }
    }

    return true;
}

static bool LoadTileLayer(const char *text, const char *layerName, GameMap *map, TileType **tiles) {
    const char *cursor;
    int row;
    int column;

    cursor = FindLayerDataArray(text, layerName);
    if (cursor == NULL) {
        return false;
    }

    for (row = 0; row < map->height; row++) {
        for (column = 0; column < map->width; column++) {
            int gid;

            if (!ReadNextInt(&cursor, &gid)) {
                return false;
            }
            if (gid <= 0) {
                tiles[row][column] = TILE_VOID;
            } else {
                int tileValue;

                tileValue = gid - 1;
                if (tileValue < TILE_VOID || tileValue > TILE_ENTITY_BLOCKER) {
                    return false;
                }
                tiles[row][column] = (TileType)tileValue;
            }
        }
    }

    return true;
}


static bool ValidateMapMetadata(const char *text, GameMap *map) {
    double width;
    double height;
    double tileWidth;
    double tileHeight;

    if (!ParseNumberFieldInRange(text, text + strlen(text), "width", &width)
        || !ParseNumberFieldInRange(text, text + strlen(text), "height", &height)
        || !ParseNumberFieldInRange(text, text + strlen(text), "tilewidth", &tileWidth)
        || !ParseNumberFieldInRange(text, text + strlen(text), "tileheight", &tileHeight)) {
        return false;
    }

    if ((int)width <= 0 || (int)height <= 0 || (int)tileWidth <= 0 || (int)tileHeight <= 0) {
        return false;
    }
    if ((int)tileWidth != TILE_SIZE || (int)tileHeight != TILE_SIZE) {
        return false;
    }
    if (!Map_Resize(map, (int)width, (int)height, (int)tileWidth, (int)tileHeight)) {
        return false;
    }
    return true;
}

bool MapInternal_LoadTiledMap(GameMap *map, const char *relativePath) {
    char *text;
    int contentVersion;

    if (map == NULL) {
        return false;
    }

    text = relativePath != NULL && relativePath[0] != '\0' ? ReadResolvedFile(relativePath) : NULL;
    if (text == NULL) {
        return false;
    }

    if (!ValidateMapMetadata(text, map)
        || !LoadTileLayer(text, "Ground", map, map->groundTiles)
        || !LoadTileLayer(text, "Props", map, map->propTiles)) {
        free(text);
        return false;
    }

    contentVersion = 1;
    ParsePropertyNumberInRange(text, text + strlen(text), "contentVersion", &contentVersion);
    map->contentVersion = contentVersion > 0 ? contentVersion : 1;

    map->hasAreaLayer = LoadAreaLayer(text, "Area", map);
    map->hasHazardLayer = LoadHazardLayer(text, "Hazard", map);
    map->hasDecorLayer = LoadDecorLayer(text, "Decor", map);
    LoadAnchorObjects(text, map);
    LoadResourceObjects(text, map);
    LoadMonsterObjects(text, map);
    LoadLogObjects(text, map);
    LoadRegionObjects(text, map);
    LoadUnlockObjects(text, map);
    map->hasAnchorLayer = HasNamedLayer(text, "Anchors");
    map->hasResourceLayer = HasNamedLayer(text, "Resources");
    map->hasMonsterLayer = HasNamedLayer(text, "Monsters");
    map->hasLogLayer = HasNamedLayer(text, "Logs");
    map->hasRegionLayer = HasNamedLayer(text, "Regions");
    map->hasUnlockLayer = HasNamedLayer(text, "Unlocks");
    if ((HasNamedLayer(text, "Portals") && !LoadPortalObjects(text, map))
        || (HasNamedLayer(text, "Entities") && !LoadEntityObjects(text, map))
        || !ValidateEntityReferences(map)) {
        free(text);
        return false;
    }
    map->loadedFromTiled = true;

    free(text);
    return true;
}
