#include "DataIO/EXR/ExrFileReader.h"
#include "Utility/utility.h"
#include "Frame/TFrame.h"
#include "DataIO/EXR/exr_io_common.h"

#include "Common/ThirdParty/lib_openexr.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/exception.h>
#include <Common/profiling.h>

#include <type_traits>
#include <string>
#include <array>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ExrFileReader, DataIO);

ExrFileReader::ExrFileReader(const Path& filePath) :
	m_filePath(filePath)
{}

void ExrFileReader::load(HdrRgbFrame* const out_frame)
{
	loadFromFilesystem(out_frame);
}

void ExrFileReader::loadFromFilesystem(
	HdrRgbFrame* out_frame,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName)
{
	PH_PROFILE_SCOPE();

	loadStandaloneImageData(
		m_filePath,
		out_frame,
		redChannelName,
		greenChannelName,
		blueChannelName);
}

void ExrFileReader::loadStandaloneImageData(
	const Path filePath,
	HdrRgbFrame* out_frame,
	std::string_view redChannelName,
	std::string_view greenChannelName,
	std::string_view blueChannelName)
{
	PH_ASSERT(out_frame);

	Imf::InputFile file(filePath.toNativeString().c_str());
	const Imf::Header& header = file.header();

	using ChannelNames = std::array<std::string_view, 3>;
	using Channels = std::array<const Imf::Channel*, 3>;

	const ChannelNames channelNames{redChannelName, greenChannelName, blueChannelName};
	const Channels channels = find_imf_channels<3>(
		file.header(),
		{redChannelName, greenChannelName, blueChannelName});

	if(channels == Channels{nullptr, nullptr, nullptr})
	{
		PH_LOG_WARNING(ExrFileReader,
			"All standalone channels are not found;\n"
			"available layers and channels in {} are: {}",
			filePath, list_all_imf_layers_and_channels(header));
		out_frame->setSize(0, 0);
		return;
	}

	// Coordinates are descrete, hence the +1 in the end
	const Imath::Box2i dataWindow = header.dataWindow();
	const int dataWidth = dataWindow.max.x - dataWindow.min.x + 1;
	const int dataHeight = dataWindow.max.y - dataWindow.min.y + 1;

	out_frame->setSize(dataWidth, dataHeight);

	const bool isAllFloatChannel = 
		(!channels[0] || channels[0]->type == Imf::FLOAT) &&
		(!channels[1] || channels[1]->type == Imf::FLOAT) &&
		(!channels[2] || channels[2]->type == Imf::FLOAT);

	// Fastest route: directly map data to output `frame`
	if(isAllFloatChannel)
	{
		Imf::FrameBuffer framebuffer;
		map_imf_framebuffer_to_frame(
			framebuffer,
			header,
			*out_frame,
			{redChannelName, greenChannelName, blueChannelName});

		file.setFrameBuffer(framebuffer);
		file.readPixels(dataWindow.min.y, dataWindow.max.y);
	}
	// Need to allocate a new frame to store the copied/converted data there.
	// We always do it with an additional RGB frame.
	else
	{
		TFrame<half, 3> halfFrame(dataWidth, dataHeight);

		Imf::FrameBuffer framebuffer;
		for(std::size_t channelIdx = 0; channelIdx < 3; ++channelIdx)
		{
			const Imf::Channel* channel = channels[channelIdx];
			if(!channel)
			{
				continue;
			}

			ChannelNames singleChannelName{};
			singleChannelName[channelIdx] = channelNames[channelIdx];

			if(channel->type == Imf::FLOAT)
			{
				PH_ASSERT((std::is_same_v<HdrRgbFrame::ElementType, float>));

				map_imf_framebuffer_to_frame(
					framebuffer,
					header,
					*out_frame,
					singleChannelName);
			}
			else
			{
				if(channel->type != Imf::HALF)
				{
					PH_LOG_WARNING(ExrFileReader,
						"Unsupported channel value type {} in file {}, ignoring this channel.",
						enum_to_value(channel->type), filePath);
					continue;
				}
				
				map_imf_framebuffer_to_frame(
					framebuffer,
					header,
					halfFrame,
					singleChannelName);
			}
		}

		file.setFrameBuffer(framebuffer);
		file.readPixels(dataWindow.min.y, dataWindow.max.y);

		// When using an additional frame of different type, we need to convert data stored
		// in it back to the output frame
		out_frame->forEachPixel(
			[&halfFrame, channels]
			(const uint32 x, const uint32 y, const HdrRgbFrame::PixelType& pixel)
			{
				const auto halfPixel = halfFrame.getPixel({x, y});

				auto combinedPixel = pixel;
				for(std::size_t channelIdx = 0; channelIdx < 3; ++channelIdx)
				{
					const Imf::Channel* channel = channels[channelIdx];
					if(channel && channels[channelIdx]->type == Imf::HALF)
					{
						combinedPixel[channelIdx] = halfPixel[channelIdx];
					}
				}

				return combinedPixel;
			});
	}
}

}// end namespace ph
