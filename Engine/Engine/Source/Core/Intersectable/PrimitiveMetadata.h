#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Core/VolumeBehavior/VolumeBehavior.h"

#include <Common/assertion.h>

#include <memory>
#include <vector>

namespace ph
{

class PrimitiveMetadata final
{
public:
	static constexpr auto INVALID_CHANNEL_ID = static_cast<uint8>(-1);
	static constexpr auto DEFAULT_CHANNEL_ID = static_cast<uint8>( 0);

	PrimitiveMetadata();

	/*! @brief Adds a channel to the metadata.
	@return The channel's ID. Can be used to access the just-added channel.
	*/
	uint8 addChannel(const PrimitiveChannel& channel);

	void setChannel(uint8 channelId, PrimitiveChannel channel);
	const PrimitiveChannel& getChannel(uint8 channelId) const;
	const PrimitiveChannel& getDefaultChannel() const;
	bool isChannelIdValid(uint8 channelId) const;

	SurfaceBehavior& getSurface();
	VolumeBehavior& getInterior();
	VolumeBehavior& getExterior();

	const SurfaceBehavior& getSurface() const;
	const VolumeBehavior& getInterior() const;
	const VolumeBehavior& getExterior() const;

private:
	SurfaceBehavior               m_surface;
	VolumeBehavior                m_interior;
	VolumeBehavior                m_exterior;
	std::vector<PrimitiveChannel> m_channels;
};

inline const PrimitiveChannel& PrimitiveMetadata::getChannel(const uint8 channelId) const
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

inline const PrimitiveChannel& PrimitiveMetadata::getDefaultChannel() const
{
	return getChannel(DEFAULT_CHANNEL_ID);
}

inline bool PrimitiveMetadata::isChannelIdValid(const uint8 channelId) const
{
	return channelId < m_channels.size();
}

inline SurfaceBehavior& PrimitiveMetadata::getSurface()
{
	return m_surface;
}

inline VolumeBehavior& PrimitiveMetadata::getInterior()
{
	return m_interior;
}

inline VolumeBehavior& PrimitiveMetadata::getExterior()
{
	return m_exterior;
}

inline const SurfaceBehavior& PrimitiveMetadata::getSurface() const
{
	return m_surface;
}

inline const VolumeBehavior& PrimitiveMetadata::getInterior() const
{
	return m_interior;
}

inline const VolumeBehavior& PrimitiveMetadata::getExterior() const
{
	return m_exterior;
}

}// end namespace ph
