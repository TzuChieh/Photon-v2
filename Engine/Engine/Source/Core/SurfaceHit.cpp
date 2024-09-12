#include "Core/SurfaceHit.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"

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
	PH_ASSERT_MSG(surfaceHit.getDetail().getPrimitive(),
		"Does not make sense to call the method if `surfaceHit` hits nothing.");

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
	return SurfaceHit(m_ray, newProbe, m_reason);
}

bool SurfaceHit::hasSurfaceOptics() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getSurface().getOptics() != nullptr : false;
}

bool SurfaceHit::hasInteriorOptics() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getInterior().getOptics() != nullptr : false;
}

bool SurfaceHit::hasExteriorOptics() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getExterior().getOptics() != nullptr : false;
}

const Emitter* SurfaceHit::getSurfaceEmitter() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getSurface().getEmitter() : nullptr;
}

const SurfaceOptics* SurfaceHit::getSurfaceOptics() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getSurface().getOptics() : nullptr;
}

const VolumeOptics* SurfaceHit::getInteriorOptics() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getInterior().getOptics() : nullptr;
}

const VolumeOptics* SurfaceHit::getExteriorOptics() const
{
	auto const meta = get_primitive_metadata(*this);
	return meta ? meta->getExterior().getOptics() : nullptr;
}

}// end namespace ph
