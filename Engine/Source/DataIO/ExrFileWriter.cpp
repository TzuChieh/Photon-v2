#include "DataIO/ExrFileWriter.h"
#include "Common/Logger.h"

#include "Common/ThirdParty/lib_openexr.h"

#include <type_traits>

namespace ph
{

namespace
{
	const Logger logger(LogSender("EXR File Writer"));

	template<typename DatumType>
	bool saveStandaloneRgbData(const Path& filePath, const HdrRgbFrame& frame);
}

ExrFileWriter::ExrFileWriter(const Path& filePath) : 
	m_filePath(filePath)
{}

bool ExrFileWriter::save(const HdrRgbFrame& frame)
{
	try
	{
		return saveStandaloneRgbData<half>(m_filePath, frame);
	}
	catch(const std::exception& e)
	{
		logger.log(ELogLevel::WARNING_MED,
			"failed saving <" + m_filePath.toString() + ">, reason: " + e.what());
		return false;
	}
}

bool ExrFileWriter::saveHighPrecision(const HdrRgbFrame& frame)
{
	try
	{
		return saveStandaloneRgbData<float>(m_filePath, frame);
	}
	catch(const std::exception& e)
	{
		logger.log(ELogLevel::WARNING_MED,
			"failed saving <" + m_filePath.toString() + ">, reason: " + e.what());
		return false;
	}
}

namespace
{

template<typename DatumType>
bool saveStandaloneRgbData(const Path& filePath, const HdrRgbFrame& frame)
{
	static_assert(std::is_same_v<DatumType, half> || std::is_same_v<DatumType, float>);

	constexpr Imf::PixelType PIXEL_TYPE = std::is_same_v<DatumType, half> ?
		Imf::PixelType::HALF : Imf::PixelType::FLOAT;

	logger.log(ELogLevel::NOTE_MIN,
		"saving standalone RGB: " + filePath.toAbsoluteString() + " "
		"(" + (std::is_same_v<DatumType, half> ? "half" : "full") + " precision)");

	const int dataWidth  = static_cast<int>(frame.widthPx());
	const int dataHeight = static_cast<int>(frame.heightPx());
	
	Imf::Header header(dataWidth, dataHeight);
	header.channels().insert("R", Imf::Channel(PIXEL_TYPE));
	header.channels().insert("G", Imf::Channel(PIXEL_TYPE));
	header.channels().insert("B", Imf::Channel(PIXEL_TYPE));

	Imf::OutputFile file(filePath.toAbsoluteString().c_str(), header);

	struct RgbPixel
	{
		DatumType r, g, b;
	};

	Imf::Array2D<RgbPixel> pixels(dataHeight, dataWidth);

	frame.forEachPixel(
		[&pixels, height = (long)dataHeight]
			(const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& pixel)
		{
			RgbPixel& dataPixel = pixels[height - 1 - (long)y][(long)x];

			dataPixel.r = pixel[0];
			dataPixel.g = pixel[1];
			dataPixel.b = pixel[2];
		});

	Imf::FrameBuffer frameBuffer;
	frameBuffer.insert(
		"R",
		Imf::Slice(
			PIXEL_TYPE,
			reinterpret_cast<char*>(&pixels[0][0].r),
			sizeof(pixels[0][0]) * 1,
			sizeof(pixels[0][0]) * dataWidth));
	frameBuffer.insert(
		"G",
		Imf::Slice(
			PIXEL_TYPE,
			reinterpret_cast<char*>(&pixels[0][0].g),
			sizeof(pixels[0][0]) * 1,
			sizeof(pixels[0][0]) * dataWidth));
	frameBuffer.insert(
		"B",
		Imf::Slice(
			PIXEL_TYPE,
			reinterpret_cast<char*>(&pixels[0][0].b),
			sizeof(pixels[0][0]) * 1,
			sizeof(pixels[0][0]) * dataWidth));

	file.setFrameBuffer(frameBuffer);
	file.writePixels(dataHeight);

	return true;
}

}// end namespace

bool ExrFileWriter::save(const HdrRgbFrame& frame, std::string& byteBuffer)
{
	using DatumType = half;

	static_assert(std::is_same_v<DatumType, half> || std::is_same_v<DatumType, float>);

	constexpr Imf::PixelType PIXEL_TYPE = std::is_same_v<DatumType, half> ?
		Imf::PixelType::HALF : Imf::PixelType::FLOAT;

	logger.log(ELogLevel::NOTE_MIN,
		std::string("saving standalone RGB to memory ") + 
		"(" + (std::is_same_v<DatumType, half> ? "half" : "full") + " precision)");

	try
	{

		const int dataWidth = static_cast<int>(frame.widthPx());
		const int dataHeight = static_cast<int>(frame.heightPx());

		Imf::Header header(dataWidth, dataHeight);
		header.channels().insert("R", Imf::Channel(PIXEL_TYPE));
		header.channels().insert("G", Imf::Channel(PIXEL_TYPE));
		header.channels().insert("B", Imf::Channel(PIXEL_TYPE));

		Imf::StdOSStream stream;
		Imf::OutputFile file(stream, header);

		struct RgbPixel
		{
			DatumType r, g, b;
		};

		Imf::Array2D<RgbPixel> pixels(dataHeight, dataWidth);

		frame.forEachPixel(
			[&pixels, height = (long)dataHeight]
		(const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& pixel)
		{
			RgbPixel& dataPixel = pixels[height - 1 - (long)y][(long)x];

			dataPixel.r = pixel[0];
			dataPixel.g = pixel[1];
			dataPixel.b = pixel[2];
		});

		Imf::FrameBuffer frameBuffer;
		frameBuffer.insert(
			"R",
			Imf::Slice(
				PIXEL_TYPE,
				reinterpret_cast<char*>(&pixels[0][0].r),
				sizeof(pixels[0][0]) * 1,
				sizeof(pixels[0][0]) * dataWidth));
		frameBuffer.insert(
			"G",
			Imf::Slice(
				PIXEL_TYPE,
				reinterpret_cast<char*>(&pixels[0][0].g),
				sizeof(pixels[0][0]) * 1,
				sizeof(pixels[0][0]) * dataWidth));
		frameBuffer.insert(
			"B",
			Imf::Slice(
				PIXEL_TYPE,
				reinterpret_cast<char*>(&pixels[0][0].b),
				sizeof(pixels[0][0]) * 1,
				sizeof(pixels[0][0]) * dataWidth));

		file.setFrameBuffer(frameBuffer);
		file.writePixels(dataHeight);

		byteBuffer.append(stream.str());

		return true;
	}
	catch(const std::exception& e)
	{
		logger.log(ELogLevel::WARNING_MED,
			std::string("failed saving exr file to memroy, reason: ") + e.what());
		return false;
	}
}

}// end namespace ph
