#include "Actor/Image/GradientImage.h"
#include "Core/Texture/TLinearGradientTexture.h"

namespace ph
{

GradientImage::GradientImage() :

	BinaryMixedImage(),

	m_beginUvw(0, 0, 0), 
	m_endUvw  (1, 1, 0)
{}

std::shared_ptr<TTexture<real>> GradientImage::genTextureReal(ActorCookingContext& ctx) const
{
	const auto& images = checkoutImages();
	if(!images.first|| !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<real>>(
		m_beginUvw, 
		images.first->genTextureReal(ctx), 
		m_endUvw,
		images.second->genTextureReal(ctx));
}

std::shared_ptr<TTexture<math::Vector3R>> GradientImage::genTextureVector3R(
	ActorCookingContext& ctx) const
{
	const auto& images = checkoutImages();
	if(!images.first || !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<math::Vector3R>>(
		m_beginUvw,
		images.first->genTextureVector3R(ctx),
		m_endUvw,
		images.second->genTextureVector3R(ctx));
}

std::shared_ptr<TTexture<Spectrum>> GradientImage::genTextureSpectral(
	ActorCookingContext& ctx) const
{
	const auto& images = checkoutImages();
	if(!images.first || !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<Spectrum>>(
		m_beginUvw,
		images.first->genTextureSpectral(ctx),
		m_endUvw,
		images.second->genTextureSpectral(ctx));
}

void GradientImage::setUvwEndpoints(const math::Vector3R& beginUvw, const math::Vector3R& endUvw)
{
	m_beginUvw = beginUvw;
	m_endUvw   = endUvw;
}

}// end namespace ph
