#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class Ray;
class HitProbe;
class HitDetail;
class AABB3D;
class PrimitiveMetadata;
class PositionSample;

class Primitive : public Intersectable
{
public:
	explicit Primitive(const PrimitiveMetadata* metadata);
	~Primitive() override;

	using Intersectable::isIntersecting;
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const = 0;
	virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                                    HitDetail* out_detail) const = 0;

	virtual bool isIntersectingVolumeConservative(const AABB3D& volume) const = 0;
	virtual void calcAABB(AABB3D* out_aabb) const = 0;

	// Generates a sample point on the surface of this primitive. 
	//
	virtual void genPositionSample(PositionSample* out_sample) const;

	// Given a point on the surface of this primitive, calculates the PDF of
	// sampling this point.
	//
	virtual real calcPositionSamplePdfA(const Vector3R& position) const;

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
		const Vector3R& uvw, 
		const Vector3R& observationPoint,
		Vector3R*       out_position) const;

	const PrimitiveMetadata* getMetadata() const;

protected:
	const PrimitiveMetadata* m_metadata;
};

// In-header Implementation:

inline real Primitive::calcPositionSamplePdfA(const Vector3R& position) const
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
	const Vector3R& /* uvw */,
	const Vector3R& /* observationPoint */,
	Vector3R*       /* out_position */) const
{
	return false;
}

}// end namespace ph