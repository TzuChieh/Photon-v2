#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <cstddef>

namespace ph
{

/*! @brief A general image representing numbers.
The image may use a constant value if there is no input image or as a fallback. Swizzling do not 
have effect in situations where the constant is used. 
*/
class UnifiedNumericImage : public Image
{
public:
	UnifiedNumericImage();

	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<real>> genRealTexture(const CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(const CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(const CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(const CookingContext& ctx);

	UnifiedNumericImage& setImage(std::shared_ptr<Image> image);
	UnifiedNumericImage& setSwizzleSubscripts(std::string swizzleSubscripts);
	UnifiedNumericImage& setConstant(const float64* constantData, std::size_t dataSize);

	template<std::size_t N> requires (N <= Image::ARRAY_SIZE)
	UnifiedNumericImage& setConstant(const std::array<float64, N>& constant)
	{
		m_constant.set(0.0);
		for(std::size_t i = 0; i < N; ++i)
		{
			m_constant[i] = constant[i];
		}
		return *this;
	}

	/*! @brief Get the image.
	@return Pointer to the contained image. May be null if there was no image specified.
	*/
	Image* getImage() const;

	/*! @brief Get the constant value.
	@return A constant value. This value will not be used if an image is present.
	*/
	Image::ArrayType getConstant() const;

	/*! @brief Get the swizzle subscripts for the image.
	Swizzling do not have effect in situations where the constant is used. 
	*/
	std::string_view getSwizzleSubscripts() const;

private:
	std::shared_ptr<Image> m_image;
	std::string            m_swizzleSubscripts;
	Image::ArrayType       m_constant;
};

}// end namespace ph
