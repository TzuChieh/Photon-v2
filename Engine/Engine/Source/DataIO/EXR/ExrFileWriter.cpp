#include "DataIO/EXR/ExrFileWriter.h"
#include "DataIO/EXR/exr_io_common.h"
#include "Frame/TFrame.h"
#include "Utility/ByteBuffer.h"

//#include <Common/logging.h>
#include <Common/exceptions.h>
#include <Common/profiling.h>

#include <type_traits>

namespace ph
{

//PH_DEFINE_INTERNAL_LOG_GROUP(ExrFileWriter, DataIO);

ExrFileWriter::ExrFileWriter()
	: m_filePath()
{}

ExrFileWriter::ExrFileWriter(const Path& filePath)
	: m_filePath(filePath)
{}

void ExrFileWriter::save(const HdrRgbFrame& frame)
{
	saveToFilesystem(frame);
}

void ExrFileWriter::saveHighPrecision(const HdrRgbFrame& frame)
{
	saveToFilesystem(frame, true);
}

void ExrFileWriter::saveToFilesystem(
	const HdrRgbFrame& frame,
	bool saveInHighPrecision,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName,
	std::string_view alphaChannelName,
	HdrComponent alphaValue)
{
	PH_PROFILE_SCOPE();

	saveStandaloneImageData(
		m_filePath, 
		frame,
		saveInHighPrecision,
		redChannelName,
		greenChannelName,
		blueChannelName,
		alphaChannelName,
		alphaValue);
}

void ExrFileWriter::saveToMemory(
	const HdrRgbFrame& frame,
	ByteBuffer& buffer,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName,
	std::string_view alphaChannelName,
	HdrComponent alphaValue)
{
	PH_PROFILE_SCOPE();

#if PH_THIRD_PARTY_HAS_OPENEXR
	static_assert(std::is_same_v<HdrRgbFrame::ElementType, float>);

	const int dataWidth = static_cast<int>(frame.widthPx());
	const int dataHeight = static_cast<int>(frame.heightPx());
	const bool needAlpha = !alphaChannelName.empty();

	// Directly map RGB data from input `frame`

	Imf::Header header(dataWidth, dataHeight);
	create_imf_header_for_frame(
		header,
		frame, 
		{redChannelName, greenChannelName, blueChannelName});

	Imf::FrameBuffer framebuffer;
	map_imf_framebuffer_to_frame(
		framebuffer,
		header,
		frame,
		{redChannelName, greenChannelName, blueChannelName});

	// Optionally allocate a single-channel frame to store alpha value
	if(needAlpha)
	{
		// OPT: wasteful, can it be done better? imf slice fill value?
		TFrame<float, 1> alphaFrame(dataWidth, dataHeight);
		alphaFrame.fill(alphaValue);

		create_imf_header_for_frame(
			header,
			alphaFrame,
			{alphaChannelName});

		map_imf_framebuffer_to_frame(
			framebuffer,
			header,
			alphaFrame,
			{alphaChannelName});

		write_imf_framebuffer_to_memory(header, framebuffer, dataHeight, buffer);
	}
	else
	{
		write_imf_framebuffer_to_memory(header, framebuffer, dataHeight, buffer);
	}
#else
	throw IllegalOperationException(
		"OpenEXR library is not available.");
#endif
// end PH_THIRD_PARTY_HAS_OPENEXR
}

void ExrFileWriter::saveStandaloneImageData(
	const Path& filePath,
	const HdrRgbFrame& frame,
	bool saveInHighPrecision,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName,
	std::string_view alphaChannelName,
	HdrComponent alphaValue)
{
#if PH_THIRD_PARTY_HAS_OPENEXR
	const auto valueType = saveInHighPrecision ? Imf::FLOAT : Imf::HALF;
	const int dataWidth = static_cast<int>(frame.widthPx());
	const int dataHeight = static_cast<int>(frame.heightPx());
	const bool needAlpha = !alphaChannelName.empty();
	const bool needConversion = !(std::is_same_v<HdrRgbFrame::ElementType, float> && valueType == Imf::FLOAT);

	Imf::Header header(dataWidth, dataHeight);

	// Fastest route: directly map data from input `frame`
	if(!needAlpha && !needConversion)
	{
		PH_ASSERT((std::is_same_v<HdrRgbFrame::ElementType, float>));

		create_imf_header_for_frame(
			header,
			frame, 
			{redChannelName, greenChannelName, blueChannelName});

		Imf::FrameBuffer framebuffer;
		map_imf_framebuffer_to_frame(
			framebuffer,
			header,
			frame,
			{redChannelName, greenChannelName, blueChannelName});

		write_imf_framebuffer_to_filesystem(header, framebuffer, dataHeight, filePath);
	}
	// Need to allocate a new frame to store the copied/converted data there.
	// We always do it with a RGBA frame.
	else
	{
		if(valueType == Imf::FLOAT)
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

			create_imf_header_for_frame(
				header,
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName});

			Imf::FrameBuffer framebuffer;
			map_imf_framebuffer_to_frame(
				framebuffer,
				header,
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName});

			write_imf_framebuffer_to_filesystem(header, framebuffer, dataHeight, filePath);
		}
		else
		{
			PH_ASSERT(valueType == Imf::HALF);

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

			create_imf_header_for_frame(
				header,
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName});

			Imf::FrameBuffer framebuffer;
			map_imf_framebuffer_to_frame(
				framebuffer,
				header,
				rgbaFrame,
				{redChannelName, greenChannelName, blueChannelName, alphaChannelName});

			write_imf_framebuffer_to_filesystem(header, framebuffer, dataHeight, filePath);
		}
	}
#else
	throw IllegalOperationException(
		"OpenEXR library is not available.");
#endif
// end PH_THIRD_PARTY_HAS_OPENEXR
}

}// end namespace ph
