#pragma once

#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"

namespace ph
{

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
constexpr std::size_t TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::NUM_INTERVALS;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
constexpr real TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::LAMBDA_RANGE_NM;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
constexpr real TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::LAMBDA_INTERVAL_NM;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
inline auto TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
lambdaRangeNmOf(const std::size_t index)
	-> std::pair<real, real>
{
	return {static_cast<real>(MIN_LAMBDA_NM) + static_cast<real>(index + 0) * LAMBDA_INTERVAL_NM,
	        static_cast<real>(MIN_LAMBDA_NM) + static_cast<real>(index + 1) * LAMBDA_INTERVAL_NM};
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
TSampledSpectralStrength() :
	Parent(0)
{}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
TSampledSpectralStrength(const TSampledSpectralStrength& other) :
	Parent(other)
{}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
TSampledSpectralStrength(const TArithmeticArray<real, N>& other) : 
	Parent(other)
{}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
auto TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_genRgb() const
	-> Vector3R
{
	// TODO
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
auto TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_setRgb(const Vector3R& rgb)
	-> void
{
	// TODO
}

}// end namespace ph