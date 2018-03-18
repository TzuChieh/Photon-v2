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

	uint32 addChannel(const PrimitiveChannel& channel);

	inline const PrimitiveChannel& getChannel(const uint32 channelId) const
	{
		return m_channels[channelId];
	}

	inline const PrimitiveChannel& getDefaultChannel() const
	{
		return getChannel(0);
	}

private:
	std::vector<PrimitiveChannel> m_channels;
};

}// end namespace ph