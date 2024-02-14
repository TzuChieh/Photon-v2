#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/PrimitiveChannel.h"

#include <Common/logging.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PrimitiveMetadata, Primitive);

PrimitiveMetadata::PrimitiveMetadata() :
	m_surface(),
	m_interior(),
	m_exterior(),
	m_channels()
{
	// Adds a default channel.
	addChannel(PrimitiveChannel());
}

uint8 PrimitiveMetadata::addChannel(const PrimitiveChannel& channel)
{
	m_channels.push_back(channel);

	// Making sure the maximum index of the channels does not exceed what
	// a channel ID type can handle.
	PH_ASSERT_LT(m_channels.size() - 1, static_cast<std::size_t>(std::numeric_limits<uint8>::max()));

	return static_cast<uint8>(m_channels.size() - 1);
}

void PrimitiveMetadata::setChannel(const uint8 channelId, PrimitiveChannel channel)
{
	if(isChannelIdValid(channelId))
	{
		m_channels[channelId] = std::move(channel);
	}
	else
	{
		PH_LOG(PrimitiveMetadata, Warning, "channel ID ({}) is invalid", channelId);
	}
}

static_assert(std::is_copy_constructible_v<PrimitiveMetadata>,
	"`PrimitiveMetadata` should be copyable so modified context can be easily created.");

}// end namespace ph
