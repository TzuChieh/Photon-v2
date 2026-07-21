#pragma once

#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/Color/color_spaces.h"
#include "Engine/Math/TArithmeticArray.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace ph
{

/*! @brief Inputs for HSV color adjustment.
Hue is an offset in normalized turns. Saturation and value are unitless multipliers, and amount is
a unitless blend weight. Mapped inputs override their paired constants.
*/
struct HsvAdjustmentTextureConfig final
{
	std::shared_ptr<TTexture<math::Spectrum>> input;
	std::shared_ptr<TTexture<real>> hueMap;
	std::shared_ptr<TTexture<real>> saturationMap;
	std::shared_ptr<TTexture<real>> valueMap;
	std::shared_ptr<TTexture<real>> amountMap;

	real hue = 0.0_r;
	real saturation = 1.0_r;
	real value = 1.0_r;
	real amount = 1.0_r;
};

/*! @brief Applies an HSV adjustment in linear sRGB.

The input spectrum is converted to linear sRGB for the HSV calculation. For spectrum output, the
adjusted RGB color is first converted to a spectrum. The original input and adjusted spectra are
then linearly blended using amount. An amount of 0 preserves the input spectrum exactly, while an
amount of 1 returns the spectrum reconstructed from the adjusted RGB color.

For numeric output, amount instead blends the original and adjusted linear-sRGB colors. RGB is
stored in the first three elements, and all remaining elements are zero. Amount is clamped to
[0, 1].
*/
template<typename OutputType>
class THsvAdjustmentTexture : public TTexture<OutputType>
{
public:
	explicit THsvAdjustmentTexture(HsvAdjustmentTextureConfig config)
		: m_config(std::move(config))
	{
		PH_ASSERT(m_config.input);
	}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(out_value);

		math::Spectrum inputColor;
		m_config.input->sample(sampleLocation, &inputColor);

		const real amount = std::clamp(sampleOr(m_config.amountMap, sampleLocation, m_config.amount), 0.0_r, 1.0_r);
		if(amount == 0.0_r)
		{
			setOriginal(inputColor, sampleLocation.expectedUsage(), out_value);
			return;
		}

		const real hue = sampleOr(m_config.hueMap, sampleLocation, m_config.hue);
		const real saturation = sampleOr(m_config.saturationMap, sampleLocation, m_config.saturation);
		const real value = sampleOr(m_config.valueMap, sampleLocation, m_config.value);

		using LinearSrgbDef = math::TColorSpaceDef<math::EColorSpace::Linear_sRGB>;
		const auto usage = sampleLocation.expectedUsage();
		const auto inputLinearSrgb = inputColor.toLinearSRGB(usage);
		auto hsvColor = LinearSrgbDef::toHSV(inputLinearSrgb);
		hsvColor[0] = wrapHue(hsvColor[0] + hue);
		hsvColor[1] = std::clamp(hsvColor[1] * saturation, 0.0_r, 1.0_r);
		hsvColor[2] *= value;

		const auto adjustedLinearSrgb = LinearSrgbDef::fromHSV(hsvColor);
		setAdjusted(inputColor, inputLinearSrgb, adjustedLinearSrgb, amount, usage, out_value);
	}

private:
	static real sampleOr(
		const std::shared_ptr<TTexture<real>>& texture,
		const SampleLocation& sampleLocation,
		const real fallback)
	{
		if(!texture)
		{
			return fallback;
		}

		real value;
		texture->sample(sampleLocation, &value);
		return value;
	}

	static real wrapHue(const real hue)
	{
		return hue - std::floor(hue);
	}

	static void setOriginal(
		const math::Spectrum& inputColor,
		const math::EColorUsage usage,
		OutputType* const out_value)
	{
		if constexpr(std::is_same_v<OutputType, math::Spectrum>)
		{
			*out_value = inputColor;
		}
		else
		{
			setNumeric(inputColor.toLinearSRGB(usage), out_value);
		}
	}

	static void setAdjusted(
		const math::Spectrum& inputColor,
		const math::TristimulusValues& inputLinearSrgb,
		const math::TristimulusValues& adjustedLinearSrgb,
		const real amount,
		const math::EColorUsage usage,
		OutputType* const out_value)
	{
		if constexpr(std::is_same_v<OutputType, math::Spectrum>)
		{
			math::Spectrum adjustedColor;
			adjustedColor.setLinearSRGB(adjustedLinearSrgb, usage);
			*out_value = inputColor.lerp(adjustedColor, amount);
		}
		else
		{
			math::TristimulusValues outputLinearSrgb;
			for(std::size_t i = 0; i < outputLinearSrgb.size(); ++i)
			{
				outputLinearSrgb[i] = inputLinearSrgb[i] * (1.0_r - amount) + adjustedLinearSrgb[i] * amount;
			}
			setNumeric(outputLinearSrgb, out_value);
		}
	}

	static void setNumeric(
		const math::TristimulusValues& linearSrgb,
		OutputType* const out_value)
	{
		static_assert(OutputType::NUM_ELEMENTS >= 3);

		out_value->set(0);
		for(std::size_t i = 0; i < linearSrgb.size(); ++i)
		{
			(*out_value)[i] = linearSrgb[i];
		}
	}

	HsvAdjustmentTextureConfig m_config;
};

}// end namespace ph
