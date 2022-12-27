#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

enum class EMouseCode : uint8
{
	// Unknow/unsupported mouse button
	Unknown = 0,

	// Common mouse buttons
	Left   = 1,
	Right  = 2,
	Middle = 3,

	// Number of values
	Num,
};// end EMouseCode

}// end namespace ph::editor