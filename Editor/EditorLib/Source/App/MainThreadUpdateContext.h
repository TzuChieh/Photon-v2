#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::editor
{

class MainThreadUpdateContext final
{
public:
	std::size_t frameNumber = 0;
	float64     deltaS      = 0.0;
};

}// end namespace ph::editor
