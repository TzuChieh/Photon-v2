#include "DataIO/ExrFileWriter.h"

#include "Common/ThirdParty/lib_openexr.h"

#include <Common/logging.h>

#include <type_traits>
#include <string>
#include <array>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ExrFileWriter, DataIO);

namespace
{

/* Helpers for writing OpenEXR files. The basic idea is that we provide pixel data in `TFrame`, then
create `Imf::Header` and map frame data using `Imf::FrameBuffer`. Finally, pixel data is written to
file using `Imf::OutputFile`. If the element type in `TFrame` does not match the precision we want to
output, then a conversion will be done (converting to a suitable `TFrame`).

`Imf::FrameBuffer` and `Imf::Slice` do not own the specified data, they only reference it. The original
pixel data (typically a `TFrame` here) must be kept alive during the output process.
*/

template<typename T>
struct TImfPixel final
{
	T r;
	T g;
	T b;
	T a;
};

template<typename T>
inline void copyFrameToImfArray2D(
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
		(const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& framePixel)
		{
			TImfPixel<T>& pixel = array2D[static_cast<long>(y)][x];

			pixel.r = framePixel[0];
			pixel.g = framePixel[1];
			pixel.b = framePixel[2];
			pixel.a = alphaValue;
		});
}

/*!
@param channelNames Name for each channel. Channels without a name will be omitted.
*/
template<typename T, std::size_t N>
inline void mapFrameToImfFramebuffer(
	const TFrame<T, N>& frame,
	const std::array<std::string_view, N>& channelNames,
	Imf::FrameBuffer& framebuffer)
{
	static_assert(std::is_same_v<T, half> || std::is_same_v<T, float>);

	constexpr Imf::PixelType PIXEL_TYPE = std::is_same_v<T, half> ?
		Imf::PixelType::HALF : Imf::PixelType::FLOAT;

	PH_ASSERT_GE(frame.heightPx(), 1);
	const auto lastScanlineOffset = (N * frame.widthPx()) * (frame.heightPx() - 1);

	for(std::size_t channelIdx = 0; channelIdx < N; ++channelIdx)
	{
		// Skip channels without a name
		if(channelNames[channelIdx].empty())
		{
			continue;
		}

		auto lastScanlinePtr = reinterpret_cast<const char*>(
			frame.getPixelData().data() + lastScanlineOffset + channelIdx);

		framebuffer.insert(
			std::string(channelNames[channelIdx]),
			Imf::Slice(
				PIXEL_TYPE,
				// necessary evil here: OpenEXR currently do not have something like `Imf::SliceView`,
				// so we need to `const_cast` here
				const_cast<char*>(lastScanlinePtr),
				sizeof(T) * N,
				// another necessary evil: we need to present `frame` with its last scanline first and
				// use a negative y-stride as OpenEXR expect images in top-down manner; this parameter
				// is unsigned and we relied on proper overflow behavior, see https://lists.aswf.io/g/openexr-dev/topic/openexr_rgbaoutputfile_is/70222932
				-static_cast<std::ptrdiff_t>(sizeof(T) * N * frame.widthPx())));
	}
}

template<typename T, std::size_t N>
inline void createImfHeaderForFrame(
	const TFrame<T, N>& frame,
	const std::array<std::string_view, N>& channelNames,
	Imf::Header& header)
{
	static_assert(std::is_same_v<T, half> || std::is_same_v<T, float>);

	constexpr Imf::PixelType PIXEL_TYPE = std::is_same_v<T, half> ?
		Imf::PixelType::HALF : Imf::PixelType::FLOAT;

	// OpenEXR's origin is on upper-left. In OpenEXR's term, Photon's frame starts with the highest
	// y-coordinate, hence the scanline should be stored in `DECREASING_Y` order for best (read)
	// performance. Note that line order has nothing to do with whether the image is vertically 
	// flipped or not, the image stays the same no matter what you set line order to. Line order
	// simply means the order scanlines are stored in file, much like how the endianess does not
	// affect an integer value but its memory representation.
	header.lineOrder() = Imf::DECREASING_Y;

	for(std::size_t channelIdx = 0; channelIdx < N; ++channelIdx)
	{
		// Skip channels without a name
		if(channelNames[channelIdx].empty())
		{
			continue;
		}

		header.channels().insert(std::string(channelNames[channelIdx]), Imf::Channel(PIXEL_TYPE));
	}
}

inline void writeImfFramebufferToFilesystem(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	const Path& filePath)
{
	Imf::OutputFile output(filePath.toNativeString().c_str(), header);
	output.setFrameBuffer(framebuffer);
	output.writePixels(numScanlines);
}

inline void writeImfFramebufferToMemory(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	ByteBuffer& buffer)
{
	Imf::StdOSStream stream;
	Imf::OutputFile output(stream, header);
	output.setFrameBuffer(framebuffer);
	output.writePixels(numScanlines);

	// OPT: implement a custom Imf stream type to write data directly

	std::string imfStreamData = stream.str();
	buffer.write(imfStreamData.data(), imfStreamData.size());
}

}// end anonymous namespace

ExrFileWriter::ExrFileWriter()
	: m_filePath()
{}

ExrFileWriter::ExrFileWriter(const Path& filePath)
	: m_filePath(filePath)
{}

bool ExrFileWriter::save(const HdrRgbFrame& frame)
{
	return saveToFilesystem(frame);
}

bool ExrFileWriter::saveHighPrecision(const HdrRgbFrame& frame)
{
	return saveToFilesystem(frame, true);
}

bool ExrFileWriter::saveToFilesystem(
	const HdrRgbFrame& frame,
	bool saveInHighPrecision,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName,
	std::string_view alphaChannelName,
	HdrComponent alphaValue)
{
	try
	{
		return saveStandaloneImageData(
			m_filePath, 
			frame,
			saveInHighPrecision,
			redChannelName,
			greenChannelName,
			blueChannelName,
			alphaChannelName,
			alphaValue);
	}
	catch(const std::exception& e)
	{
		PH_LOG_WARNING(ExrFileWriter, "failed saving <{}>, reason: {}",
			m_filePath.toString(), e.what());
		return false;
	}
}

bool ExrFileWriter::saveToMemory(
	const HdrRgbFrame& frame,
	ByteBuffer& buffer,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName,
	std::string_view alphaChannelName,
	HdrComponent alphaValue)
{
	static_assert(std::is_same_v<HdrRgbFrame::ElementType, float>);
	constexpr auto PIXEL_TYPE = Imf::PixelType::FLOAT;

	const int dataWidth = static_cast<int>(frame.widthPx());
	const int dataHeight = static_cast<int>(frame.heightPx());
	const bool needAlpha = !alphaChannelName.empty();

	try
	{
		// Directly map RGB data from input `frame`

		Imf::Header header(dataWidth, dataHeight);
		createImfHeaderForFrame(
			frame, 
			{redChannelName, greenChannelName, blueChannelName},
			header);

		Imf::FrameBuffer framebuffer;
		mapFrameToImfFramebuffer(
			frame,
			{redChannelName, greenChannelName, blueChannelName},
			framebuffer);

		// Optionally allocate a single-channel frame to store alpha value
		if(needAlpha)
		{
			// OPT: wasteful, can it be done better? imf slice fill value?
			TFrame<float, 1> alphaFrame(dataWidth, dataHeight);
			alphaFrame.fill(alphaValue);

			createImfHeaderForFrame(
				alphaFrame,
				{alphaChannelName},
				header);

			mapFrameToImfFramebuffer(
				alphaFrame,
				{alphaChannelName},
				framebuffer);

			writeImfFramebufferToMemory(header, framebuffer, dataHeight, buffer);
		}
		else
		{
			writeImfFramebufferToMemory(header, framebuffer, dataHeight, buffer);
		}

		return true;
	}
	catch(const std::exception& e)
	{
		PH_LOG_WARNING(ExrFileWriter, "failed saving exr file to memroy, reason: {}", 
			e.what());
		return false;
	}
}

bool ExrFileWriter::saveStandaloneImageData(
	const Path& filePath,
	const HdrRgbFrame& frame,
	bool saveInHighPrecision,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName,
	std::string_view alphaChannelName,
	HdrComponent alphaValue)
{
	const auto pixelType = saveInHighPrecision ? Imf::PixelType::FLOAT : Imf::PixelType::HALF;
	const int dataWidth = static_cast<int>(frame.widthPx());
	const int dataHeight = static_cast<int>(frame.heightPx());
	const bool needAlpha = !alphaChannelName.empty();
	const bool needConversion = !(std::is_same_v<HdrRgbFrame::ElementType, float> && pixelType == Imf::PixelType::FLOAT);

	Imf::Header header(dataWidth, dataHeight);

	// Fastest route: directly map data from input `frame`
	if(!needAlpha && !needConversion)
	{
		PH_ASSERT((std::is_same_v<HdrRgbFrame::ElementType, float>));

		createImfHeaderForFrame(
			frame, 
			{redChannelName, greenChannelName, blueChannelName},
			header);

		Imf::FrameBuffer framebuffer;
		mapFrameToImfFramebuffer(
			frame,
			{redChannelName, greenChannelName, blueChannelName},
			framebuffer);

		writeImfFramebufferToFilesystem(header, framebuffer, dataHeight, filePath);
	}
	// Need to allocate a new frame to store the copied/converted data there.
	// We always do it with a RGBA frame.
	else
	{
		if(pixelType == Imf::PixelType::FLOAT)
		{
			TFrame<float, 4> rgbaFrame(dataWidth, dataHeight);
			frame.forEachPixel(
				[&rgbaFrame, alphaValue]
				(uint32 x, uint32 y, const HdrRgbFrame::PixelType& srcPixel)
				{
					TFrame<float, 4>::PixelType dstPixel;
					dstPixel[0] = static_cast<float>(srcPixel[0]);
					dstPixel[1] = static_cast<float>(srcPixel[1]);
					dstPixel[2] = static_cast<float>(srcPixel[2]);
					dstPixel[3] = static_cast<float>(alphaValue);

					rgbaFrame.setPixel(x, y, dstPixel);
				});

			createImfHeaderForFrame(
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName},
				header);

			Imf::FrameBuffer framebuffer;
			mapFrameToImfFramebuffer(
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName},
				framebuffer);

			writeImfFramebufferToFilesystem(header, framebuffer, dataHeight, filePath);
		}
		else
		{
			PH_ASSERT(pixelType == Imf::PixelType::HALF);

			TFrame<half, 4> rgbaFrame(dataWidth, dataHeight);
			frame.forEachPixel(
				[&rgbaFrame, alphaValue]
				(uint32 x, uint32 y, const HdrRgbFrame::PixelType& srcPixel)
				{
					TFrame<half, 4>::PixelType dstPixel;
					dstPixel[0] = static_cast<half>(srcPixel[0]);
					dstPixel[1] = static_cast<half>(srcPixel[1]);
					dstPixel[2] = static_cast<half>(srcPixel[2]);
					dstPixel[3] = static_cast<half>(alphaValue);

					rgbaFrame.setPixel(x, y, dstPixel);
				});

			createImfHeaderForFrame(
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName},
				header);

			Imf::FrameBuffer framebuffer;
			mapFrameToImfFramebuffer(
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName},
				framebuffer);

			writeImfFramebufferToFilesystem(header, framebuffer, dataHeight, filePath);
		}
	}

	return true;
}

}// end namespace ph
