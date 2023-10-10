#pragma once

#include "Actor/Image/Image.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "Math/TVector3.h"
#include "Math/Color/color_enums.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief A general image representing colors.
Allow user to easily specify multiple types of common image parameters in one place. This class is
mainly for convenience as all functionalities are achievable by image references. This class may
use a constant value if there is no input image or as a fallback.
*/
class UnifiedColorImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	UnifiedColorImage& setImage(std::shared_ptr<Image> image);
	UnifiedColorImage& setFile(Path imageFile);
	UnifiedColorImage& setConstant(math::Vector3R constant);
	UnifiedColorImage& setConstantColor(math::Vector3R colorValue, math::EColorSpace colorSpace);

	/*! @brief Get the image.
	@return Pointer to the contained image. May be null if there was no image specified.
	*/
	Image* getImage() const;

	/*! @brief Get the constant color.
	@return A constant color. This color will not be used if an image reference is present.
	*/
	math::Vector3R getConstant() const;

	/*! @brief Get color space of the constant color.
	*/
	math::EColorSpace getConstantColorSpace() const;

private:
	std::shared_ptr<Image> m_image;
	ResourceIdentifier m_imageFile;
	math::Vector3R m_constant;
	math::EColorSpace m_constantColorSpace;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<UnifiedColorImage>)
	{
		ClassType clazz("unified-color-image");
		clazz.docName("Unified Color Image");
		clazz.description(
			"A general image that incorporates many color representing methods (e.g., constant "
			"colors, image files, image references, and more).");
		clazz.baseOn<UnifiedColorImage>();

		TSdlReference<Image, OwnerType> image("image", &OwnerType::m_image);
		image.description("References any image.");
		image.optional();
		clazz.addField(image);

		TSdlResourceIdentifier<OwnerType> imageFile("image-file", &OwnerType::m_imageFile);
		imageFile.description("An image file. Will be considered only if no image reference was specified.");
		imageFile.optional();
		clazz.addField(imageFile);

		TSdlVector3<OwnerType, real> constant("constant", &OwnerType::m_constant);
		constant.description("A constant color. Will only be considered if no other image data was specified.");
		constant.defaultTo(math::Vector3R(0));
		constant.optional();
		clazz.addField(constant);

		TSdlEnumField<OwnerType, math::EColorSpace> constantColorSpace("constant-color-space", &OwnerType::m_constantColorSpace);
		constantColorSpace.description("Color space of the constant color.");
		constantColorSpace.defaultTo(math::EColorSpace::Linear_sRGB);
		constantColorSpace.optional();
		clazz.addField(constantColorSpace);

		return clazz;
	}
};

}// end namespace ph
