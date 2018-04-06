#pragma once

#include "Common/primitive_type.h"
#include <cstddef>

namespace ph
{
	
template<typename T, std::size_t N>
class TFrame;

typedef uint8   LdrComponent;
typedef float32 HdrComponent;

typedef TFrame<LdrComponent, 3> LdrRgbFrame;
typedef TFrame<HdrComponent, 3> HdrRgbFrame;

}// end namespace ph