#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "DataIO/ExrFileReader.h"
#include "Frame/frame_utils.h"
#include "Frame/RegularPicture.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/ExrFileWriter.h"
#include "DataIO/PfmFileWriter.h"
#include "Math/TVector2.h"
#include "DataIO/sdl_picture_file_type.h"

#include "Common/ThirdParty/lib_stb.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <format>
#include <climits>
#include <type_traits>

namespace ph::io_utils
{

PH_DEFINE_INTERNAL_LOG_GROUP(IOUtils, DataIO);

namespace detail
{

bool init_picture_IO()
{
	// Default loading's origin is on the upper-left corner, these calls made the 
	// origin on the lower-left corner to meet Photon's expectation
	// TODO: New release of stb seems to fix this (need check). Maybe we can just set this for every
	// load call so more control
	stbi_set_flip_vertically_on_load(true);
	stbi_flip_vertically_on_write(true);

	return true;
}

}// end namespace detail

// TODO: make use stb "*_is_16_bit" related funcs

namespace
{

RegularPicture load_LDR_via_stb(const std::string& fullFilename)
{
	// Variables to retrieve image info from stbi_load()
	int widthPx;
	int heightPx;
	int numComponents;

	// The last parameter is "0" since we want the actual components the image has;
	// replace "0" with "1" ~ "4" to force that many components per pixel
	// (we will always get RGBA-ordered data)
	stbi_uc* const stbImageData = stbi_load(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		throw FileIOError(
			"LDR picture loading failed: " + std::string(stbi_failure_reason()),
			fullFilename);
	}

	auto sizePx = math::Vector2S(widthPx, heightPx);
	auto numUCharsInStbImageData = sizePx.product() * numComponents;

	RegularPictureFormat format;

	// HACK: assuming input LDR image is in sRGB color space, we need to properly detect this
	format.setColorSpace(math::EColorSpace::sRGB);

	if(numComponents == 1)
	{
		format.setIsGrayscale(true);
	}
	else if(numComponents == 3)
	{
		format.setIsGrayscale(false);
	}
	else if(numComponents == 4)
	{
		format.setHasAlpha(true);
	}
	else
	{
		PH_LOG_WARNING(IOUtils,
			"unsupported number of components in LDR picture <{}> ({} components detected), may produce error" ,
			fullFilename, numComponents);
	}

	RegularPicture picture(
		sizePx,
		numComponents,
		EPicturePixelComponent::UInt8);

	picture.setFormat(format);
	picture.getPixels().setPixels(stbImageData, numUCharsInStbImageData);

	//for(std::size_t y = 0; y < sizePx.y(); y++)
	//{
	//	for(std::size_t x = 0; x < sizePx.x(); x++)
	//	{
	//		const std::size_t i = (y * sizePx.x() + x) * numComponents;
	//		PH_ASSERT_LT(i, static_cast<std::size_t>(widthPx) * heightPx * numComponents);

	//		// For each pixel component, transform from [0, 255] to [0, 1]
	//		RegularPicture::Pixel pixel(0);
	//		for(int ci = 0; ci < numComponents; ++ci)
	//		{
	//			pixel[ci] = stbImageData[i + ci] / 255.0f;
	//		}

	//		picture.frame.setPixel(x, y, pixel);
	//	}
	//}

	// Free the image data loaded by stb
	stbi_image_free(stbImageData);

	return picture;
}

RegularPicture load_HDR_via_stb(const std::string& fullFilename)
{
	// Variables to retrieve image info from stbi_loadf()
	int widthPx;
	int heightPx;
	int numComponents;

	// The last parameter is "0" since we want the actual components the image has
	// (replace "0" with "1" ~ "4" to force that many components per pixel)
	// (we will always get RGBA-ordered data)
	float* const stbImageData = stbi_loadf(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		throw FileIOError(
			"HDR picture loading failed: " + std::string(stbi_failure_reason()),
			fullFilename);
	}

	auto sizePx = math::Vector2S(widthPx, heightPx);
	auto numFloatsInStbImageData = sizePx.product() * numComponents;

	RegularPictureFormat format;

	// HACK: assuming input HDR image is in linear-sRGB color space, we need to properly detect this
	format.setColorSpace(math::EColorSpace::Linear_sRGB);

	if(numComponents == 1)
	{
		format.setIsGrayscale(true);
	}
	else if(numComponents == 3)
	{
		format.setIsGrayscale(false);
	}
	else if(numComponents == 4)
	{
		format.setHasAlpha(true);
	}
	else
	{
		PH_LOG_WARNING(IOUtils,
			"unsupported number of components in LDR picture <{}> ({} components detected), may produce error" ,
			fullFilename, numComponents);
	}

	RegularPicture picture(
		sizePx,
		numComponents,
		EPicturePixelComponent::Float32);

	picture.setFormat(format);
	picture.getPixels().setPixels(stbImageData, numFloatsInStbImageData);

	//for(uint32 y = 0; y < picture.frame.heightPx(); y++)
	//{
	//	for(uint32 x = 0; x < picture.frame.widthPx(); x++)
	//	{
	//		const std::size_t i = (static_cast<std::size_t>(y) * picture.frame.widthPx() + x) * numComponents;
	//		PH_ASSERT(i < static_cast<std::size_t>(widthPx) * heightPx * numComponents);

	//		// For each pixel component, directly copy floating-point values
	//		RegularPicture::Pixel pixel(0);
	//		for(int ci = 0; ci < numComponents; ++ci)
	//		{
	//			pixel[ci] = stbImageData[i + ci];
	//		}

	//		picture.frame.setPixel(x, y, pixel);
	//	}
	//}

	// Free the image data loaded by stb
	stbi_image_free(stbImageData);

	return picture;
}

}// end anonymous namespace

std::string load_text(const Path& filePath)
{
	std::ifstream textFile;
	textFile.open(filePath.toNativeString());
	if(!textFile.is_open())
	{
		throw FileIOError("cannot open text file <" + filePath.toAbsoluteString() + ">");
	}

	// OPTIMIZATION: a redundant copy here
	std::stringstream buffer;
	buffer << textFile.rdbuf();
	return buffer.str();
}

RegularPicture load_picture(const Path& picturePath)
{
	const std::string& ext = picturePath.getExtension();

	bool hasTriedHDR = false;
	bool hasTriedLDR = false;

	// Try to load it as HDR first
	if(has_HDR_support(ext))
	{
		try
		{
			return load_HDR_picture(picturePath);
		}
		catch(const FileIOError& /* e */)
		{
			hasTriedHDR = true;
		}
	}

	// Then, try to load it as LDR
	if(has_LDR_support(ext))
	{
		try
		{
			return load_LDR_picture(picturePath);
		}
		catch(const FileIOError& /* e */)
		{
			hasTriedLDR = true;
		}
	}

	// If the flow reaches here, loading has failed and we need to throw
	throw FileIOError(std::format(
		"failed loading <{}> image; tried loading as HDR: {}, as LDR: {}", ext, hasTriedHDR, hasTriedLDR),
		picturePath.toString());
}

RegularPicture load_LDR_picture(const Path& picturePath)
{
	PH_LOG_DEBUG(IOUtils, 
		"loading LDR picture <{}>", picturePath.toString());

	const std::string& ext = picturePath.getExtension();
	if(ext == ".png"  || ext == ".PNG"  ||
	   ext == ".jpg"  || ext == ".JPG"  ||
	   ext == ".jpeg" || ext == ".JPEG" ||
	   ext == ".bmp"  || ext == ".BMP"  ||
	   ext == ".tga"  || ext == ".TGA"  ||
	   ext == ".ppm"  || ext == ".PPM"  ||
	   ext == ".pgm"  || ext == ".PGM")
	{
		return load_LDR_via_stb(picturePath.toNativeString());
	}
	else
	{
		throw FileIOError(
			"unsupported LDR image format <" + ext + ">", picturePath.toString());
	}
}

RegularPicture load_HDR_picture(const Path& picturePath)
{
	PH_LOG_DEBUG(IOUtils, 
		"loading HDR picture <{}>", picturePath.toString());

	const std::string& ext = picturePath.getExtension();
	if(ext == ".exr" || ext == ".EXR")
	{
		ExrFileReader exrFileReader(picturePath);

		HdrRgbFrame frame;
		if(exrFileReader.load(&frame))
		{
			// TODO: properly handle picture attributes; properly load from via EXR

			RegularPicture picture(
				math::Vector2S(frame.getSizePx()),
				3,
				EPicturePixelComponent::Float32);

			RegularPictureFormat format;
			format.setColorSpace(math::EColorSpace::Linear_sRGB);
			format.setIsGrayscale(false);
			format.setHasAlpha(false);
			picture.setFormat(format);

			picture.getPixels().setPixels(
				frame.getPixelData().data(),
				math::Vector2S(frame.getSizePx()).product() * 3);
			//picture.nativeFormat = EPicturePixelFormat::PPF_RGB_32F;
			//picture.colorSpace = math::EColorSpace::Linear_sRGB;
			//picture.frame.forEachPixel([&frame](const uint32 x, const uint32 y, auto /* pixel */)
			//{
			//	const auto framePixel = frame.getPixel({x, y});

			//	RegularPicture::Pixel picturePixel(0);
			//	picturePixel[0] = framePixel[0];
			//	picturePixel[1] = framePixel[1];
			//	picturePixel[2] = framePixel[2];

			//	return picturePixel;
			//});

			return picture;
		}
		else
		{
			throw FileIOError(
				".exr file loading failed", picturePath.toString());
		}
	}
	else if(ext == ".hdr" || ext == ".HDR")
	{
		return load_HDR_via_stb(picturePath.toNativeString());
	}
	else
	{
		throw FileIOError(
			"unsupported HDR picture format <" + ext + ">", picturePath.toString());
	}
}

// OPT: make this faster
bool has_LDR_support(const std::string_view filenameExt)
{
	return 
		filenameExt == ".png"  || filenameExt == ".PNG"  ||
		filenameExt == ".jpg"  || filenameExt == ".JPG"  ||
		filenameExt == ".jpeg" || filenameExt == ".JPEG" ||
		filenameExt == ".bmp"  || filenameExt == ".BMP"  ||
		filenameExt == ".tga"  || filenameExt == ".TGA"  ||
		filenameExt == ".ppm"  || filenameExt == ".PPM"  ||
		filenameExt == ".pgm"  || filenameExt == ".PGM";
}

// OPT: make this faster
bool has_HDR_support(const std::string_view filenameExt)
{
	return
		filenameExt == ".exr" || filenameExt == ".EXR" ||
		filenameExt == ".hdr" || filenameExt == ".HDR";
}

bool load_picture_meta(
	const Path& picturePath,
	math::Vector2S* out_sizePx,
	std::size_t* out_numComponents,
	RegularPictureFormat* out_format)
{
	// Variables to retrieve image info from stbi_load()
	int widthPx;
	int heightPx;
	int numComponents;

	if(!stbi_info(picturePath.toNativeString().c_str(), &widthPx, &heightPx, &numComponents))
	{
		return false;
	}

	if(out_sizePx)
	{
		out_sizePx->x() = widthPx;
		out_sizePx->y() = heightPx;
	}

	if(out_numComponents)
	{
		*out_numComponents = numComponents;
	}

	if(out_format)
	{
		*out_format = RegularPictureFormat{};

		// For color space, for currently supported formats we can only make an educated guess
		const std::string& ext = picturePath.getExtension();
		if(has_HDR_support(ext))
		{
			out_format->setColorSpace(math::EColorSpace::Linear_sRGB);
		}

		// Same goes for component info
		if(numComponents == 1)
		{
			out_format->setIsGrayscale(true);
		}
		else if(numComponents == 3)
		{
			out_format->setIsGrayscale(false);
		}
		else if(numComponents == 4)
		{
			out_format->setHasAlpha(true);
		}
	}

	return true;
}

void save(const LdrRgbFrame& frame, const Path& picturePath)
{
	save(
		frame,
		picturePath.getParent(),
		picturePath.removeExtension().getFilename(),
		picture_file_type_from_extension(picturePath.getExtension()));
}

void save(const HdrRgbFrame& frame, const Path& picturePath)
{
	save(
		frame, 
		picturePath.getParent(), 
		picturePath.removeExtension().getFilename(),
		picture_file_type_from_extension(picturePath.getExtension()));
}

void save(
	const LdrRgbFrame& frame,
	const Path& pictureDirectory,
	const std::string& name,
	EPictureFile format)
{
	switch(format)
	{
	case EPictureFile::PNG:
		save_png(frame, pictureDirectory / (name + ".png"));
		break;

	case EPictureFile::JPG:
		save_jpg(frame, pictureDirectory / (name + ".jpg"));
		break;

	case EPictureFile::BMP:
		save_bmp(frame, pictureDirectory / (name + ".bmp"));
		break;

	case EPictureFile::TGA:
		save_tga(frame, pictureDirectory / (name + ".tga"));
		break;

	case EPictureFile::HDR:
	case EPictureFile::EXR:
	case EPictureFile::HighPrecisionEXR:
	{
		HdrRgbFrame hdrFrame;
		frame_utils::to_HDR(frame, &hdrFrame);
		save(hdrFrame, pictureDirectory, name, format);
		break;
	}

	default:
		throw FileIOError(
			"failed to save LDR frame: unsupported format \"" + std::string(TSdlEnum<EPictureFile>{}[format]) + "\"",
			(pictureDirectory / name).toAbsoluteString());
	}
}

void save(
	const HdrRgbFrame& frame,
	const Path& pictureDirectory,
	const std::string& name,
	EPictureFile format)
{
	switch(format)
	{
	case EPictureFile::PNG:
	case EPictureFile::JPG:
	case EPictureFile::BMP:
	case EPictureFile::TGA:
	{
		LdrRgbFrame ldrFrame;
		frame_utils::to_LDR(frame, &ldrFrame);
		save(ldrFrame, pictureDirectory, name, format);
		break;
	}

	case EPictureFile::HDR:
		save_hdr(frame, pictureDirectory / (name + ".hdr"));
		break;

	case EPictureFile::EXR:
		save_exr(frame, pictureDirectory / (name + ".exr"));
		break;

	case EPictureFile::HighPrecisionEXR:
		save_exr_high_precision(frame, pictureDirectory / (name + ".exr"));
		break;

	default:
		throw FileIOError(
			"failed to save HDR frame: unsupported format \"" + std::string(TSdlEnum<EPictureFile>{}[format]) + "\"",
			(pictureDirectory / name).toAbsoluteString());
	}
}

void save_png(const LdrRgbFrame& frame, const Path& picturePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	PH_LOG(IOUtils,
		"saving png <{}>", picturePath.toAbsoluteString());

	const bool stbiResult = stbi_write_png(
		picturePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData().data(),
		static_cast<int>(frame.widthPx()) * 3);

	if(stbiResult == 0)
	{
		throw FileIOError(
			"failed saving png", picturePath.toString());
	}
}

void save_jpg(const LdrRgbFrame& frame, const Path& picturePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	// FIXME: variable quality
	constexpr int QUALITY = 10;

	PH_LOG(IOUtils,
		"saving jpg <{}> with quality = {}", picturePath.toAbsoluteString(), QUALITY);

	const bool stbiResult = stbi_write_jpg(
		picturePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData().data(),
		10);

	if(stbiResult == 0)
	{
		throw FileIOError(
			"failed saving jpg", picturePath.toString());
	}
}

void save_bmp(const LdrRgbFrame& frame, const Path& picturePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	PH_LOG(IOUtils, 
		"saving bmp <{}>", picturePath.toAbsoluteString());

	const bool stbiResult = stbi_write_bmp(
		picturePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData().data());

	if(stbiResult == 0)
	{
		throw FileIOError(
			"failed saving bmp", picturePath.toString());
	}
}

void save_tga(const LdrRgbFrame& frame, const Path& picturePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	PH_LOG(IOUtils,
		"saving tga <{}>", picturePath.toAbsoluteString());

	const bool stbiResult = stbi_write_tga(
		picturePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData().data());

	if(stbiResult == 0)
	{
		throw FileIOError(
			"failed saving tga", picturePath.toString());
	}
}

void save_hdr(const HdrRgbFrame& frame, const Path& picturePath)
{
	static_assert(std::is_same_v<HdrComponent, float>);

	PH_LOG(IOUtils, 
		"saving hdr <{}>", picturePath.toAbsoluteString());

	const bool stbiResult = stbi_write_hdr(
		picturePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData().data());

	if(stbiResult == 0)
	{
		throw FileIOError(
			"failed saving hdr", picturePath.toString());
	}
}

void save_exr(const HdrRgbFrame& frame, const Path& picturePath)
{
	ExrFileWriter writer(picturePath);
	if(!writer.save(frame))
	{
		throw FileIOError(
			"failed saving exr", picturePath.toString());
	}
}

void save_exr_high_precision(const HdrRgbFrame& frame, const Path& picturePath)
{
	ExrFileWriter writer(picturePath);
	if(!writer.saveHighPrecision(frame))
	{
		throw FileIOError(
			"failed saving high precision exr", picturePath.toString());
	}
}

void save_pfm(const HdrRgbFrame& frame, const Path& picturePath)
{
	PfmFileWriter writer(picturePath);
	if(!writer.save(frame))
	{
		throw FileIOError(
			"failed saving pfm", picturePath.toString());
	}
}

void save_exr(const HdrRgbFrame& frame, std::string& byteBuffer)
{
	ExrFileWriter writer(Path(""));
	if(!writer.save(frame, byteBuffer))
	{
		throw FileIOError(
			"failed saving exr to byte buffer");
	}
}

}// end namespace ph::io_utils
