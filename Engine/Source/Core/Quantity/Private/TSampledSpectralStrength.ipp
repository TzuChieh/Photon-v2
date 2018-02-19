#pragma once

#include "Core/Quantity/Private/TSampledSpectralStrength.h"

namespace ph
{

template<std::size_t N>
TSampledSpectralStrength<N>::TSampledSpectralStrength(const TSampledSpectralStrength& other) : 
	Parent(other)
{}

template<std::size_t N>
TSampledSpectralStrength<N>::TSampledSpectralStrength(const TArithmeticArray<real, N>& other) : 
	Parent(other)
{}

template<std::size_t N>
Vector3R TSampledSpectralStrength<N>::impl_genRgb() const
{
	// TODO
}

template<std::size_t N>
void TSampledSpectralStrength<N>::impl_setRgb(const Vector3R& rgb)
{
	// TODO
}

}// end namespace ph