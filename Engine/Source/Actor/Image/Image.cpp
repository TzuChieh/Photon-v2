#include "Actor/Image/Image.h"
#include "Actor/actor_exceptions.h"

namespace ph
{

Image::Image()
{}

std::shared_ptr<TTexture<Image::NumericArray>> Image::genNumericTexture(
	ActorCookingContext& ctx) const
{
	throw ActorCookException(
		"The image does not support numeric texture generation.");
}

std::shared_ptr<TTexture<math::Spectrum>> Image::genColorTexture(
	ActorCookingContext& ctx) const
{
	throw ActorCookException(
		"The image does not support color texture generation.");
}

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
