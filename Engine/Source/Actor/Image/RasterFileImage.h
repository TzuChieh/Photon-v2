#pragma once

#include "Actor/Image/RasterImageBase.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"
#include "Core/Texture/Pixel/PixelBuffer2D.h"
#include "Math/Color/color_enums.h"

#include <memory>

namespace ph
{

class RasterFileImage : public RasterImageBase
{
public:
	RasterFileImage();
	explicit RasterFileImage(Path filePath);

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

protected:
	std::shared_ptr<PixelBuffer2D> loadPixelBuffer(
		ActorCookingContext&         ctx, 
		math::EColorSpace*           out_colorSpace = nullptr,
		pixel_texture::EPixelLayout* out_pixelLayout = nullptr) const;

	pixel_texture::ESampleMode getTextureSampleMode() const;
	pixel_texture::EWrapMode getTextureWrapModeS() const;
	pixel_texture::EWrapMode getTextureWrapModeT() const;

private:
	Path m_filePath;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<RasterFileImage>)
	{
		ClassType clazz("raster-file");
		clazz.docName("Raster File Image");
		clazz.description(
			"Raster-based image file (most common image file formats belongs to this category).");
		clazz.baseOn<RasterImageBase>();

		TSdlPath<OwnerType> filePath("file-path", &OwnerType::m_filePath);
		filePath.description("Path to the image file. May be a SDL resource identifier.");
		filePath.required();
		clazz.addField(filePath);

		return clazz;
	}
};

}// end namespace ph
