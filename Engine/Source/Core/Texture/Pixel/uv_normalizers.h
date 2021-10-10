#pragma once

#include "Common/primitive_type.h"

#include <array>

namespace ph
{

template<typename NormalizerType>
concept CUVNormalizer = requires (NormalizerType normalizer, std::array<float64, 2> inputUV)
{
	{ normalizer(inputUV) } -> std::same_as<std::array<float64, 2>>
};

}// end namespace ph
