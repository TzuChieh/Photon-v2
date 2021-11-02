#include "Actor/Image/RasterFileImage.h"
#include "Core/Texture/Pixel/TFrameBuffer2D.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Actor/actor_exceptions.h"
#include "Frame/TFrame.h"
#include "Core/Texture/Pixel/TNumericPixelTexture2D.h"
#include "Core/Texture/Pixel/TColorPixelTexture2D.h"
#include "Common/logging.h"

#include <utility>
#include <cstddef>
#include <type_traits>
#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(RasterFileImage, Image);

namespace
{

template<typename T, std::size_t N>
inline TFrame<T, N> make_frame_from_picture(const RegularPicture& picture)
{
	TFrame<T, N> frame(picture.frame.widthPx(), picture.frame.heightPx());

	frame.forEachPixel(
	[&picture](const uint32 x, const uint32 y, auto /* pixel */)
	{
		RegularPicture::Pixel srcPixel = picture.frame.getPixel({x, y});

		// For LDR type, transform to the range [0, 255] for later casting
		if constexpr(std::is_same_v<T, uint8>)
		{
			srcPixel = srcPixel * 255.0f + 0.5f;
		}

		constexpr auto MIN_ELEMENTS = std::min(RegularPicture::Pixel::NUM_ELEMENTS, N);

		// Pixel element casting is based on the smaller number of elements of the two--other
		// elements are either discarded or defaulted to 0.
		TFrame<T, N>::typename Pixel dstPixel(0);
		for(std::size_t ei = 0; ei < NUM_ELEMENTS; ++ei)
		{
			dstPixel[ei] = static_cast<T>(srcPixel[ei]);
		}

		return dstPixel;
	});

	return frame;
}

template<typename T, std::size_t N>
inline std::shared_ptr<TFrameBuffer2D<T, N>> make_frame_buffer_from_picture(const RegularPicture& picture)
{
	return std::make_shared<TFrameBuffer2D<T, N>>(make_frame_from_picture<T, N>(picture));
}

inline pixel_texture::EWrapMode to_texture_wrap_mode(const EImageWrapMode wrapMode)
{
	switch(wrapMode)
	{
	case EImageWrapMode::ClampToEdge:
		return pixel_texture::EWrapMode::ClampToEdge;

	case EImageWrapMode::Repeat:
		return pixel_texture::EWrapMode::Repeat;

	case EImageWrapMode::FlippedClampToEdge:
		return pixel_texture::EWrapMode::FlippedClampToEdge;

	default:
		PH_LOG_WARNING(RasterFileImage, "unsupported image wrap mode, using Repeat");
		return pixel_texture::EWrapMode::Repeat;
	}
}

}// end anonymous namespace

RasterFileImage::RasterFileImage() :
	RasterFileImage(Path())
{}

RasterFileImage::RasterFileImage(Path filePath) :
	
	RasterImageBase(),

	m_filePath(std::move(filePath))
{}

std::shared_ptr<TTexture<Image::Array>> RasterFileImage::genNumericTexture(
	ActorCookingContext& ctx)
{
	auto pixelBuffer = loadPixelBuffer(ctx);
	setResolution(pixelBuffer->getSize());

	return std::make_shared<TNumericPixelTexture2D<Image::ARRAY_SIZE>>(
		pixelBuffer,
		getTextureSampleMode(),
		getTextureWrapModeS(),
		getTextureWrapModeT());
}

std::shared_ptr<TTexture<math::Spectrum>> RasterFileImage::genColorTexture(
	ActorCookingContext& ctx)
{
	math::EColorSpace colorSpace;
	auto pixelBuffer = loadPixelBuffer(ctx, &colorSpace);

	setResolution(pixelBuffer->getSize());

	const auto sampleMode = getTextureSampleMode();
	const auto wrapModeS  = getTextureWrapModeS();
	const auto wrapModeT  = getTextureWrapModeT();
	const auto layout     = pixel_texture::EPixelLayout::PL_RGBA;

	auto textureMaker =
	[&pixelBuffer, layout, sampleMode, wrapModeS, wrapModeT]<math::EColorSpace COLOR_SPACE>()
	{
		return std::make_shared<TColorPixelTexture2D<COLOR_SPACE>>(
			pixelBuffer,
			layout,
			sampleMode,
			wrapModeS,
			wrapModeT);
	};

	switch(colorSpace)
	{
	case math::EColorSpace::sRGB:
		return textureMaker.template operator () <math::EColorSpace::sRGB>();

	case math::EColorSpace::Linear_sRGB:
		return textureMaker.template operator () <math::EColorSpace::Linear_sRGB>();

	case math::EColorSpace::ACEScg:
		return textureMaker.template operator () <math::EColorSpace::ACEScg>();

	// We should not have other color spaces from raster files
	default:
		// TODO: better log warning and use a default picture
		throw ActorCookException(
			"error on generating texture for picture <" + m_filePath.toAbsoluteString() + ">: invalid color space");
	}
}

std::shared_ptr<PixelBuffer2D> RasterFileImage::loadPixelBuffer(
	ActorCookingContext&     ctx,
	math::EColorSpace* const out_colorSpace) const
{
	RegularPicture picture;
	try
	{
		picture = io_utils::load_picture(m_filePath);
	}
	catch(const IOException& e)
	{
		// TODO: better log warning and use a default picture
		throw ActorCookException(
			"error on loading picture <" + m_filePath.toAbsoluteString() + ">");
	}

	if(out_colorSpace)
	{
		*out_colorSpace = picture.colorSpace;
	}

	// TODO: make use of half
	std::shared_ptr<PixelBuffer2D> frameBuffer;
	switch(picture.nativeFormat)
	{
	case EPicturePixelFormat::PPF_Grayscale_8:
		frameBuffer = make_frame_buffer_from_picture<uint8, 1>(picture);
		break;

	case EPicturePixelFormat::PPF_Grayscale_16F:
	case EPicturePixelFormat::PPF_Grayscale_32F:
		frameBuffer = make_frame_buffer_from_picture<float32, 1>(picture);
		break;

	case EPicturePixelFormat::PPF_RGB_8:
		frameBuffer = make_frame_buffer_from_picture<uint8, 3>(picture);
		break;

	case EPicturePixelFormat::PPF_RGB_16F:
	case EPicturePixelFormat::PPF_RGB_32F:
		frameBuffer = make_frame_buffer_from_picture<float32, 3>(picture);
		break;

	case EPicturePixelFormat::PPF_RGBA_8:
		frameBuffer = make_frame_buffer_from_picture<uint8, 4>(picture);
		break;

	case EPicturePixelFormat::PPF_RGBA_16F:
	case EPicturePixelFormat::PPF_RGBA_32F:
		frameBuffer = make_frame_buffer_from_picture<float32, 4>(picture);
		break;

	default:
		// TODO: better log warning and use a default picture
		throw ActorCookException(
			"error on creating frame buffer for <" + m_filePath.toAbsoluteString() + ">");
		break;
	}

	return frameBuffer;
}

pixel_texture::ESampleMode RasterFileImage::getTextureSampleMode() const
{
	switch(getSampleMode())
	{
	case EImageSampleMode::Nearest:
		return pixel_texture::ESampleMode::Nearest;

	case EImageSampleMode::Bilinear:
		return pixel_texture::ESampleMode::Bilinear;

	case EImageSampleMode::Trilinear:
		return pixel_texture::ESampleMode::Trilinear;

	default:
		PH_LOG_WARNING(RasterFileImage, "unsupported image sample mode, using Bilinear");
		return pixel_texture::ESampleMode::Bilinear;
	}
}

pixel_texture::EWrapMode RasterFileImage::getTextureWrapModeS() const
{
	return to_texture_wrap_mode(getHorizontalWrapMode());
}

pixel_texture::EWrapMode RasterFileImage::getTextureWrapModeT() const
{
	return to_texture_wrap_mode(getVerticalWrapMode());
}

}// end namespace ph
