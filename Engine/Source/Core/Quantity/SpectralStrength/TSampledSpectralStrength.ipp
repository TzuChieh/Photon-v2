#pragma once

#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"
#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"

#include <type_traits>

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
auto TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_genLinearSrgb() const
	-> Vector3R
{
	if constexpr(std::is_same_v<TSampledSpectralStrength, SampledSpectralStrength>)
	{
		return ColorSpace::SPD_to_linear_sRGB(static_cast<const SampledSpectralStrength&>(*this));
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
auto TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_setLinearSrgb(const Vector3R& linearSrgb)
	-> void
{
	if constexpr(std::is_same_v<TSampledSpectralStrength, SampledSpectralStrength>)
	{
		ColorSpace::linear_sRGB_to_SPD(linearSrgb, static_cast<SampledSpectralStrength*>(this));
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
auto TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_setSampled(const SampledSpectralStrength& sampled)
	-> void
{
	if constexpr(std::is_same_v<TSampledSpectralStrength, SampledSpectralStrength>)
	{
		static_cast<SampledSpectralStrength&>(*this) = sampled;
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph