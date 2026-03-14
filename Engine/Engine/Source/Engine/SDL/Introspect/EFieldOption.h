#pragma once

#include "Engine/Math/math.h"
#include "Engine/Utility/TBitFlags.h"

#include <Common/primitive_type.h>

namespace ph
{

enum class EFieldOption : uint32
{
	/*! No flags set. */
	None = 0,

	/*!
	Whether the field want to disable the built-in fallback mechanism to handle I/O problems.
	An example of this is default field value. With fallback enabled, the field may set itself to
	the default value supplied (if available) on error. By default, fallback is enabled.
	*/
	DisableFallback = math::flag_bit<uint32, 0>(),

	/*! Prefer direct memory access if supported. You are expected to validate data yourself with
	this option set. This will disable internal fallback mechanisms and some error handling to
	give native access more flexibility and speed. */
	PreferNativeAccess = math::flag_bit<uint32, 1>(),
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EFieldOption);

using FieldOptions = TEnumFlags<EFieldOption>;

}// end namespace ph
