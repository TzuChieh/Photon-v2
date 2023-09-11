#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::editor::render
{

class UpdateContext final
{
public:
	/*! @brief Current frame number on the render thread.
	*/
	uint64 frameNumber = 0;

	/*! @brief Current cycle index on the render thread.
	*/
	uint32 frameCycleIndex = 0;
};

}// end namespace ph::editor::render
