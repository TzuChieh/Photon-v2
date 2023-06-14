#pragma once

#include "Actor/Image/RasterImageBase.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "SDL/sdl_interface.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"
#include "Core/Texture/Pixel/PixelBuffer2D.h"
#include "Math/Color/color_enums.h"

#include <memory>

namespace ph
{

class RegularPicture;

class RasterFileImage : public RasterImageBase
{
public:
	RasterFileImage();
	explicit RasterFileImage(Path filePath);
	explicit RasterFileImage(ResourceIdentifier imageFile);

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		CookingContext& ctx) override;

	RegularPicture loadRegularPicture() const;
	void setFilePath(Path filePath);

protected:
	std::shared_ptr<PixelBuffer2D> loadPixelBuffer(
		CookingContext&              ctx,
		math::EColorSpace*           out_colorSpace = nullptr,
		pixel_texture::EPixelLayout* out_pixelLayout = nullptr) const;

	pixel_texture::ESampleMode getTextureSampleMode() const;
	pixel_texture::EWrapMode getTextureWrapModeS() const;
	pixel_texture::EWrapMode getTextureWrapModeT() const;

private:
	ResourceIdentifier m_imageFile;

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

		return clazz;
	}
};

}// end namespace ph
