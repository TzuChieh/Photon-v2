#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB3D.h"

namespace ph
{

class Ray;
class HitProbe;
class HitDetail;
class PrimitiveMetadata;
class PositionSample;

class Primitive : public Intersectable
{
public:
	explicit Primitive(const PrimitiveMetadata* metadata);

	using Intersectable::isIntersecting;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override = 0;
	void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                            HitDetail* out_detail) const override = 0;

	bool isIntersectingVolumeConservative(const math::AABB3D& volume) const override = 0;
	void calcAABB(math::AABB3D* out_aabb) const override = 0;

	// Generates a sample point on the surface of this primitive. 
	//
	virtual void genPositionSample(PositionSample* out_sample) const;

	// Given a point on the surface of this primitive, calculates the PDF of
	// sampling this point.
	//
	virtual real calcPositionSamplePdfA(const math::Vector3R& position) const;

	// Calculates the area extended by this primitive. The term "extended"
	// implies single-sided, e.g., a triangle's extended area is the absolute
	// value of the cross product of its two edge vectors, no need to multiply
	// by 2 for two sides. A zero return value means the concept of extended
	// area does not apply to this primitive.
	//
	virtual real calcExtendedArea() const;

	// TODO: make this method for EmitablePrimitive
	// This method calculates the position mapped to the specified uvw 
	// coordinates. This kind of inverse mapping may not be always possible; 
	// if the mapping failed, false is returned. 
	//
	virtual bool uvwToPosition(
		const math::Vector3R& uvw,
		const math::Vector3R& observationPoint,
		math::Vector3R*       out_position) const;

	const PrimitiveMetadata* getMetadata() const;

protected:
	const PrimitiveMetadata* m_metadata;
};

// In-header Implementation:

inline real Primitive::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	return 0.0_r;
}

inline real Primitive::calcExtendedArea() const
{
	return 0.0_r;
}

inline const PrimitiveMetadata* Primitive::getMetadata() const
{
	return m_metadata;
}

inline bool Primitive::uvwToPosition(
	const math::Vector3R& /* uvw */,
	const math::Vector3R& /* observationPoint */,
	math::Vector3R*       /* out_position */) const
{
	return false;
}

}// end namespace ph
