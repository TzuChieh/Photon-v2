#pragma once

#include <cstddef>

namespace ph::editor
{

class MainThreadRenderUpdateContext final
{
public:
	std::size_t frameNumber     = 0;
	std::size_t frameCycleIndex = 0;
};

}// end namespace ph::editor
