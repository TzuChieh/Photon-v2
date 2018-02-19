#include "FileIO/PictureLoader.h"
#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Math/Math.h"

#include "Common/ThirdParty/lib_stb.h"

#include <iostream>

namespace ph
{

const Logger& PictureLoader::LOGGER()
{
	static const Logger logger(LogSender("Pic. Loader"));
	return logger;
}

LdrRgbFrame PictureLoader::loadLdr(const Path& picturePath)
{
	// FIXME: testing
	LOGGER().log(ELogLevel::NOTE_MED, "loading image <" + picturePath.toString() + ">");

	LdrRgbFrame picture;

	const std::string& ext = picturePath.getExtension();
	if(ext == ".png"  || 
	   ext == ".jpg"  || 
	   ext == ".jpeg" ||
	   ext == ".bmp"  ||
	   ext == ".tga"  ||
	   ext == ".ppm"  ||
	   ext == ".pgm")
	{
		picture = loadLdrViaStb(picturePath.toAbsoluteString());
	}
	else
	{
		std::cerr << "warning: at PictureLoader::loadLdr(), " 
		          << "cannot load <"
		          << picturePath.toString()
		          << "> since the format is unsupported" << std::endl;
		picture = LdrRgbFrame();
	}

	if(picture.isEmpty())
	{
		std::cerr << "warning: at PictureLoader::loadLdr(), " 
		          << "picture <"
		          << picturePath.toString()
		          << "> is empty" << std::endl;
	}

	return picture;
}

LdrRgbFrame PictureLoader::loadLdrViaStb(const std::string& fullFilename)
{
	std::cout << "loading image <" << fullFilename << ">" << std::endl;

	// variables to retrieve image info from stbi_load()
	int widthPx;
	int heightPx;
	int numComponents;

	// default loading's origin is on the upper-left corner, this call made stb made the 
	// origin on the lower-left corner to meet with Photon's expectation
	stbi_set_flip_vertically_on_load(true);

	// the last parameter is "0" since we want the actual components the image has;
	// replace "0" with "1" ~ "4" to force that many components per pixel
	stbi_uc* stbImageData = stbi_load(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		std::cerr << "warning: at PictureLoader::loadLdrViaStb(), "
		          << "picture <" 
		          << fullFilename 
		          << "> loading failed" << std::endl;
		std::cerr << "(message: " << stbi_failure_reason() << ")" << std::endl;
		return LdrRgbFrame();
	}

	if(numComponents != 3)
	{
		std::cerr << "warning: at TextureLoader::load(), " 
		          << "picture <" 
		          << fullFilename 
		          << ">'s number of components != 3 (has " 
		          << numComponents 
		          << " components), may produce error" << std::endl;
		//return TFrame<uint8>();
	}

	LdrRgbFrame picture(widthPx, heightPx);
	for(uint32 y = 0; y < picture.heightPx(); y++)
	{
		for(uint32 x = 0; x < picture.widthPx(); x++)
		{
			const std::size_t i = (static_cast<std::size_t>(y) * picture.widthPx() + x) * numComponents;
			PH_ASSERT(i < static_cast<std::size_t>(widthPx) * heightPx * numComponents);

			// HACK: assuming input image is in sRGB color space
			const Vector3R srgbPixel(stbImageData[i + 0] / 255.0_r, 
		                             stbImageData[i + 1] / 255.0_r, 
		                             stbImageData[i + 2] / 255.0_r);
			Vector3R linearSrgb = ColorSpace::srgbToLinearSrgb(srgbPixel);

			linearSrgb.mulLocal(255.0_r).addLocal(0.5_r).clampLocal(0.0_r, 255.0_r);
			// TODO: truncating to 0 ~ 255 hurts precision especially when storing linear sRGB values
			// (regarding human perception), maybe bind color space information with each frame
			const TVector3<uint8> srgb255(linearSrgb);
			picture.setPixel(x, y, LdrRgbFrame::Pixel({srgb255.x, srgb255.y, srgb255.z}));
		}
	}

	// free the image data loaded by stb
	stbi_image_free(stbImageData);

	return picture;
}

}// end namespace ph