#include "DataIO/EXR/exr_io_common.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/ByteBuffer.h"

#include <set>

namespace ph
{

#if PH_THIRD_PARTY_HAS_OPENEXR

void write_imf_framebuffer_to_filesystem(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	const Path& filePath)
{
	Imf::OutputFile output(filePath.toNativeString().c_str(), header);
	output.setFrameBuffer(framebuffer);
	output.writePixels(numScanlines);
}

void write_imf_framebuffer_to_memory(
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

std::string list_all_imf_layers_and_channels(const Imf::Header& header)
{
	const Imf::ChannelList& channels = header.channels();

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

#endif
// end PH_THIRD_PARTY_HAS_OPENEXR

}// end namespace ph
