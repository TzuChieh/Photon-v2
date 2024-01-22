#pragma once

#include "Actor/Image/RasterImageBase.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "SDL/sdl_interface.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"
#include "Core/Texture/Pixel/PixelBuffer2D.h"
#include "Actor/SDLExtension/sdl_color_enums.h"

#include <memory>

namespace ph
{

class RegularPicture;

class RasterFileImage : public RasterImageBase
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

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
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<RasterFileImage>)
	{
		ClassType clazz("raster-file");
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
			"Color space of the raster image. By default, color space is retrieved from the file, "
			"and if such information is not available, the engine will make an educated guess. "
			"If user has specified a non-default color space, then the engine will use it instead "
			"(even if the file said otherwise). Note that when the image is used in numeric mode, "
			"the engine still follow the same convention--color space is dropped and raw values "
			"are used.");
		colorSpace.defaultTo(math::EColorSpace::Unspecified);
		colorSpace.optional();
		clazz.addField(colorSpace);

		TSdlBool<OwnerType> isColor("is-color", &OwnerType::m_isColor);
		isColor.description(
			"If this flag is set to false, then the raster image will be treated as raw data even "
			"if the image is used in color mode. This is useful for situations where color-related "
			"numeric values are used (e.g., math operations involving other color data). When the "
			"engine is in spectral mode, raw data may be treated as linear sRGB if a direct "
			"conversion is impossible.");
		isColor.defaultTo(true);
		isColor.optional();
		clazz.addField(isColor);

		return clazz;
	}
};

}// end namespace ph
