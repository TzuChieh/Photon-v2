#include "Actor/Image/RasterFileImage.h"

#include <utility>

namespace ph
{

RasterFileImage::RasterFileImage() :
	RasterFileImage(Path())
{}

RasterFileImage::RasterFileImage(Path filePath) :
	
	RasterImageBase(),

	m_filePath(std::move(filePath))
{}

std::shared_ptr<TTexture<Image::NumericArray>> RasterFileImage::genNumericTexture(
	ActorCookingContext& ctx) const
{

}

std::shared_ptr<TTexture<math::Spectrum>> RasterFileImage::genColorTexture(
	ActorCookingContext& ctx) const
{

}

}// end namespace ph
