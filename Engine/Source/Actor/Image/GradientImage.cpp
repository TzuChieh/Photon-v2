#include "Actor/Image/GradientImage.h"
#include "Core/Texture/TLinearGradientTexture.h"

namespace ph
{

GradientImage::GradientImage() :

	BinaryMixedImage(),

	m_beginUvw(0, 0, 0), 
	m_endUvw  (1, 1, 0)
{}

std::shared_ptr<TTexture<Image::ArrayType>> GradientImage::genNumericTexture(
	const CookingContext& ctx)
{
	const auto& images = checkoutImages();
	if(!images.first|| !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<Image::ArrayType>>(
		m_beginUvw, 
		images.first->genNumericTexture(ctx), 
		m_endUvw,
		images.second->genNumericTexture(ctx));
}

std::shared_ptr<TTexture<math::Spectrum>> GradientImage::genColorTexture(
	const CookingContext& ctx)
{
	const auto& images = checkoutImages();
	if(!images.first || !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<math::Spectrum>>(
		m_beginUvw,
		images.first->genColorTexture(ctx),
		m_endUvw,
		images.second->genColorTexture(ctx));
}

void GradientImage::setUvwEndpoints(const math::Vector3R& beginUvw, const math::Vector3R& endUvw)
{
	m_beginUvw = beginUvw;
	m_endUvw   = endUvw;
}

}// end namespace ph
