#include "Actor/Image/Image.h"

namespace ph
{

Image::Image()
{}

std::shared_ptr<TTexture<real>> Image::genTextureReal(
	ActorCookingContext& ctx) const
{
	return genDefaultTexture<real>();
}

std::shared_ptr<TTexture<math::Vector3R>> Image::genTextureVector3R(
	ActorCookingContext& ctx) const
{
	return genDefaultTexture<math::Vector3R>();
}

std::shared_ptr<TTexture<math::Spectrum>> Image::genTextureSpectral(
	ActorCookingContext& ctx) const
{
	return genDefaultTexture<math::Spectrum>();
}

}// end namespace ph
