#pragma once

#include "Core/Intersectable/PBasicSphere.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/Transform/StaticRigidTransform.h"

namespace ph
{

/*! @brief A sphere specifically built for environment lighting.
This primitive is for environment map with latitude-longitude format. Unlike regular primitives, 
transformations are done locally rather than wrapping the primitive by another to allow a wider
set of custom operations. Note that the UV for the environment sphere is based on the direction
of incident ray.
*/
class PLatLongEnvSphere : public PBasicSphere
{
public:
	explicit PLatLongEnvSphere(real radius);

	PLatLongEnvSphere(
		real radius, 
		const math::StaticRigidTransform* localToWorld,
		const math::StaticRigidTransform* worldToLocal);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool isOccluding(const Ray& ray) const override;

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcExtendedArea() const override;

	bool latLong01ToSurface(
		const math::Vector2R& latLong01, 
		const math::Vector3R& observationPos,
		math::Vector3R*       out_surface) const;

private:
	const math::StaticRigidTransform* m_localToWorld;
	const math::StaticRigidTransform* m_worldToLocal;
	math::Vector3R m_worldOrigin;
};

inline bool PLatLongEnvSphere::mayOverlapVolume(const math::AABB3D& volume) const
{
	// Rather than transforming `volume` to local space, we transform the sphere to world space 
	// instead. Under static rigid transform the sphere shape is rotational invariant, we can 
	// inversely translate `volume` and test against the sphere at the origin
	auto effectiveVolume = volume.getTranslated(m_worldOrigin.negate());
	return PBasicSphere::mayOverlapVolume(effectiveVolume);
}

inline math::AABB3D PLatLongEnvSphere::calcAABB() const
{
	// Under static rigid transform, only translation will matter to the AABB of a sphere
	return PBasicSphere::calcAABB().translate(m_worldOrigin);
}

inline real PLatLongEnvSphere::calcExtendedArea() const
{
	// Does not change under rigid transform
	return PBasicSphere::calcExtendedArea();
}

}// end namespace ph
