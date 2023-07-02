#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>
#include <string>
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

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<real>> genRealTexture(CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(CookingContext& ctx);

	UnifiedNumericImage& setImage(std::shared_ptr<Image> image);
	UnifiedNumericImage& setSwizzleSubscripts(std::string swizzleSubscripts);
	UnifiedNumericImage& setConstant(const float64* constantData, std::size_t dataSize);

	template<std::size_t N> requires (N <= Image::ARRAY_SIZE)
	inline UnifiedNumericImage& setConstant(const std::array<float64, N>& constant)
	{
		m_constant.set(0.0);
		for(std::size_t i = 0; i < N; ++i)
		{
			m_constant[i] = constant[i];
		}
		return *this;
	}

private:
	std::shared_ptr<Image> m_image;
	std::string            m_swizzleSubscripts;
	Image::Array           m_constant;
};

}// end namespace ph
