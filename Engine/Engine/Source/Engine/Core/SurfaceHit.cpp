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

const PrimitiveMetadata& SurfaceHit::getMetadata() const
{
	PH_ASSERT_MSG(m_metadata,
		"Does not make sense to call the method if `SurfaceHit` hits nothing. "
		"You may miss a call to check for valid hit.");

	return *m_metadata;
}

const SurfaceEmitter& SurfaceHit::getSurfaceEmitter() const
{
	return getMetadata().getSurface().getEmitter();
}

const SurfaceOptics& SurfaceHit::getSurfaceOptics() const
{
	return getMetadata().getSurface().getOptics();
}

const VolumeOptics* SurfaceHit::getInteriorOptics() const
{
	return getMetadata().getInterior().getOptics();
}

const VolumeOptics* SurfaceHit::getExteriorOptics() const
{
	return getMetadata().getExterior().getOptics();
}

const PrimitiveMetadata& SurfaceHit::getPrimitiveMetadataRef(const HitDetail& detail)
{
	const Primitive& primitive = getPrimitiveRef(detail);
	return primitive.getMetadata(primitive.toMetadataSlot(detail.getFaceID()));
}

}// end namespace ph
