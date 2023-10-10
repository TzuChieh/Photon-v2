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
	explicit UnifiedColorImage(std::shared_ptr<Image> image);
	explicit UnifiedColorImage(math::Vector3R constant);
	UnifiedColorImage(math::Vector3R colorValue, math::EColorSpace colorSpace);

	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	UnifiedColorImage& setImage(std::shared_ptr<Image> image);
	UnifiedColorImage& setConstant(math::Vector3R constant);
	UnifiedColorImage& setConstantColor(math::Vector3R colorValue, math::EColorSpace colorSpace);

	/*! @brief Get the image.
	@return Pointer to the contained image. May be null if there was no image specified.
	*/
	Image* getImage() const;

	/*! @brief Get the constant color.
	@return A constant color. This color will not be used if an image is present.
	*/
	math::Vector3R getConstant() const;

	/*! @brief Get color space of the constant color.
	*/
	math::EColorSpace getConstantColorSpace() const;

private:
	std::shared_ptr<Image> m_image;
	math::Vector3R         m_constant;
	math::EColorSpace      m_constantColorSpace;
};

}// end namespace ph
