#pragma once

#include "Engine/Actor/Image/RasterImageBase.h"
#include "Engine/DataIO/FileSystem/ResourceIdentifier.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Core/Texture/Pixel/pixel_texture_basics.h"
#include "Engine/Core/Texture/Pixel/PixelBuffer2D.h"
#include "Engine/Actor/SDLExtension/sdl_color_enums.h"

#include <memory>

namespace ph
{

class RegularPicture;

class RasterFileImage : public RasterImageBase
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	// TODO: override more numeric texture type for better performance

	RegularPicture loadRegularPicture() const;
	void setFilePath(Path filePath);

protected:
	std::shared_ptr<PixelBuffer2D> loadPixelBuffer(
		const CookingContext&        ctx,
		math::EColorSpace*           out_colorSpace = nullptr,
		pixel_texture::EPixelLayout* out_pixelLayout = nullptr) const;

	pixel_texture::ESampleMode getTextureSampleMode() const;
	pixel_texture::EWrapMode getTextureWrapModeS() const;
	pixel_texture::EWrapMode getTextureWrapModeT() const;

private:
	ResourceIdentifier m_imageFile;
	math::EColorSpace  m_colorSpace;
	bool               m_isColor;

public:
	PH_DEFINE_SDL_CLASS(RasterFileImage, clazz)
	{
		clazz.typeName("raster-file");
		clazz.docName("Raster File Image");
		clazz.description(
			"Raster-based image file (most common image file formats belongs to this category).");
		clazz.baseOn<RasterImageBase>();

		TSdlResourceIdentifier<OwnerType> imageFile("image-file", &OwnerType::m_imageFile);
		imageFile.description("The image file.");
		imageFile.required();
		clazz.addField(imageFile);

		TSdlEnumField<OwnerType, math::EColorSpace> colorSpace(&OwnerType::m_colorSpace);
		colorSpace.description(
			"Source color space. By default, it is read from the file when available; otherwise, "
			"the engine makes an educated guess. An explicit value overrides the result. Numeric "
			"textures always use Raw pixel values and ignore color space.");
		colorSpace.defaultTo(math::EColorSpace::Unspecified);
		colorSpace.optional();
		clazz.addField(colorSpace);

		TSdlBool<OwnerType> isColor("is-color", &OwnerType::m_isColor);
		isColor.description(
			"False treats pixels as Raw even in color mode, which is useful for color-related "
			"numeric data such as image math. Monochromatic Raw pixels fill every working color "
			"space component. Other Raw layouts require a tristimulus working color space, where "
			"channels map directly to its components.");
		isColor.defaultTo(true);
		isColor.optional();
		clazz.addField(isColor);
	}
};

}// end namespace ph
