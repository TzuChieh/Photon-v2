#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::editor
{

class MainThreadRenderUpdateContext final
{
public:
	/*! @brief Current frame number on the main thread.
	*/
	uint64 frameNumber = 0;

	/*! @brief Current cycle index on the main thread.
	*/
	uint32 frameCycleIndex = 0;
};

}// end namespace ph::editor
