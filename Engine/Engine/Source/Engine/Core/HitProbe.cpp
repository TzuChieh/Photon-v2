#include "Engine/Core/HitProbe.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<HitProbe>);

bool HitProbe::isOnDefaultChannel() const
{
	return m_hitDetailChannel == PrimitiveMetadata::DEFAULT_CHANNEL_ID;
}

}// end namespace ph
