#pragma once

#include "Math/TArithmeticArray.h"

#include <cstddef>

namespace ph
{

template<typename T, std::size_t N>
using TTexPixel = math::TArithmeticArray<T, N>;

}// end namespace ph
