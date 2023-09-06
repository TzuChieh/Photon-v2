#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::editor
{

class GHIThreadUpdateContext final
{
public:
	/*! @brief Current frame number on the GHI thread.
	*/
	uint64 frameNumber = 0;
};

}// end namespace ph::editor
