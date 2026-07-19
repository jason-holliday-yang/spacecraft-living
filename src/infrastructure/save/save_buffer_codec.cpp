#include "save_codec_internal.h"

#include <cfloat>
#include <cstring>

bool SaveCodec_IsFiniteFloat(float value) {
    return value <= FLT_MAX && value >= -FLT_MAX;
}

void SaveWriter_Init(SaveBufferWriter *writer, uint8_t *data, size_t size) {
    writer->data = data;
    writer->size = size;
    writer->offset = 0;
    writer->failed = false;
}

void SaveWriter_WriteBytes(SaveBufferWriter *writer, const void *source, size_t size) {
    if (writer->failed || source == NULL || writer->offset + size > writer->size) {
        writer->failed = true;
        return;
    }

    std::memcpy(writer->data + writer->offset, source, size);
    writer->offset += size;
}

void SaveWriter_WriteUInt8(SaveBufferWriter *writer, uint8_t value) {
    SaveWriter_WriteBytes(writer, &value, sizeof(value));
}

void SaveWriter_WriteInt16(SaveBufferWriter *writer, int16_t value) {
    uint16_t unsignedValue;
    uint8_t bytes[2];

    unsignedValue = (uint16_t)value;
    bytes[0] = (uint8_t)(unsignedValue & 0xFFu);
    bytes[1] = (uint8_t)((unsignedValue >> 8) & 0xFFu);
    SaveWriter_WriteBytes(writer, bytes, sizeof(bytes));
}

void SaveWriter_WriteInt32(SaveBufferWriter *writer, int32_t value) {
    uint32_t unsignedValue;
    uint8_t bytes[4];

    unsignedValue = (uint32_t)value;
    bytes[0] = (uint8_t)(unsignedValue & 0xFFu);
    bytes[1] = (uint8_t)((unsignedValue >> 8) & 0xFFu);
    bytes[2] = (uint8_t)((unsignedValue >> 16) & 0xFFu);
    bytes[3] = (uint8_t)((unsignedValue >> 24) & 0xFFu);
    SaveWriter_WriteBytes(writer, bytes, sizeof(bytes));
}

void SaveWriter_WriteUInt32(SaveBufferWriter *writer, uint32_t value) {
    uint8_t bytes[4];

    bytes[0] = (uint8_t)(value & 0xFFu);
    bytes[1] = (uint8_t)((value >> 8) & 0xFFu);
    bytes[2] = (uint8_t)((value >> 16) & 0xFFu);
    bytes[3] = (uint8_t)((value >> 24) & 0xFFu);
    SaveWriter_WriteBytes(writer, bytes, sizeof(bytes));
}

void SaveWriter_WriteFloat(SaveBufferWriter *writer, float value) {
    uint32_t bits;

    std::memcpy(&bits, &value, sizeof(bits));
    SaveWriter_WriteUInt32(writer, bits);
}

void SaveReader_Init(SaveBufferReader *reader, const uint8_t *data, size_t size) {
    reader->data = data;
    reader->size = size;
    reader->offset = 0;
    reader->failed = false;
}

void SaveReader_ReadBytes(SaveBufferReader *reader, void *target, size_t size) {
    if (reader->failed || target == NULL || reader->offset + size > reader->size) {
        reader->failed = true;
        return;
    }

    std::memcpy(target, reader->data + reader->offset, size);
    reader->offset += size;
}

uint8_t SaveReader_ReadUInt8(SaveBufferReader *reader) {
    uint8_t value;

    value = 0;
    SaveReader_ReadBytes(reader, &value, sizeof(value));
    return value;
}

int16_t SaveReader_ReadInt16(SaveBufferReader *reader) {
    uint8_t bytes[2];
    uint16_t unsignedValue;

    std::memset(bytes, 0, sizeof(bytes));
    SaveReader_ReadBytes(reader, bytes, sizeof(bytes));
    unsignedValue = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
    return (int16_t)unsignedValue;
}

int32_t SaveReader_ReadInt32(SaveBufferReader *reader) {
    uint8_t bytes[4];
    uint32_t unsignedValue;
    int32_t value;

    std::memset(bytes, 0, sizeof(bytes));
    SaveReader_ReadBytes(reader, bytes, sizeof(bytes));
    unsignedValue = (uint32_t)bytes[0]
        | ((uint32_t)bytes[1] << 8)
        | ((uint32_t)bytes[2] << 16)
        | ((uint32_t)bytes[3] << 24);
    std::memcpy(&value, &unsignedValue, sizeof(value));
    return value;
}

uint32_t SaveReader_ReadUInt32(SaveBufferReader *reader) {
    uint8_t bytes[4];

    std::memset(bytes, 0, sizeof(bytes));
    SaveReader_ReadBytes(reader, bytes, sizeof(bytes));
    return (uint32_t)bytes[0]
        | ((uint32_t)bytes[1] << 8)
        | ((uint32_t)bytes[2] << 16)
        | ((uint32_t)bytes[3] << 24);
}

float SaveReader_ReadFloat(SaveBufferReader *reader) {
    uint32_t bits;
    float value;

    bits = SaveReader_ReadUInt32(reader);
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
