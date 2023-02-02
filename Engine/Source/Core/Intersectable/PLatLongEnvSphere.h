#pragma once

#include "Core/Intersectable/PBasicSphere.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"

namespace ph
{

namespace math { class RigidTransform; }

/*! @brief A sphere specifically built for environment lighting.

This primitive is for environment map with latitude-longitude format. Unlike
regular primitives, transformations are done locally rather than wrapping the
primitive by another. This is due to the fact that UV for environment map is
based on incident direction in world space, i.e., mapping UV from local space
incident direction can result in inconsistent lighting on different objects
in world space (as UV is not transformed to world space later). Making this a 
nested primitive is discouraged unless the specification say so.
*/

// TODO: assert on transform wrappers that this class is not applicable
class PLatLongEnvSphere : public PBasicSphere
{
public:
	PLatLongEnvSphere(const PrimitiveMetadata* metadata, real radius);

	PLatLongEnvSphere(
		const PrimitiveMetadata*    metadata, 
		real                        radius, 
		const math::RigidTransform* localToWorld,
		const math::RigidTransform* worldToLocal);

	math::Vector2R positionToUV(const math::Vector3R& position) const override;

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool latLong01ToSurface(
		const math::Vector2R& latLong01, 
		const math::Vector3R& observationPos,
		math::Vector3R*       out_surface) const;

private:
	const math::RigidTransform* m_localToWorld;
	const math::RigidTransform* m_worldToLocal;
};

}// end namespace ph
