#pragma once

#include "Core/Intersection/Intersectable.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB3D.h"

#include <Common/primitive_type.h>

namespace ph
{

class Ray;
class HitProbe;
class HitDetail;
class PrimitiveMetadata;
class PrimitivePosSampleQuery;
class SampleFlow;

/*! @brief A physical shape in the scene. 
*/
class Primitive : public Intersectable
{
public:
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override = 0;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override = 0;

	math::AABB3D calcAABB() const override = 0;

	/*! @brief Generates a sample point on the surface of this primitive.
	*/
	virtual void genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const;

	/*! @brief Given a point on the surface of this primitive, calculates the PDF of sampling this point.
	*/
	virtual real calcPositionSamplePdfA(const math::Vector3R& position) const;

	/*! @brief Calculates the area extended by this primitive.
	
	The term "extended" implies single-sided, e.g., a triangle's extended area is half the absolute
	value of the cross product of its two edge vectors. To treat it as double-sided, you need to
	multiply the result by 2 manually.

	A zero return value means the concept of extended area does not apply to this primitive.
	*/
	virtual real calcExtendedArea() const;

	// TODO: make this method for EmitablePrimitive
	// This method calculates the position mapped to the specified uvw 
	// coordinates. This kind of inverse mapping may not be always possible; 
	// if the mapping failed, false is returned.
	virtual bool uvwToPosition(
		const math::Vector3R& uvw,
		math::Vector3R*       out_position) const;

	virtual const PrimitiveMetadata* getMetadata() const;
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

inline bool Primitive::uvwToPosition(
	const math::Vector3R& /* uvw */,
	math::Vector3R*       /* out_position */) const
{
	return false;
}

inline const PrimitiveMetadata* Primitive::getMetadata() const
{
	return nullptr;
}

}// end namespace ph
