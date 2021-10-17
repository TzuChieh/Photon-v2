#pragma once

#include "Actor/Image/RasterImageBase.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/SDL/sdl_interface.h"
#include "DataIO/SDL/Introspect/TSdlPath.h"
#include "Core/Texture/Pixel/PixelBuffer2D.h"

#include <memory>

namespace ph
{

class RasterFileImage : public RasterImageBase
{
public:
	RasterFileImage();
	explicit RasterFileImage(Path filePath);

	/*std::shared_ptr<TTexture<Image::NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) const override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) const override;*/

protected:
	std::shared_ptr<PixelBuffer2D> loadPixelBuffer(ActorCookingContext& ctx);

private:
	Path m_filePath;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<RasterFileImage>)
	{
		ClassType clazz("raster-file");
		clazz.docName("Raster File Image");
		clazz.description(
			"Raster-based image file (most common image file formats belongs to this category).");
		clazz.baseOn<RasterFileImage>();

		TSdlPath<OwnerType> filePath("file-path", &OwnerType::m_filePath);
		filePath.description("Path to the image file. May be a SDL resource identifier.");
		filePath.required();
		clazz.addField(filePath);

		return clazz;
	}
};

}// end namespace ph
