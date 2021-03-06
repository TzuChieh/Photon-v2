#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Common/assertion.h"

#include <iostream>
#include <limits>

namespace ph
{

const Logger PrimitiveMetadata::logger(LogSender("Primitive Metadata"));

PrimitiveMetadata::PrimitiveMetadata() :
	m_surface(),
	m_interior(),
	m_exterior(),
	m_channels()
{
	// Adds a default channel.
	//
	addChannel(PrimitiveChannel());
}

uint32 PrimitiveMetadata::addChannel(const PrimitiveChannel& channel)
{
	m_channels.push_back(channel);

	// Making sure the maximum index of the channels does not exceed what
	// a channel ID type can handle.
	//
	PH_ASSERT(m_channels.size() - 1 <= static_cast<std::size_t>(std::numeric_limits<uint32>::max()));

	return static_cast<uint32>(m_channels.size() - 1);
}

void PrimitiveMetadata::setChannel(const uint32 channelId, const PrimitiveChannel& channel)
{
	if(isChannelIdValid(channelId))
	{
		m_channels[channelId] = channel;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
			"channel ID (" + std::to_string(channelId) + ") is invalid");
	}
}

}// end namespace ph