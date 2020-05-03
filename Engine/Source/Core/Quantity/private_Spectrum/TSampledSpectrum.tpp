#pragma once

#include "Core/Quantity/private_Spectrum/TSampledSpectrum.h"
#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
constexpr std::size_t TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::NUM_INTERVALS;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
constexpr real TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::LAMBDA_RANGE_NM;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
constexpr real TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::LAMBDA_INTERVAL_NM;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
inline auto TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
lambdaRangeNmOf(const std::size_t index)
	-> std::pair<real, real>
{
	return {static_cast<real>(MIN_LAMBDA_NM) + static_cast<real>(index + 0) * LAMBDA_INTERVAL_NM,
	        static_cast<real>(MIN_LAMBDA_NM) + static_cast<real>(index + 1) * LAMBDA_INTERVAL_NM};
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
inline auto TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_genLinearSrgb(const EQuantity valueType) const
	-> math::Vector3R
{
	if constexpr(std::is_same_v<TSampledSpectrum, SampledSpectrum>)
	{
		switch(valueType)
		{
		case EQuantity::EMR:
			return ColorSpace::SPD_to_linear_sRGB<ESourceHint::ILLUMINANT>(
				static_cast<const SampledSpectrum&>(*this));

		case EQuantity::ECF:
			return ColorSpace::SPD_to_linear_sRGB<ESourceHint::REFLECTANCE>(
				static_cast<const SampledSpectrum&>(*this));

		default:
			return ColorSpace::SPD_to_linear_sRGB<ESourceHint::RAW_DATA>(
				static_cast<const SampledSpectrum&>(*this));
		}
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
inline auto TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_setLinearSrgb(const math::Vector3R& linearSrgb, const EQuantity valueType)
	-> void
{
	if constexpr(std::is_same_v<TSampledSpectrum, SampledSpectrum>)
	{
		switch(valueType)
		{
		case EQuantity::EMR:
			ColorSpace::linear_sRGB_to_SPD<ESourceHint::ILLUMINANT>(
				linearSrgb, static_cast<SampledSpectrum*>(this));
			break;

		case EQuantity::ECF:
			ColorSpace::linear_sRGB_to_SPD<ESourceHint::REFLECTANCE>(
				linearSrgb, static_cast<SampledSpectrum*>(this));
			break;

		default:
			ColorSpace::linear_sRGB_to_SPD<ESourceHint::RAW_DATA>(
				linearSrgb, static_cast<SampledSpectrum*>(this));
			break;
		}
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
inline auto TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>::
impl_setSampled(const SampledSpectrum& sampled, const EQuantity valueType)
	-> void
{
	if constexpr(std::is_same_v<TSampledSpectrum, SampledSpectrum>)
	{
		static_cast<SampledSpectrum&>(*this) = sampled;
	}
	else
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph
