#include "Actor/Image/Image.h"

namespace ph
{

Image::Image() : 
	Image(ImageParameter())
{}

Image::Image(const ImageParameter& param) : 
	m_param(param)
{}

Image::~Image() = default;

std::shared_ptr<TTexture<real>> Image::genTextureReal(
	CookingContext& context) const
{
	return genDefaultTexture<real>();
}

std::shared_ptr<TTexture<Vector3R>> Image::genTextureVector3R(
	CookingContext& context) const
{
	return genDefaultTexture<Vector3R>();
}

std::shared_ptr<TTexture<SpectralStrength>> Image::genTextureSpectral(
	CookingContext& context) const
{
	return genDefaultTexture<SpectralStrength>();
}

ImageParameter Image::getParameter() const
{
	return m_param;
}

// command interface

SdlTypeInfo Image::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "image");
}

void Image::ciRegister(CommandRegister& cmdRegister)
{
	ciRegisterMathFunctions(cmdRegister);
}

void Image::ciRegisterMathFunctions(CommandRegister& cmdRegister)
{
	// TODO
}

}// end namespace ph