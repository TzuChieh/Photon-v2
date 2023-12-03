#pragma once

#include "Common/primitive_type.h"

#include <cstddef>

namespace ph
{
	
template<typename T, std::size_t N>
class TFrame;

using LdrComponent = uint8;
using HdrComponent = float32;

using LdrRgbFrame = TFrame<LdrComponent, 3>;
using HdrRgbFrame = TFrame<HdrComponent, 3>;

}// end namespace ph
