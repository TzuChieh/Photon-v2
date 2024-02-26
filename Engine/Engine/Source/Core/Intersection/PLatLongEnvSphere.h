#pragma once

#include "Core/Intersection/PBasicSphere.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/Transform/StaticRigidTransform.h"

#include <array>

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
	// TODO: currently not fully time aware
public:
	explicit PLatLongEnvSphere(real radius);

	PLatLongEnvSphere(
		real radius, 
		const math::StaticRigidTransform* localToWorld,
		const math::StaticRigidTransform* worldToLocal);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool isOccluding(const Ray& ray) const override;

	void calcHitDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;
	real calcExtendedArea() const override;

	/*! @brief Generates a sample point on the surface of this primitive.
	Basically a hit event generating version of `latLong01ToSurface(3)`.
	@param latLong01 The sampled UV coordinates. Determines the direction of observation.
	@param latLong01Pdf UV space PDF of sampling the UV coordinates.
	@note Generates hit event (with `PrimitivePosSampleOutput::getObservationRay()` and `probe`).
	*/
	void genPosSampleWithObservationPos(
		const math::Vector2R& latLong01,
		real latLong01Pdf,
		PrimitivePosSampleQuery& query,
		HitProbe& probe) const;

	/*!
	@note Generates hit event (with `PrimitivePosSamplePdfInput::getObservationRay()` and `probe`).
	*/
	void calcPosSamplePdfWithObservationPos(
		const math::Vector2R& latLong01,
		real latLong01Pdf,
		PrimitivePosSamplePdfQuery& query,
		HitProbe& probe) const;

	/*! @brief Generates a sample point on the surface of this primitive.
	Basically a hit event generating version of `latLong01ToSurface(4)`.
	@param latLong01 The sampled UV coordinates. Determines the direction of observation.
	@param latLong01Pdf UV space PDF of sampling the UV coordinates.
	@note Generates hit event (with `PrimitivePosSampleOutput::getObservationRay()` and `probe`).
	*/
	void genPosSampleWithoutObservationPos(
		const math::Vector2R& latLong01,
		real latLong01Pdf,
		PrimitivePosSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe,
		math::Vector3R* out_unitObservationDir,
		real* out_pdfW) const;

	/*! @brief Map UV coordinates back to a point on the surface.
	@param latLong01 The sampled UV coordinates. Determines the direction of observation.
	@param observationPos The reference point of this mapping. When a point of reference is given,
	this kind of mapping is one-to-one.
	@param out_surface The point on the surface that is being observed.
	@param out_observationDir The direction of observation from the reference point.
	@return `true` if the mapping is successful. `false` if the mapping cannot be done (e.g.,
	`observationPos` is outside the sphere).
	*/
	bool latLong01ToSurface(
		const math::Vector2R& latLong01,
		const math::Vector3R& observationPos,
		math::Vector3R* out_surface,
		math::Vector3R* out_unitObservationDir) const;

	/*! @brief Map UV coordinates back to a point on the surface.
	@param latLong01 The sampled UV coordinates. Determines the direction of observation.
	@param uniformSample As this kind of mapping (without a reference point) is one-to-many,
	the sample is used for choosing from all possibilities in the direction determined by `latLong01`.
	@param out_surface The point on the surface that is being observed.
	@param out_observationDir The direction of observation from the reference point.
	*/
	void latLong01ToSurface(
		const math::Vector2R& latLong01,
		const std::array<real, 2>& uniformSample,
		math::Vector3R* out_surface,
		math::Vector3R* out_unitObservationDir,
		real* out_pdfA) const;

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
