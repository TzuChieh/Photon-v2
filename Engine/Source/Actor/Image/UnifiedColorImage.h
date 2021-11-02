#pragma once

#include "Actor/Image/Image.h"
#include "Math/Color/color_enums.h"

#include <memory>

namespace ph
{

/*! @brief A general image representing colors.
The image may use a constant value if there is no input image or as a fallback.
*/
class UnifiedColorImage : public Image
{
public:
	UnifiedColorImage();

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

	UnifiedColorImage& setImage(std::shared_ptr<Image> image);
	UnifiedColorImage& setConstant(math::Vector3R constant);
	UnifiedColorImage& setConstantColor(math::Vector3R colorValue, math::EColorSpace colorSpace);
	math::Vector3R getConstant() const;
	math::EColorSpace getConstantColorSpace() const;

private:
	std::shared_ptr<Image> m_image;
	math::Vector3R         m_constant;
	math::EColorSpace      m_constantColorSpace;
};

}// end namespace ph
