#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticTransform.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Core/Intersectable/PrimitiveChannel.h"

#include <memory>
#include <vector>

namespace ph
{

class PrimitiveMetadata final
{
public:
	SurfaceBehavior surfaceBehavior;

	PrimitiveMetadata();

	// Adds a channel to the metadata. Returns a channel ID that can be used
	// to access the channel.
	//
	uint32 addChannel(const PrimitiveChannel& channel);

	inline const PrimitiveChannel& getChannel(const uint32 channelId) const
	{
		if(channelId < m_channels.size())
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

private:
	std::vector<PrimitiveChannel> m_channels;
};

}// end namespace ph