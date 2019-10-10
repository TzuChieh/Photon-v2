#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Core/VolumeBehavior/VolumeBehavior.h"
#include "Common/Logger.h"

#include <memory>
#include <vector>

namespace ph
{

class PrimitiveMetadata final
{
public:
	static constexpr uint32 INVALID_CHANNEL_ID = static_cast<uint32>(-1);
	static constexpr uint32 DEFAULT_CHANNEL_ID = static_cast<uint32>( 0);

	PrimitiveMetadata();

	// Adds a channel to the metadata. Returns a channel ID that can be used
	// to access the just added channel.
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
		return getChannel(DEFAULT_CHANNEL_ID);
	}

	inline bool isChannelIdValid(const uint32 channelId) const
	{
		PH_ASSERT(channelId != INVALID_CHANNEL_ID);

		return channelId < m_channels.size();
	}

	inline SurfaceBehavior&       getSurface()  { return m_surface;  }
	inline VolumeBehavior&        getInterior() { return m_interior; }
	inline VolumeBehavior&        getExterior() { return m_exterior; }

	inline const SurfaceBehavior& getSurface() const  { return m_surface;  }
	inline const VolumeBehavior&  getInterior() const { return m_interior; }
	inline const VolumeBehavior&  getExterior() const { return m_exterior; }

private:
	SurfaceBehavior               m_surface;
	VolumeBehavior                m_interior;
	VolumeBehavior                m_exterior;
	std::vector<PrimitiveChannel> m_channels;

	static const Logger logger;
};

}// end namespace ph
