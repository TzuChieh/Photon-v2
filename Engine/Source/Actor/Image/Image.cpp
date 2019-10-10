#include "Actor/Image/Image.h"

namespace ph
{

Image::Image()
{}

std::shared_ptr<TTexture<real>> Image::genTextureReal(
	CookingContext& context) const
{
	return genDefaultTexture<real>();
}

std::shared_ptr<TTexture<math::Vector3R>> Image::genTextureVector3R(
	CookingContext& context) const
{
	return genDefaultTexture<math::Vector3R>();
}

std::shared_ptr<TTexture<SpectralStrength>> Image::genTextureSpectral(
	CookingContext& context) const
{
	return genDefaultTexture<SpectralStrength>();
}

// command interface

Image::Image(const InputPacket& packet) :
	Image()
{}

SdlTypeInfo Image::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "image");
}

void Image::ciRegister(CommandRegister& cmdRegister)
{
	registerMathFunctions(cmdRegister);
}

void Image::registerMathFunctions(CommandRegister& cmdRegister)
{
	// TODO
}

}// end namespace ph
