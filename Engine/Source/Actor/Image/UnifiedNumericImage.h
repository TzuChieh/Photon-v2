#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Utility/TSpan.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "SDL/sdl_interface.h"

#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <cstddef>
#include <vector>

namespace ph
{

/*! @brief A general image representing numbers.
Allow user to easily specify multiple types of common image parameters in one place. This class is
mainly for convenience as all functionalities are achievable by image references. This class may use
a constant value if there is no input image or as a fallback. Swizzling will affect all image forms,
including the case where constant is used. 
*/
class UnifiedNumericImage : public Image
{
public:
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
	UnifiedNumericImage& setConstant(TSpanView<float64> constant);

	template<std::size_t N> requires (N <= Image::ARRAY_SIZE)
	UnifiedNumericImage& setConstant(const std::array<float64, N>& constant)
	{
		return setConstant(TSpanView<float64>(constant));
	}

	/*! @brief Get the image.
	@return Pointer to the contained image. May be null if there was no image specified.
	*/
	Image* getImage() const;

	/*! @brief Get the constant values.
	@return Constant values. This value will not be used if an image reference is present.
	*/
	TSpanView<float64> getConstant() const;

	/*! @brief Get the swizzle subscripts for the image.
	Swizzling do not have effect in situations where the constant is used. 
	*/
	std::string_view getSwizzleSubscripts() const;

private:
	Image::ArrayType getSwizzledConstant() const;

	std::shared_ptr<Image> m_image;
	ResourceIdentifier m_imageFile;
	std::vector<float64> m_constant;
	std::string m_swizzleSubscripts;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<UnifiedNumericImage>)
	{
		ClassType clazz("unified-numeric-image");
		clazz.docName("Unified Numeric Image");
		clazz.description(
			"A general image that incorporates many numeric value representing methods (e.g., constant "
			"values, image files, image references, and more).");
		clazz.baseOn<UnifiedNumericImage>();

		TSdlReference<Image, OwnerType> image("image", &OwnerType::m_image);
		image.description("References any image.");
		image.optional();
		clazz.addField(image);

		TSdlResourceIdentifier<OwnerType> imageFile("image-file", &OwnerType::m_imageFile);
		imageFile.description(
			"An image file. Will be considered only if no image reference was specified.");
		imageFile.optional();
		clazz.addField(imageFile);

		TSdlRealArray<OwnerType, float64> constant("constant", &OwnerType::m_constant);
		constant.description(
			"Constant values. Will only be considered if no other image data was specified.");
		constant.optional();
		clazz.addField(constant);

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
