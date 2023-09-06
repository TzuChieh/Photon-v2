#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::editor
{

class MainThreadUpdateContext final
{
public:
	/*! @brief Current frame number on the main thread.
	*/
	uint64 frameNumber = 0;

	/*! @brief Time elapsed since last update in seconds.
	*/
	float64 deltaS = 0.0;
};

}// end namespace ph::editor
