#include "DataIO/PictureLoader.h"
#include "Math/Color/color_spaces.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Math/math.h"
#include "DataIO/ExrFileReader.h"
#include "Frame/frame_utils.h"

#include "Common/ThirdParty/lib_stb.h"

#include <iostream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PictureLoader, DataIO);

HdrRgbFrame PictureLoader::load(const Path& picturePath)
{
	HdrRgbFrame frame;

	const std::string& ext = picturePath.getExtension();
	if(ext == ".png"  || ext == ".PNG"  ||
	   ext == ".jpg"  || ext == ".JPG"  ||
	   ext == ".jpeg" || ext == ".JPEG" ||
	   ext == ".bmp"  || ext == ".BMP"  ||
	   ext == ".tga"  || ext == ".TGA"  ||
	   ext == ".ppm"  || ext == ".PPM"  ||
	   ext == ".pgm"  || ext == ".PGM")
	{
		frame_utils::to_HDR(loadLdr(picturePath), &frame);
	}
	else if(ext == ".exr" || ext == ".EXR" ||
	        ext == ".hdr" || ext == ".HDR")
	{
		frame = loadHdr(picturePath);
	}

	return frame;
}

LdrRgbFrame PictureLoader::loadLdr(const Path& picturePath)
{
	PH_LOG(PictureLoader, "loading picture <{}>", picturePath.toString());

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
		PH_LOG_WARNING(PictureLoader, "cannot load <{}> since the format is unsupported",
			picturePath.toString());

		picture = LdrRgbFrame();
	}

	if(picture.isEmpty())
	{
		PH_LOG_WARNING(PictureLoader, "picture <{}> is empty", picturePath.toString());
	}

	return picture;
}

HdrRgbFrame PictureLoader::loadHdr(const Path& picturePath)
{
	PH_LOG(PictureLoader, "loading picture <{}>", picturePath.toString());

	HdrRgbFrame picture;

	const std::string& ext = picturePath.getExtension();
	if(ext == ".exr" || ext == ".EXR")
	{
		ExrFileReader exrFileReader(picturePath);
		if(!exrFileReader.load(&picture))
		{
			PH_LOG_WARNING(PictureLoader, ".exr file loading failed");
		}
	}
	else if(ext == ".hdr" || ext == ".HDR")
	{
		picture = loadHdrViaStb(picturePath.toAbsoluteString());
	}
	else
	{
		PH_LOG_WARNING(PictureLoader, "cannot load <{}> since the format is unsupported",
			picturePath.toString());

		picture = HdrRgbFrame();
	}

	if(picture.isEmpty())
	{
		PH_LOG_WARNING(PictureLoader, "picture <{}> is empty", picturePath.toString());
	}

	return picture;
}

LdrRgbFrame PictureLoader::loadLdrViaStb(const std::string& fullFilename)
{
	// Variables to retrieve image info from stbi_load()
	int widthPx;
	int heightPx;
	int numComponents;

	// Default loading's origin is on the upper-left corner, this call made stb made the 
	// origin on the lower-left corner to meet with Photon's expectation
	stbi_set_flip_vertically_on_load(true);

	// The last parameter is "0" since we want the actual components the image has;
	// replace "0" with "1" ~ "4" to force that many components per pixel
	stbi_uc* stbImageData = stbi_load(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		PH_LOG_WARNING(PictureLoader, "picture <{}> loading failed", fullFilename);

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
			auto linearSrgb = math::Vector3R(math::sRGB_nonlinear_to_linear<real>({
				stbImageData[i + 0] / 255.0_r,
				stbImageData[i + 1] / 255.0_r, 
				stbImageData[i + 2] / 255.0_r}));

			linearSrgb.mulLocal(255.0_r).addLocal(0.5_r).clampLocal(0.0_r, 255.0_r);
			// TODO: truncating to 0 ~ 255 hurts precision especially when storing linear sRGB values
			// (regarding human perception), maybe bind color space information with each frame
			const math::TVector3<uint8> srgb255(linearSrgb);
			picture.setPixel(x, y, LdrRgbFrame::Pixel({srgb255.x, srgb255.y, srgb255.z}));
		}
	}

	// Free the image data loaded by stb
	stbi_image_free(stbImageData);

	return picture;
}

HdrRgbFrame PictureLoader::loadHdrViaStb(const std::string& fullFilename)
{
	// Variables to retrieve image info from stbi_loadf()
	int widthPx;
	int heightPx;
	int numComponents;

	// Stb's default origin is on the upper-left corner, this call made the 
	// origin on the lower-left corner to meet with Photon's expectation
	stbi_set_flip_vertically_on_load(true);

	// The last parameter is "0" since we want the actual components the image has
	// (replace "0" with "1" ~ "4" to force that many components per pixel)
	float* stbImageData = stbi_loadf(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		PH_LOG_WARNING(PictureLoader, "picture <{}> loading failed", fullFilename);

		return HdrRgbFrame();
	}

	if(numComponents != 3)
	{
		PH_LOG_WARNING(PictureLoader, 
			"picture <{}>'s number of components != 3 (has {} components), may produce error", 
			fullFilename, numComponents);
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

	// Free the image data loaded by stb
	stbi_image_free(stbImageData);

	return picture;
}

}// end namespace ph
