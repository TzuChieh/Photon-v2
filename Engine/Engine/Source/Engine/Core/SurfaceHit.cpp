#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<SurfaceHit>);

SurfaceHit SurfaceHit::switchChannel(const uint32 newChannel) const
{
	// Since channel switching is fairly expensive, do not perform a redundant
	// switch if we are already on the target channel.
	//
	if(newChannel == getChannel())
	{
		return *this;
	}

	HitEvent newEvent = m_event;
	newEvent.setChannel(static_cast<uint8>(newChannel));

	HitDetail newDetail;
	newEvent.calcHitDetail(&newDetail);
	if(m_hasFullHitDetail)
	{
		newDetail.computeBases();
	}

	return SurfaceHit(
		newEvent,
		newDetail,
		m_metadata,
		m_hasFullHitDetail);
}

const PrimitiveMetadata& SurfaceHit::getPrimitiveMetadataRef(const HitDetail& detail)
{
	const Primitive& primitive = getPrimitiveRef(detail);
	return primitive.getMetadata(primitive.toMetadataSlot(detail.getFaceID()));
}

}// end namespace ph
