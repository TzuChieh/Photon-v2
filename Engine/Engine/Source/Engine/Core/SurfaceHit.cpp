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
	if(newChannel == m_recordedProbe.getChannel())
	{
		return *this;
	}

	HitProbe newProbe = m_recordedProbe;
	newProbe.setChannel(newChannel);

	if(m_hasFullHitDetail)
	{
		return SurfaceHit(m_ray, newProbe, m_reason);
	}
	else
	{
		HitDetail newDetail;
		newProbe.calcHitDetail(m_ray, &newDetail);

		return SurfaceHit(m_ray, newProbe, newDetail, m_metadata, m_reason, false);
	}
}

const PrimitiveMetadata& SurfaceHit::getPrimitiveMetadataRef(const HitDetail& detail)
{
	const Primitive& primitive = getPrimitiveRef(detail);
	return primitive.getMetadata(primitive.toMetadataSlot(detail.getFaceID()));
}

}// end namespace ph
