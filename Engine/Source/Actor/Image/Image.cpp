#include "Actor/Image/Image.h"
#include "Actor/actor_exceptions.h"

namespace ph
{

Image::Image()
{}

std::shared_ptr<TTexture<Image::NumericArray>> Image::genNumericTexture(
	ActorCookingContext& ctx)
{
	throw ActorCookException(
		"The image does not support numeric texture generation.");
}

std::shared_ptr<TTexture<math::Spectrum>> Image::genColorTexture(
	ActorCookingContext& ctx)
{
	throw ActorCookException(
		"The image does not support color texture generation.");
}

}// end namespace ph
