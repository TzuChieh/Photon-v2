#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "SDL/sdl_interface.h"

#include <memory>
#include <string>
#include <string_view>

namespace ph
{

/*! @brief An image whose primary purpose is to swizzle the output of a source image.
*/
class SwizzledImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	/*! @brief Generates numeric texture of an alternative type.
	*/
	///@{
	std::shared_ptr<TTexture<real>> genRealTexture(const CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(const CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(const CookingContext& ctx);
	std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(const CookingContext& ctx);
	///@}

	SwizzledImage& setInput(std::shared_ptr<Image> image);
	SwizzledImage& setSwizzleSubscripts(std::string swizzleSubscripts);

	/*! @brief Get the input image.
	@return Pointer to the input image. May be null if there was no image specified.
	*/
	Image* getInput() const;

	/*! @brief Get the swizzle subscripts for the image.
	*/
	std::string_view getSwizzleSubscripts() const;

private:
	std::shared_ptr<Image> m_input;
	std::string m_swizzleSubscripts;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<SwizzledImage>)
	{
		ClassType clazz("swizzled-image");
		clazz.docName("Swizzled Image");
		clazz.description(
			"An image whose primary purpose is to swizzle the output of a source image.");
		clazz.baseOn<Image>();

		TSdlReference<Image, OwnerType> input("input", &OwnerType::m_input);
		input.description("Reference to an input image that will be swizzled.");
		clazz.addField(input);

		TSdlString<OwnerType> swizzleSubscripts("swizzle-subscripts", &OwnerType::m_swizzleSubscripts);
		swizzleSubscripts.description(
			"Subscripts for imge value swizzling. Supported subscript sets: "
			"1. Cartesian coordinates \"xyzw\", "
			"2. Colors \"rgba\", "
			"3. Image/texture coordinates \"stpq\", "
			"4. Hex-based subscripts (for wide vector types) \"0123456789ABCDEF\".");
		swizzleSubscripts.optional();
		clazz.addField(swizzleSubscripts);

		return clazz;
	}
};

}// end namespace ph
