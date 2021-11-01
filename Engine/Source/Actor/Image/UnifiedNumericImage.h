#pragma once

#include "Actor/Image/Image.h"

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

	std::shared_ptr<TTexture<Image::NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<real>> genRealTexture(ActorCookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(ActorCookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(ActorCookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(ActorCookingContext& ctx);

	UnifiedNumericImage& setImage(std::shared_ptr<Image> image);
	UnifiedNumericImage& setSwizzleSubscripts(std::string swizzleSubscripts);
	UnifiedNumericImage& setConstant(const float64* constantData, std::size_t dataSize);

	template<std::size_t N> requires (N <= Image::NUMERIC_ARRAY_SIZE)
	inline UnifiedNumericImage& setConstant(const std::array<float64, N>& constant)
	{
		m_constant.fill(0.0);
		for(std::size_t i = 0; i < N; ++i)
		{
			m_constant[i] = constant[i];
		}
		return *this;
	}

private:
	std::shared_ptr<Image> m_image;
	std::string            m_swizzleSubscripts;
	Image::NumericArray    m_constant;
};

}// end namespace ph
