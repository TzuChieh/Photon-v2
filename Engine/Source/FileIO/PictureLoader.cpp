#include "FileIO/PictureLoader.h"
#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include "Common/ThirdParty/lib_stb.h"

#include <iostream>

namespace ph
{

const Logger PictureLoader::logger(LogSender("Picture Loader"));

LdrRgbFrame PictureLoader::loadLdr(const Path& picturePath)
{
	logger.log(ELogLevel::NOTE_MED, 
	           "loading picture <" + picturePath.toString() + ">");

	LdrRgbFrame picture;

	// OPT: make this faster
	const std::string& ext = picturePath.getExtension();
	if(ext == ".png"  || ext == ".PNG"  ||
	   ext == ".jpg"  || ext == ".JPG"  ||
	   ext == ".jpeg" || ext == ".JPEG" ||
	   ext == ".bmp"  || ext == ".BMP"  ||
	   ext == ".tga"  || ext == ".TGA"  ||
	   ext == ".ppm"  || ext == ".PPM"  ||
	   ext == ".pgm"  || ext == ".PGM")
	{
		picture = loadLdrViaStb(picturePath.toAbsoluteString());
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
		           "cannot load <" + 
		           picturePath.toString() + 
		           "> since the format is unsupported");
		picture = LdrRgbFrame();
	}

	if(picture.isEmpty())
	{
		logger.log(ELogLevel::WARNING_MED,
		           "picture <" + picturePath.toString() + "> is empty");
	}
	return picture;
}

HdrRgbFrame PictureLoader::loadHdr(const Path& picturePath)
{
	logger.log(ELogLevel::NOTE_MED, 
	           "loading picture <" + picturePath.toString() + ">");

	HdrRgbFrame picture;

	const std::string& ext = picturePath.getExtension();
	if(ext == ".hdr" || ext == ".HDR")
	{
		picture = loadHdrViaStb(picturePath.toAbsoluteString());
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
		           "cannot load <" + 
		           picturePath.toString() + 
		           "> since the format is unsupported");
		picture = HdrRgbFrame();
	}

	if(picture.isEmpty())
	{
		logger.log(ELogLevel::WARNING_MED,
		           "picture <" + picturePath.toString() + "> is empty");
	}
	return picture;
}

LdrRgbFrame PictureLoader::loadLdrViaStb(const std::string& fullFilename)
{
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
		logger.log(ELogLevel::WARNING_MED,
		           "picture <" + fullFilename + "> loading failed");
		return LdrRgbFrame();
	}

	// FIXME: supports alpha
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
			Vector3R linearSrgb = ColorSpace::sRGB_to_linear_sRGB(srgbPixel);

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

HdrRgbFrame PictureLoader::loadHdrViaStb(const std::string& fullFilename)
{
	// variables to retrieve image info from stbi_loadf()
	//
	int widthPx;
	int heightPx;
	int numComponents;

	// stb's default origin is on the upper-left corner, this call made the 
	// origin on the lower-left corner to meet with Photon's expectation
	//
	stbi_set_flip_vertically_on_load(true);

	// the last parameter is "0" since we want the actual components the image has
	// (replace "0" with "1" ~ "4" to force that many components per pixel)
	//
	float* stbImageData = stbi_loadf(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		logger.log(ELogLevel::WARNING_MED,
		           "picture <" + fullFilename + "> loading failed");
		return HdrRgbFrame();
	}

	if(numComponents != 3)
	{
		logger.log(ELogLevel::WARNING_MED,
			"picture <" + fullFilename + ">'s number of components != 3 (has " +
			std::to_string(numComponents) + " components), may produce error");
	}

	HdrRgbFrame picture(widthPx, heightPx);
	for(uint32 y = 0; y < picture.heightPx(); y++)
	{
		for(uint32 x = 0; x < picture.widthPx(); x++)
		{
			const std::size_t i = (static_cast<std::size_t>(y) * picture.widthPx() + x) * numComponents;
			PH_ASSERT(i < static_cast<std::size_t>(widthPx) * heightPx * numComponents);

			HdrRgbFrame::Pixel rgbPixel;
			rgbPixel[0] = static_cast<HdrComponent>(stbImageData[i + 0]);
			rgbPixel[1] = static_cast<HdrComponent>(stbImageData[i + 1]);
			rgbPixel[2] = static_cast<HdrComponent>(stbImageData[i + 2]);
			picture.setPixel(x, y, rgbPixel);
		}
	}

	// free the image data loaded by stb
	//
	stbi_image_free(stbImageData);

	return picture;
}

}// end namespace ph