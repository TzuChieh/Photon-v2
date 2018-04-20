#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticTransform.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Common/Logger.h"

#include <memory>
#include <vector>

namespace ph
{

class PrimitiveMetadata final
{
public:
	static constexpr uint32 INVALID_ID = static_cast<uint32>(-1);

	SurfaceBehavior surfaceBehavior;

	PrimitiveMetadata();

	// Adds a channel to the metadata. Returns a channel ID that can be used
	// to access the just added channel.
	//
	uint32 addChannel(const PrimitiveChannel& channel);

	void setChannel(uint32 channelId, const PrimitiveChannel& channel);

	inline const PrimitiveChannel& getChannel(const uint32 channelId) const
	{
		if(isChannelIdValid(channelId))
		{
			return m_channels[channelId];
		}
		else
		{
			return getDefaultChannel();
		}
	}

	inline const PrimitiveChannel& getDefaultChannel() const
	{
		return getChannel(0);
	}

	inline bool isChannelIdValid(const uint32 channelId) const
	{
		PH_ASSERT(channelId != INVALID_ID);

		return channelId < m_channels.size();
	}

private:
	std::vector<PrimitiveChannel> m_channels;

	static const Logger logger;
};

}// end namespace ph