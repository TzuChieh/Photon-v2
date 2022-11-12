#pragma once

#include <cstddef>

namespace ph::editor
{

class MainThreadRenderUpdateContext final
{
public:
	/*! @brief Current frame number on the main thread.
	*/
	std::size_t frameNumber = 0;

	/*! @brief Current cycle index on the main thread.
	*/
	std::size_t frameCycleIndex = 0;
};

}// end namespace ph::editor
