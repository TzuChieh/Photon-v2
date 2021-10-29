#pragma once

#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"
#include "Actor/SDLExtension/sdl_color_space_type.h"
#include "Actor/SDLExtension/sdl_color_usage_type.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>

namespace ph
{

class ConstantImage : public Image
{
public:
	ConstantImage();
	explicit ConstantImage(real value);
	explicit ConstantImage(const math::Vector3R& values);
	explicit ConstantImage(std::vector<real> values);
	ConstantImage(real color, math::EColorSpace colorSpace);
	ConstantImage(const math::Vector3R& values, math::EColorSpace colorSpace);
	ConstantImage(std::vector<real> values, math::EColorSpace colorSpace);

	std::shared_ptr<TTexture<Image::NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

private:
	std::vector<float64> m_values;
	math::EColorSpace    m_colorSpace;
};

}// end namespace ph
