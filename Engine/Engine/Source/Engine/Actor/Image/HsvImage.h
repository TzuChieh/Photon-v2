#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

struct HsvAdjustmentTextureConfig;

class HsvImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	HsvImage& setInput(std::shared_ptr<Image> image);
	HsvImage& setHue(real hue);
	HsvImage& setHueMap(std::shared_ptr<Image> map);
	HsvImage& setSaturation(real saturation);
	HsvImage& setSaturationMap(std::shared_ptr<Image> map);
	HsvImage& setValue(real value);
	HsvImage& setValueMap(std::shared_ptr<Image> map);
	HsvImage& setAmount(real amount);
	HsvImage& setAmountMap(std::shared_ptr<Image> map);

private:
	HsvAdjustmentTextureConfig makeConfig(const CookingContext& ctx) const;

	std::shared_ptr<Image> m_input;
	real m_hue;
	std::shared_ptr<Image> m_hueMap;
	real m_saturation;
	std::shared_ptr<Image> m_saturationMap;
	real m_value;
	std::shared_ptr<Image> m_valueMap;
	real m_amount;
	std::shared_ptr<Image> m_amountMap;

public:
	PH_DEFINE_SDL_CLASS(HsvImage, clazz)
	{
		clazz.typeName("hsv");
		clazz.docName("HSV Image");
		clazz.description(
			"Adjusts image hue, saturation, and value in linear sRGB. Mapped controls override "
			"paired constants. Numeric output stores the resulting linear-sRGB color in RGB order "
			"and zero in remaining channels. Color output retains the original spectral "
			"contribution according to amount.");
		clazz.baseOn<Image>();

		TSdlReference<Image, OwnerType> input("input", &OwnerType::m_input);
		input.description("Color image to adjust.");
		input.required();
		clazz.addField(input);

		TSdlReal<OwnerType> hue("hue", &OwnerType::m_hue);
		hue.description(
			"Hue offset in normalized turns. 0 is neutral, 0.25 shifts by +90 degrees, and values "
			"outside 1 turn wrap.");
		hue.defaultTo(0.0_r);
		hue.optional();
		clazz.addField(hue);

		TSdlReference<Image, OwnerType> hueMap("hue-map", &OwnerType::m_hueMap);
		hueMap.description("Hue-offset image input in normalized turns.");
		hueMap.optional();
		clazz.addField(hueMap);

		TSdlReal<OwnerType> saturation("saturation", &OwnerType::m_saturation);
		saturation.description(
			"Unitless saturation multiplier; 1 is neutral and the resulting HSV saturation is "
			"clamped to [0, 1].");
		saturation.defaultTo(1.0_r);
		saturation.optional();
		clazz.addField(saturation);

		TSdlReference<Image, OwnerType> saturationMap("saturation-map", &OwnerType::m_saturationMap);
		saturationMap.description("Saturation-multiplier image input.");
		saturationMap.optional();
		clazz.addField(saturationMap);

		TSdlReal<OwnerType> value("value", &OwnerType::m_value);
		value.description(
			"Unitless HSV value multiplier; 1 is neutral and HDR results greater than 1 are supported.");
		value.defaultTo(1.0_r);
		value.optional();
		clazz.addField(value);

		TSdlReference<Image, OwnerType> valueMap("value-map", &OwnerType::m_valueMap);
		valueMap.description("HSV-value-multiplier image input.");
		valueMap.optional();
		clazz.addField(valueMap);

		TSdlReal<OwnerType> amount("amount", &OwnerType::m_amount);
		amount.description(
			"Unitless blend weight in [0, 1]; 0 preserves the original and 1 applies the full "
			"adjustment. Values are clamped to this range.");
		amount.defaultTo(1.0_r);
		amount.optional();
		clazz.addField(amount);

		TSdlReference<Image, OwnerType> amountMap("amount-map", &OwnerType::m_amountMap);
		amountMap.description("Blend-weight image input; sampled values are clamped to [0, 1].");
		amountMap.optional();
		clazz.addField(amountMap);
	}
};

}// end namespace ph
