#pragma once

#include "Actor/Image/Image.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class UnifiedColorImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

private:

};

}// end namespace ph
