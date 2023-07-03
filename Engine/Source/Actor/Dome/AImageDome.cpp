#include "Actor/Dome/AImageDome.h"
#include "Actor/Image/RasterFileImage.h"

namespace ph
{

std::shared_ptr<TTexture<math::Spectrum>> AImageDome::loadRadianceFunction(
	const CookingContext& ctx, DomeRadianceFunctionInfo* const out_info) const
{
	RasterFileImage image(m_imageFile.getPath());
	image.setSampleMode(EImageSampleMode::Bilinear);

	// Since we are viewing it from inside a sphere, we flip the image horizontally
	image.setWrapMode(EImageWrapMode::FlippedClampToEdge, EImageWrapMode::ClampToEdge);

	auto radianceFunc = image.genColorTexture(ctx);

	// Provide info for image property after successfully cooking for an up-to-date value
	if(out_info)
	{
		out_info->isAnalytical = false;
		out_info->resolution = math::Vector2S(image.getResolution());
	}

	return radianceFunc;
}

}// end namespace ph
