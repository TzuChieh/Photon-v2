#include "Actor/Image/GradientImage.h"
#include "DataIO/SDL/InputPacket.h"
#include "Core/Texture/TLinearGradientTexture.h"

namespace ph
{

GradientImage::GradientImage() :

	BinaryMixedImage(),

	m_beginUvw(0, 0, 0), 
	m_endUvw  (1, 1, 0)
{}

std::shared_ptr<TTexture<real>> GradientImage::genTextureReal(CookingContext& context) const
{
	const auto& images = checkoutImages();
	if(!images.first|| !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<real>>(
		m_beginUvw, 
		images.first->genTextureReal(context), 
		m_endUvw,
		images.second->genTextureReal(context));
}

std::shared_ptr<TTexture<math::Vector3R>> GradientImage::genTextureVector3R(
	CookingContext& context) const
{
	const auto& images = checkoutImages();
	if(!images.first || !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<math::Vector3R>>(
		m_beginUvw,
		images.first->genTextureVector3R(context),
		m_endUvw,
		images.second->genTextureVector3R(context));
}

std::shared_ptr<TTexture<Spectrum>> GradientImage::genTextureSpectral(
	CookingContext& context) const
{
	const auto& images = checkoutImages();
	if(!images.first || !images.second)
	{
		return nullptr;
	}

	return std::make_shared<TLinearGradientTexture<Spectrum>>(
		m_beginUvw,
		images.first->genTextureSpectral(context),
		m_endUvw,
		images.second->genTextureSpectral(context));
}

void GradientImage::setUvwEndpoints(const math::Vector3R& beginUvw, const math::Vector3R& endUvw)
{
	m_beginUvw = beginUvw;
	m_endUvw   = endUvw;
}

GradientImage::GradientImage(const InputPacket& packet) :
	BinaryMixedImage(packet)
{
	const auto beginUvw = packet.getVector3("begin-uvw", {0, 0, 0});
	const auto endUvw   = packet.getVector3("end-uvw", {1, 1, 0});

	setUvwEndpoints(beginUvw, endUvw);
}

SdlTypeInfo GradientImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "gradient");
}

void GradientImage::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<GradientImage>(packet);
		}));
}

}// end namespace ph
