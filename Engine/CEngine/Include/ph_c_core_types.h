#pragma once

// primitive data types

#include <stdint.h>
#include <stddef.h>

typedef int8_t         PhInt8;
typedef uint8_t        PhUInt8;
typedef int16_t        PhInt16;
typedef uint16_t       PhUInt16;
typedef int32_t        PhInt32;
typedef uint32_t       PhUInt32;
typedef int64_t        PhInt64;
typedef uint64_t       PhUInt64;
typedef float          PhFloat32;
typedef double         PhFloat64;
typedef char           PhChar;
typedef unsigned char  PhUChar;
typedef size_t         PhSize;

typedef enum PhBool
{
	PH_FALSE,
	PH_TRUE
} PhBool;

/*! @brief Indicates the endianness.
This type is not for detecting endianness of the current platform, but simply
for representing the endianness for operations that can make use of it.
In other words, you cannot use `PH_NATIVE_ENDIAN == PH_BIG_ENDIAN` to
determine whether the platform is in big endian.
*/
typedef enum PhEndian
{
	PH_NATIVE_ENDIAN,
	PH_LITTLE_ENDIAN,
	PH_BIG_ENDIAN
} PhEndian;

typedef enum PhBufferFormat
{
	PH_BUFFER_FORMAT_EXR_IMAGE,
	PH_BUFFER_FORMAT_FLOAT32_ARRAY
} PhBufferFormat;

typedef enum PhFrameRegionStatus
{
	PH_FRAME_REGION_STATUS_INVALID,
	PH_FRAME_REGION_STATUS_FINISHED,
	PH_FRAME_REGION_STATUS_UPDATING
} PhFrameRegionStatus;

#define PH_MAX_FRAME_DATA_NAME_SIZE 256

//typedef struct PhFrameLoadInfo
//{
//	PhSize numChannels;
//	const PhChar* channelNames[PH_MAX_FRAME_DATA_NAME_SIZE];
//} PhFrameLoadInfo;

typedef struct PhFrameSaveInfo
{
	PhSize numChannels;
	const PhChar* const* channelNames;
	PhEndian endianness;
} PhFrameSaveInfo;
