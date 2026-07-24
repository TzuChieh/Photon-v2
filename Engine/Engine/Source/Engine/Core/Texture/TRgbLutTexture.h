#pragma once

#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/Color/color_spaces.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace ph
{

/*! @brief Applies an RGB lookup in a specified tristimulus color space.

`RgbLutType` owns the lookup representation and maps tristimulus values via `operator()`. Factor
blends the original and remapped results: 0 keeps the original and 1 uses the remapped result. A
factor texture overrides the constant; factor values are not clamped.
*/
template<math::EColorSpace COLOR_SPACE, typename OutputType, typename RgbLutType>
class TRgbLutTexture : public TTexture<OutputType>
{
public:
	static_assert(
		math::TColorSpaceDef<COLOR_SPACE>::isTristimulus(),
		"TRgbLutTexture requires a tristimulus color space.");
	static_assert(
		std::is_same_v<OutputType, math::Spectrum> ||
		std::is_same_v<OutputType, math::TristimulusValues>,
		"TRgbLutTexture outputs Spectrum or TristimulusValues.");
	static_assert(
		requires (
			const RgbLutType& rgbLut,
			const math::TristimulusValues& tristimulus)
		{
			{ rgbLut(tristimulus) } -> std::same_as<math::TristimulusValues>;
		},
		"RgbLutType must map tristimulus values to tristimulus values.");

	TRgbLutTexture(
		std::shared_ptr<TTexture<math::Spectrum>> input,
		RgbLutType rgbLut,
		real factor,
		std::shared_ptr<TTexture<real>> factorMap = nullptr)
		: m_input(std::move(input))
		, m_rgbLut(std::move(rgbLut))
		, m_factor(factor)
		, m_factorMap(std::move(factorMap))
	{
		PH_ASSERT(m_input);
	}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(out_value);

		math::Spectrum inputColor;
		m_input->sample(sampleLocation, &inputColor);

		const auto usage = sampleLocation.expectedUsage();
		const real factor = sampleFactor(sampleLocation);
		const auto inputTristimulus = inputColor.template toTransformed<COLOR_SPACE>(usage);
		const auto remappedTristimulus = m_rgbLut(inputTristimulus);
		if constexpr(std::is_same_v<OutputType, math::Spectrum>)
		{
			setOutput(inputColor, remappedTristimulus, factor, usage, out_value);
		}
		else
		{
			setOutput(inputTristimulus, remappedTristimulus, factor, out_value);
		}
	}

private:
	real sampleFactor(const SampleLocation& sampleLocation) const
	{
		if(!m_factorMap)
		{
			return m_factor;
		}

		real factor;
		m_factorMap->sample(sampleLocation, &factor);
		return factor;
	}

	static void setOutput(
		const math::Spectrum& inputColor,
		const math::TristimulusValues& remappedTristimulus,
		const real factor,
		const math::EColorUsage usage,
		math::Spectrum* const out_value)
	{
		math::Spectrum remappedColor;
		remappedColor.setTransformed<COLOR_SPACE>(remappedTristimulus, usage);
		*out_value = inputColor.lerp(remappedColor, factor);
	}

	static void setOutput(
		const math::TristimulusValues& inputTristimulus,
		const math::TristimulusValues& remappedTristimulus,
		const real factor,
		math::TristimulusValues* const out_value)
	{
		for(std::size_t ci = 0; ci < out_value->size(); ++ci)
		{
			(*out_value)[ci] = inputTristimulus[ci] + (remappedTristimulus[ci] - inputTristimulus[ci]) * factor;
		}
	}

	std::shared_ptr<TTexture<math::Spectrum>> m_input;
	RgbLutType m_rgbLut;
	real m_factor;
	std::shared_ptr<TTexture<real>> m_factorMap;
};

}// end namespace ph
