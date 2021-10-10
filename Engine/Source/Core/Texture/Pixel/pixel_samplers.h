#pragma once

namespace ph
{

template<typename SamplerType>
concept CPixelSampler = requires (SamplerType sampler, std::array<float64, 2> inputUV)
{
	{ normalizer(inputUV) } -> std::same_as<std::array<float64, 2>>
};

}// end namespace ph
