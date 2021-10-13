#pragma once

#include "Actor/Image/RasterImageBase.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class RasterFileImage : public RasterImageBase
{
public:
	RasterFileImage();
	explicit RasterFileImage(Path filePath);

	std::shared_ptr<TTexture<Image::NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) const override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) const override;

private:
	Path m_filePath;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<AModel>)
	{
	}
};

}// end namespace ph
