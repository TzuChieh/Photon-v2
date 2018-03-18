#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Common/assertion.h"

#include <iostream>
#include <limits>

namespace ph
{

PrimitiveMetadata::PrimitiveMetadata() :
	surfaceBehavior(),
	m_channels()
{
	addChannel(PrimitiveChannel());
}

uint32 PrimitiveMetadata::addChannel(const PrimitiveChannel& channel)
{
	m_channels.push_back(channel);

	PH_ASSERT(m_channels.size() - 1 <= static_cast<std::size_t>(std::numeric_limits<uint32>::max()));

	return static_cast<uint32>(m_channels.size() - 1);
}

}// end namespace ph