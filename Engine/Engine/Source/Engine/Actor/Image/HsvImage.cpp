#include "Engine/Actor/Image/HsvImage.h"

#include "Engine/Core/Texture/THsvAdjustmentTexture.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

std::shared_ptr<TTexture<Image::NumericType>> HsvImage::genNumericTexture(
	const CookingContext& ctx)
{
	return std::make_shared<THsvAdjustmentTexture<Image::NumericType>>(makeConfig(ctx));
}

std::shared_ptr<TTexture<math::Spectrum>> HsvImage::genColorTexture(
	const CookingContext& ctx)
{
	return std::make_shared<THsvAdjustmentTexture<math::Spectrum>>(makeConfig(ctx));
}

HsvAdjustmentTextureConfig HsvImage::makeConfig(const CookingContext& ctx) const
{
	PH_ASSERT(m_input);

	HsvAdjustmentTextureConfig config;
	config.input = m_input->genColorTexture(ctx);
	config.hueMap = m_hueMap ? m_hueMap->genRealTexture(ctx) : nullptr;
	config.saturationMap = m_saturationMap ? m_saturationMap->genRealTexture(ctx) : nullptr;
	config.valueMap = m_valueMap ? m_valueMap->genRealTexture(ctx) : nullptr;
	config.amountMap = m_amountMap ? m_amountMap->genRealTexture(ctx) : nullptr;
	config.hue = m_hue;
	config.saturation = m_saturation;
	config.value = m_value;
	config.amount = m_amount;
	return config;
}

HsvImage& HsvImage::setInput(std::shared_ptr<Image> image)
{
	m_input = std::move(image);
	return *this;
}

HsvImage& HsvImage::setHue(const real hue)
{
	m_hue = hue;
	return *this;
}

HsvImage& HsvImage::setHueMap(std::shared_ptr<Image> map)
{
	m_hueMap = std::move(map);
	return *this;
}

HsvImage& HsvImage::setSaturation(const real saturation)
{
	m_saturation = saturation;
	return *this;
}

HsvImage& HsvImage::setSaturationMap(std::shared_ptr<Image> map)
{
	m_saturationMap = std::move(map);
	return *this;
}

HsvImage& HsvImage::setValue(const real value)
{
	m_value = value;
	return *this;
}

HsvImage& HsvImage::setValueMap(std::shared_ptr<Image> map)
{
	m_valueMap = std::move(map);
	return *this;
}

HsvImage& HsvImage::setAmount(const real amount)
{
	m_amount = amount;
	return *this;
}

HsvImage& HsvImage::setAmountMap(std::shared_ptr<Image> map)
{
	m_amountMap = std::move(map);
	return *this;
}

}// end namespace ph
