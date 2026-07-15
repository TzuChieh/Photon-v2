#include "Engine/Actor/Image/LuminanceImage.h"
#include "Engine/Core/Texture/TLuminanceTexture.h"
#include "Engine/Core/Texture/constant_textures.h"

#include <utility>

namespace ph
{

std::shared_ptr<TTexture<Image::NumericType>> LuminanceImage::genNumericTexture(
	const CookingContext& ctx)
{
	if(!m_input)
	{
		return std::make_shared<TConstantTexture<Image::NumericType>>(Image::NumericType(0));
	}

	return std::make_shared<TLuminanceTexture<Image::NumericType>>(
		m_input->genColorTexture(ctx));
}

std::shared_ptr<TTexture<math::Spectrum>> LuminanceImage::genColorTexture(
	const CookingContext& ctx)
{
	if(!m_input)
	{
		return std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum(0));
	}

	return std::make_shared<TLuminanceTexture<math::Spectrum>>(
		m_input->genColorTexture(ctx));
}

std::shared_ptr<TTexture<real>> LuminanceImage::genRealTexture(const CookingContext& ctx)
{
	if(!m_input)
	{
		return std::make_shared<TConstantTexture<real>>(0.0_r);
	}

	return std::make_shared<TLuminanceTexture<real>>(m_input->genColorTexture(ctx));
}

LuminanceImage& LuminanceImage::setInput(std::shared_ptr<Image> image)
{
	m_input = std::move(image);
	return *this;
}

const std::shared_ptr<Image>& LuminanceImage::getInput() const
{
	return m_input;
}

}// end namespace ph
