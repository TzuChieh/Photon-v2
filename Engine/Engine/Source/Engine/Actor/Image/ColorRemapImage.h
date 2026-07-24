#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/Math/TVector3.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <memory>
#include <vector>

namespace ph
{

class ColorRemapImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;
	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	ColorRemapImage& setInput(std::shared_ptr<Image> image);
	ColorRemapImage& setRgbValues(std::vector<math::Vector3R> rgbValues);
	ColorRemapImage& setLowerExtrapolationSlope(const math::Vector3R& slope);
	ColorRemapImage& setUpperExtrapolationSlope(const math::Vector3R& slope);
	ColorRemapImage& setFactor(real factor);
	ColorRemapImage& setFactorMap(std::shared_ptr<Image> map);

private:
	std::shared_ptr<Image> m_input;
	std::vector<math::Vector3R> m_rgbValues;
	math::Vector3R m_lowerExtrapolationSlope;
	math::Vector3R m_upperExtrapolationSlope;
	real m_factor;
	std::shared_ptr<Image> m_factorMap;

public:
	PH_DEFINE_SDL_CLASS(ColorRemapImage, clazz)
	{
		clazz.typeName("color-remap");
		clazz.docName("Color Remap Image");
		clazz.description(
			"Remaps linear-sRGB color through a sampled one-dimensional RGB transfer function. "
			"The samples store only a transfer function and do not create or modify a texture "
			"file. Numeric output stores RGB in the first three elements and zero in remaining "
			"channels.");
		clazz.baseOn<Image>();

		TSdlReference<Image, OwnerType> input("input", &OwnerType::m_input);
		input.description("Color image to remap.");
		input.required();
		clazz.addField(input);

		TSdlVector3Array<OwnerType> rgbValues("rgb-values", &OwnerType::m_rgbValues);
		rgbValues.description(
			"RGB transfer values at uniformly spaced inputs over [0, 1], including both "
			"endpoints. At least two values are required and adjacent values are linearly "
			"interpolated.");
		rgbValues.required();
		clazz.addField(rgbValues);

		TSdlVector3<OwnerType> lowerExtrapolationSlope("lower-extrapolation-slope", &OwnerType::m_lowerExtrapolationSlope);
		lowerExtrapolationSlope.description(
			"RGB output derivative with respect to input below 0. Zero produces constant "
			"extension and one produces unit-slope extension.");
		lowerExtrapolationSlope.defaultTo({1, 1, 1});
		lowerExtrapolationSlope.optional();
		clazz.addField(lowerExtrapolationSlope);

		TSdlVector3<OwnerType> upperExtrapolationSlope("upper-extrapolation-slope", &OwnerType::m_upperExtrapolationSlope);
		upperExtrapolationSlope.description(
			"RGB output derivative with respect to input above 1. Zero produces constant "
			"extension and one produces unit-slope extension.");
		upperExtrapolationSlope.defaultTo({1, 1, 1});
		upperExtrapolationSlope.optional();
		clazz.addField(upperExtrapolationSlope);

		TSdlReal<OwnerType> factor("factor", &OwnerType::m_factor);
		factor.description(
			"Blend weight between original and remapped colors. 0 preserves the original and 1 "
			"applies the full remap; values are not clamped.");
		factor.defaultTo(1.0_r);
		factor.optional();
		clazz.addField(factor);

		TSdlReference<Image, OwnerType> factorMap("factor-map", &OwnerType::m_factorMap);
		factorMap.description("Blend-weight image input. Mapped values override the constant factor.");
		factorMap.optional();
		clazz.addField(factorMap);
	}
};

}// end namespace ph
