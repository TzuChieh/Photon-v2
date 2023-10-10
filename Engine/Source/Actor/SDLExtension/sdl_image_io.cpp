#include "Actor/SDLExtension/sdl_image_io.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/RasterFileImage.h"

#include <vector>

namespace ph::sdl
{

//std::shared_ptr<Image> load_spectral_color(const Spectrum& spectrum)
//{
//
//}

std::shared_ptr<Image> load_tristimulus_color(const math::Vector3R& tristimulus, const math::EColorSpace colorSpace)
{
	return std::make_shared<ConstantImage>(tristimulus, colorSpace);
}

std::shared_ptr<Image> load_picture_file(const Path& filePath)
{
	return std::make_shared<RasterFileImage>(filePath);
}

}// end namespace ph::sdl
