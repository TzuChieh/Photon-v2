#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/Primitive.h"

#include <Common/assertion.h>

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<SurfaceHit>);

namespace
{

inline const PrimitiveMetadata* get_primitive_metadata(const SurfaceHit& surfaceHit)
{
	PH_ASSERT(surfaceHit.getDetail().getPrimitive());

	return surfaceHit.getDetail().getPrimitive()->getMetadata();
}

}// end anonymous namespace

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
	return SurfaceHit(m_incidentRay, newProbe);
}

bool SurfaceHit::hasSurfaceOptics() const
{
	return get_primitive_metadata(*this)->getSurface().getOptics();
}

bool SurfaceHit::hasInteriorOptics() const
{
	return get_primitive_metadata(*this)->getInterior().getOptics();
}

bool SurfaceHit::hasExteriorOptics() const
{
	return get_primitive_metadata(*this)->getExterior().getOptics();
}

}// end namespace ph
