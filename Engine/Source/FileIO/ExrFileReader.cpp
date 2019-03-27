#include "FileIO/ExrFileReader.h"
#include "Common/assertion.h"
#include "Common/Logger.h"

#include "Common/ThirdParty/lib_openexr.h"

#include <set>
#include <iostream>
#include <type_traits>
#include <exception>

namespace ph
{

namespace
{
	const Logger logger(LogSender("EXR File"));

	template<typename DatumType>
	bool loadStandaloneRgbData(Imf::InputFile& file, HdrRgbFrame* const out_frame);
}

ExrFileReader::ExrFileReader(const Path& filePath) :
	m_filePath(filePath)
{}

bool ExrFileReader::load(HdrRgbFrame* const out_frame)
{
	try
	{
		return loadStandaloneRgb(out_frame);
	}
	catch(const std::exception& e)
	{
		logger.log(ELogLevel::WARNING_MED,
			"failed loading <" + m_filePath.toString() + ">, reason: " + e.what());
		return false;
	}
}

bool ExrFileReader::loadStandaloneRgb(HdrRgbFrame* const out_frame)
{
	const std::string filePath = m_filePath.toAbsoluteString();
	
	logger.log(ELogLevel::NOTE_MIN,
		"loading standalone RGB: " + filePath);
	
	Imf::InputFile file(filePath.c_str());

	const Imf::ChannelList& channelList   = file.header().channels();
	const Imf::Channel*     rgbChannels[] = {
		channelList.findChannel("R"),
		channelList.findChannel("G"),
		channelList.findChannel("B")
		};

	if(!rgbChannels[0] || !rgbChannels[1] || !rgbChannels[2])
	{
		logger.log(ELogLevel::WARNING_MED,
			"expecting standalone RGB channels, cannot find some/all of them;\n"
			"all layers and channels: " + listAllLayersAndChannels());
		return false;
	}

	const Imf::PixelType pixelType = rgbChannels[0]->type;
	if(!(rgbChannels[0]->type == rgbChannels[1]->type && rgbChannels[1]->type == rgbChannels[2]->type) ||
	   !(pixelType == Imf::PixelType::HALF || pixelType == Imf::PixelType::FLOAT))
	{
		logger.log(ELogLevel::WARNING_MED,
			"expecting RGB channels have the same floating point type; IDs: "
			"R=" + std::to_string(rgbChannels[0]->type) + ", "
			"G=" + std::to_string(rgbChannels[1]->type) + ", "
			"B=" + std::to_string(rgbChannels[2]->type));
		return false;
	}

	// DEBUG
	std::cerr << pixelType << std::endl;

	PH_ASSERT(pixelType == Imf::PixelType::HALF || pixelType == Imf::PixelType::FLOAT);
	if(pixelType == Imf::PixelType::HALF)
	{
		return loadStandaloneRgbData<half>(file, out_frame);
	}
	else
	{
		return loadStandaloneRgbData<float>(file, out_frame);
	}
}

std::string ExrFileReader::listAllLayersAndChannels()
{
	Imf::InputFile file(m_filePath.toAbsoluteString().c_str());

	const Imf::ChannelList& channels = file.header().channels();

	std::string result;
	for(auto channel = channels.begin(); channel != channels.end(); ++channel)
	{
		const std::string channelName(channel.name());
		if(!channelName.empty() && (
			channelName.front()   == '.' ||
			channelName.back()    == '.' ||
			channelName.find('.') == std::string::npos))
		{
			result += "<standalone channel: " + channelName + ">";
		}
	}

	std::set<std::string> layerNames;
	channels.layers(layerNames);
	
	for(const auto& layerName : layerNames)
	{
		result += "<layer " + layerName + ": ";

		Imf::ChannelList::ConstIterator channelBegin, channelEnd;
		channels.channelsInLayer(layerName, channelBegin, channelEnd);
		for(auto channel = channelBegin; channel != channelEnd; ++channel)
		{
			result += "[" + std::string(channel.name()) + "]";
		}

		result += ">";
	}

	return result;
}

namespace
{

template<typename DatumType>
bool loadStandaloneRgbData(Imf::InputFile& file, HdrRgbFrame* const out_frame)
{
	static_assert(std::is_same_v<DatumType, half> || std::is_same_v<DatumType, float>);

	constexpr Imf::PixelType PIXEL_TYPE = std::is_same_v<DatumType, half> ? 
		Imf::PixelType::HALF : Imf::PixelType::FLOAT;

	PH_ASSERT(out_frame);

	const Imf::Header& header = file.header();

	// coordinates are descrete, hence the +1 in the end
	const Imath::Box2i dataWindow = header.dataWindow();
	const int dataWidth  = dataWindow.max.x - dataWindow.min.x + 1;
	const int dataHeight = dataWindow.max.y - dataWindow.min.y + 1;
	const int minDataX   = dataWindow.min.x;
	const int minDataY   = dataWindow.min.y;

	struct RgbPixel
	{
		DatumType r, g, b;
	};

	Imf::Array2D<RgbPixel> pixels(dataHeight, dataWidth);

	Imf::FrameBuffer frameBuffer;
	frameBuffer.insert(
		"R",
		Imf::Slice(
			PIXEL_TYPE,
			reinterpret_cast<char*>(&pixels[-minDataY][-minDataX].r),
			sizeof(pixels[0][0]) * 1,
			sizeof(pixels[0][0]) * dataWidth,
			1, 1,
			0.0));
	frameBuffer.insert(
		"G",
		Imf::Slice(
			PIXEL_TYPE,
			reinterpret_cast<char*>(&pixels[-minDataY][-minDataX].g),
			sizeof(pixels[0][0]) * 1,
			sizeof(pixels[0][0]) * dataWidth,
			1, 1,
			0.0));
	frameBuffer.insert(
		"B",
		Imf::Slice(
			PIXEL_TYPE,
			reinterpret_cast<char*>(&pixels[-minDataY][-minDataX].b),
			sizeof(pixels[0][0]) * 1,
			sizeof(pixels[0][0]) * dataWidth,
			1, 1,
			0.0));
	file.setFrameBuffer(frameBuffer);
	file.readPixels(dataWindow.min.y, dataWindow.max.y);

	// copy read data to frame

	const Imf::LineOrder lineOrder = header.lineOrder();
	if(!(lineOrder == Imf::LineOrder::INCREASING_Y || lineOrder == Imf::LineOrder::DECREASING_Y))
	{
		logger.log(ELogLevel::WARNING_MED,
			"file < " + std::string(file.fileName()) + "> has unsupported line order: " + std::to_string(lineOrder));
		return false;
	}

	out_frame->setSize(dataWidth, dataHeight);
	out_frame->forEachPixel(
		[&pixels, lineOrder, dataHeight](const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& /* pixel */)
		{
			const RgbPixel& readPixel = lineOrder == Imf::LineOrder::INCREASING_Y ? 
				pixels[dataHeight - 1 - y][x] : pixels[y][x];

			return HdrRgbFrame::Pixel({readPixel.r, readPixel.g, readPixel.b});
		});

	return true;
}

}// end namespace

}// end namespace ph