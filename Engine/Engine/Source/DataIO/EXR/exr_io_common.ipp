#pragma once

#include "DataIO/EXR/exr_io_common.h"
#include "Frame/TFrame.h"

#include <Common/exception.h>

namespace ph
{

#if PH_THIRD_PARTY_HAS_OPENEXR

template<typename T>
inline void copy_frame_to_imf_array(
	const HdrRgbFrame& frame,
	HdrComponent alphaValue,
	Imf::Array2D<TImfPixel<T>>& array2D)
{
	static_assert(std::is_same_v<T, half> || std::is_same_v<T, float>);

	const auto dataWidth = static_cast<long>(frame.widthPx());
	const auto dataHeight = static_cast<long>(frame.heightPx());

	array2D.resizeErase(dataWidth, dataHeight);
	frame.forEachPixel(
		[&array2D, alphaValue]
		(const uint32 x, const uint32 y, const HdrRgbFrame::PixelType& framePixel)
		{
			TImfPixel<T>& pixel = array2D[static_cast<long>(y)][x];

			pixel.r = framePixel[0];
			pixel.g = framePixel[1];
			pixel.b = framePixel[2];
			pixel.a = alphaValue;
		});
}

template<typename T, std::size_t N>
inline void create_imf_header_for_frame(
	Imf::Header& header,
	const TFrame<T, N>& frame,
	const std::array<std::string_view, N>& channelNames)
{
	static_assert(std::is_same_v<T, half> || std::is_same_v<T, float>);
	constexpr Imf::PixelType VALUE_TYPE = std::is_same_v<T, half> ? Imf::HALF : Imf::FLOAT;

	// OpenEXR's origin is on upper-left. In OpenEXR's term, Photon's frame starts with the highest
	// y-coordinate, hence the scanline should be stored in `DECREASING_Y` order for best (read)
	// performance. Note that line order has nothing to do with whether the image is vertically 
	// flipped or not, the image stays the same no matter what you set line order to. Line order
	// simply means the order scanlines are stored in file, much like how the endianess does not
	// affect an integer value but its memory representation.
	//
	// An example: for `Imf::INCREASING_Y`, OpenEXR stores the scanline with smallest y (top) first,
	// and for `Imf::DECREASING_Y`, stores the largest y (bottom) first.
	//
	header.lineOrder() = Imf::DECREASING_Y;

	for(std::size_t channelIdx = 0; channelIdx < N; ++channelIdx)
	{
		// Skip channels without a name
		if(channelNames[channelIdx].empty())
		{
			continue;
		}

		header.channels().insert(std::string(channelNames[channelIdx]), Imf::Channel(VALUE_TYPE));
	}
}

template<std::size_t N>
inline auto find_imf_channels(
	const Imf::Header& header,
	const std::array<std::string_view, N>& channelNames)
-> std::array<const Imf::Channel*, N>
{
	const Imf::ChannelList& channelList = header.channels();

	std::array<const Imf::Channel*, N> channels;
	for(std::size_t channelIdx = 0; channelIdx < N; ++channelIdx)
	{
		if(!channelNames[channelIdx].empty())
		{
			channels[channelIdx] = channelList.findChannel(std::string(channelNames[channelIdx]));
		}
		else
		{
			channels[channelIdx] = nullptr;
		}
	}
	
	return channels;
}

template<typename T, std::size_t N>
inline void map_imf_framebuffer_to_frame(
	Imf::FrameBuffer& framebuffer,
	const Imf::Header& header,
	const TFrame<T, N>& frame,
	const std::array<std::string_view, N>& channelNames)
{
	static_assert(std::is_same_v<T, half> || std::is_same_v<T, float>);
	constexpr Imf::PixelType VALUE_TYPE = std::is_same_v<T, half> ? Imf::HALF : Imf::FLOAT;

	// Coordinates are descrete, hence the +1 in the end
	const Imath::Box2i dataWindow = header.dataWindow();
	const auto dataWidth  = dataWindow.max.x - dataWindow.min.x + 1;
	const auto dataHeight = dataWindow.max.y - dataWindow.min.y + 1;
	const auto minDataX   = dataWindow.min.x;
	const auto minDataY   = dataWindow.min.y;

	if(frame.widthPx() != dataWidth || frame.heightPx() != dataHeight)
	{
		throw_formatted<IllegalOperationException>(
			"cannot map frame data to Imf::Header, dimension mismatch (frame size = {}, header data "
			"window size: ({}, {}))", frame.getSizePx(), dataWidth, dataHeight);
	}

	// Recall that we treat `TFrame` to contain the full data window. `Imf::Slice` expects the base
	// data pointer to point at the first byte of the display window (OpenEXR's origin, on the 
	// upper-right corner). These variables helps the calculation of the offsets. Basically,
	// OpenEXR calculates the memory address of a pixel by `base + x * xStride + y * yStride` (this
	// formula is always valid as long as pixel outside of data window is not accessed, even if data
	// window is smaller than display window).
	// 
	// See https://openexr.com/en/latest/ReadingAndWritingImageFiles.html#writing-an-image-file
	//
	const auto pixelBytes = sizeof(T) * N;
	const auto scanlineBytes = pixelBytes * dataWidth;
	const auto xStride = static_cast<std::ptrdiff_t>(pixelBytes);
	const auto yStride = -static_cast<std::ptrdiff_t>(scanlineBytes);

	PH_ASSERT_GE(dataHeight, 1);
	const char* byteData = reinterpret_cast<const char*>(frame.getPixelData().data());
	const char* firstScanlineData = byteData + scanlineBytes * (dataHeight - 1);
	const char* firstDisplayWindowScanlineData = firstScanlineData + (-minDataY * yStride - minDataX * xStride);

#if PH_DEBUG
	// Purposely use all variables to calculate the end of scanline bytes
	const char* scanlineDataEnd = firstScanlineData + yStride * (dataHeight - 1) + xStride * dataWidth;
	const T* scanlineDataEndExpected = frame.getPixelData().data() + N * frame.widthPx();
	PH_ASSERT(scanlineDataEnd == reinterpret_cast<const char*>(scanlineDataEndExpected));
#endif

	for(std::size_t channelIdx = 0; channelIdx < N; ++channelIdx)
	{
		// Skip channels without a name
		if(channelNames[channelIdx].empty())
		{
			continue;
		}

		framebuffer.insert(
			std::string(channelNames[channelIdx]),
			Imf::Slice(
				VALUE_TYPE,
				// necessary evil here: OpenEXR currently do not have something like `Imf::SliceView`,
				// so we need to `const_cast` here
				const_cast<char*>(firstDisplayWindowScanlineData + sizeof(T) * channelIdx),
				xStride,
				// another necessary evil: we need to present `frame` with its last scanline first and
				// use a negative y-stride as OpenEXR expect images in top-down manner; this parameter
				// is unsigned and we relied on proper overflow behavior, see https://lists.aswf.io/g/openexr-dev/topic/openexr_rgbaoutputfile_is/70222932
				yStride));
	}
}

#endif
// end PH_THIRD_PARTY_HAS_OPENEXR

}// end namespace ph
