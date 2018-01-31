#pragma once

#include "Common/primitive_type.h"
#include <cstddef>

namespace ph
{
	
template<typename ComponentType, std::size_t NUM_COMPONENTS>
class TFrame;

typedef TFrame<uint8, 3> LdrRgbFrame;
typedef TFrame<real,  3> HdrRgbFrame;

}// end namespace ph