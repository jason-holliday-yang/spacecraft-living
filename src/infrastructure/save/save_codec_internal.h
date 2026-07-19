#ifndef SAVE_CODEC_INTERNAL_H
#define SAVE_CODEC_INTERNAL_H

#include "save_internal.h"

#define SETTINGS_MAGIC_V2 "SCLSET2"
#define SETTINGS_MAGIC_V3 "SCLSET3"
#define SETTINGS_MAGIC_V4 "SCLSET4"
#define SETTINGS_MAGIC_V5 "SCLSET5"
#define SETTINGS_MAGIC_V6 "SCLSET6"
#define SETTINGS_MAGIC_CURRENT SETTINGS_MAGIC_V6
#define SAVE_MAGIC_V16 "SCLSAV16"
#define SAVE_MAGIC_V15 "SCLSAV15"
#define SAVE_MAGIC_V14 "SCLSAV14"
#define SAVE_MAGIC_V13 "SCLSAV13"
#define SAVE_MAGIC_V12 "SCLSAV12"
#define SAVE_MAGIC_V11 "SCLSAV11"
#define SAVE_MAGIC_V10 "SCLSAV10"
#define SAVE_MAGIC_V9 "SCLSAV9"
#define SAVE_MAGIC_V8 "SCLSAV8"
#define SAVE_MAGIC_V7 "SCLSAV7"
#define SAVE_MAGIC_V6 "SCLSAV6"
#define SAVE_MAGIC_V5 "SCLSAV5"
#define SAVE_MAGIC_CURRENT SAVE_MAGIC_V16
#define SAVE_NATIVE_MAX_LOGS_LEGACY 6

typedef struct SaveBufferWriter {
    uint8_t *data;
    size_t size;
    size_t offset;
    bool failed;
} SaveBufferWriter;

typedef struct SaveBufferReader {
    const uint8_t *data;
    size_t size;
    size_t offset;
    bool failed;
} SaveBufferReader;

bool SaveCodec_IsFiniteFloat(float value);

void SaveWriter_Init(SaveBufferWriter *writer, uint8_t *data, size_t size);
void SaveWriter_WriteBytes(SaveBufferWriter *writer, const void *source, size_t size);
void SaveWriter_WriteUInt8(SaveBufferWriter *writer, uint8_t value);
void SaveWriter_WriteInt16(SaveBufferWriter *writer, int16_t value);
void SaveWriter_WriteInt32(SaveBufferWriter *writer, int32_t value);
void SaveWriter_WriteUInt32(SaveBufferWriter *writer, uint32_t value);
void SaveWriter_WriteFloat(SaveBufferWriter *writer, float value);

void SaveReader_Init(SaveBufferReader *reader, const uint8_t *data, size_t size);
void SaveReader_ReadBytes(SaveBufferReader *reader, void *target, size_t size);
uint8_t SaveReader_ReadUInt8(SaveBufferReader *reader);
int16_t SaveReader_ReadInt16(SaveBufferReader *reader);
int32_t SaveReader_ReadInt32(SaveBufferReader *reader);
uint32_t SaveReader_ReadUInt32(SaveBufferReader *reader);
float SaveReader_ReadFloat(SaveBufferReader *reader);

#endif
