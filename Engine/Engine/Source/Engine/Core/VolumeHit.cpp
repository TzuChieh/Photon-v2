#include "Engine/Core/VolumeHit.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<VolumeHit>);

VolumeHit::VolumeHit(
	const SurfaceHit& X,
	const Ray& incidentRay,
	const bool isInterior)

	: VolumeHit(
		X.getDetail().getPrimitive(),
		incidentRay,
		X.getPos(),
		isInterior,
		VolumeHitReason(EVolumeHitReason::IncidentRay))
{}

const VolumeOptics* VolumeHit::getInteriorOptics() const
{
	return getPrimitive().getMetadata()->getInterior().getOptics();
}

const VolumeOptics* VolumeHit::getExteriorOptics() const
{
	return getPrimitive().getMetadata()->getExterior().getOptics();
}

}// end namespace ph
