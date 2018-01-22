#include "Actor/Image/Image.h"

namespace ph
{

Image::~Image() = default;

void Image::genTextureReal(
	CookingContext& context,
	std::shared_ptr<TTexture<real>>* const out_texture) const
{
	*out_texture = genDefaultTexture<real>();
}

void Image::genTextureVector3R(
	CookingContext& context,
	std::shared_ptr<TTexture<Vector3R>>* const out_texture) const
{
	*out_texture = genDefaultTexture<Vector3R>();
}

void Image::genTextureSpectral(
	CookingContext& context,
	std::shared_ptr<TTexture<SpectralStrength>>* const out_texture) const
{
	*out_texture = genDefaultTexture<SpectralStrength>();
}

// command interface

SdlTypeInfo Image::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "image");
}

void Image::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph