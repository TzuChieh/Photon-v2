#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

class Ray;
class HitProbe;
class HitDetail;
class PrimitiveMetadata;
class PrimitivePosSampleQuery;
class PrimitivePosPdfQuery;
class SampleFlow;

/*! @brief A physical shape in the scene. 
*/
class Primitive : public Intersectable
{
public:
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override = 0;

	void calcHitDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override = 0;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override = 0;

	math::AABB3D calcAABB() const override = 0;

	/*! @brief Generates a sample point on the surface of this primitive.
	@note Generates hit event (with `PrimitivePosSampleOutput::getObservationRay()` and `probe`).
	*/
	virtual void genPosSample(
		PrimitivePosSampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const;

	/*! @brief Given a point on the surface of this primitive, calculates the PDF of sampling this point.
	*/
	virtual void calcPosPdf(PrimitivePosPdfQuery& query) const;

	/*! @brief Calculates the area extended by this primitive.
	The term "extended" implies single-sided, e.g., a triangle's extended area is half the absolute
	value of the cross product of its two edge vectors. To treat it as double-sided, you need to
	multiply the result by 2 manually.
	@return The extended area of this primitive. A zero return value means the concept of extended
	area does not apply to this primitive.
	*/
	virtual real calcExtendedArea() const;

	/*! @brief Retrieve raw attribute values associated with a face.
	@param domain Requested indexing domain. The face domain has one value. The vertex domain has
	one value per face vertex, returned in face order.
	@param faceID ID of the face whose attribute values are requested.
	@param out_values Storage for the values. An empty span performs a count-only query.
	@return Number of available values. Returns 0 if the attribute is unavailable in @p domain. If
	@p out_values is too small, returns the required size without writing any values.
	@note Attributes are opaque data, not inherently spatial, so no coordinate-space conversion
	is applied.
	*/
	[[nodiscard]]
	virtual std::size_t getAttribute(
		EPrimitiveAttribute attribute,
		EAttributeDomain domain,
		uint64 faceID,
		TSpan<math::Vector3R> out_values) const;

	virtual uint32 numMetadataSlots() const;

	virtual uint32 toMetadataSlot(uint64 faceID) const;

	/*!
	@return The metadata associated to this primitive at a slot.
	*/
	virtual const PrimitiveMetadata& getMetadata(uint32 slot) const;
};

// In-header Implementation:

inline std::size_t Primitive::getAttribute(
	const EPrimitiveAttribute /* attribute */,
	const EAttributeDomain /* domain */,
	const uint64 /* faceID */,
	const TSpan<math::Vector3R> /* out_values */) const
{
	return 0;
}

inline uint32 Primitive::numMetadataSlots() const
{
	return 1;
}

inline uint32 Primitive::toMetadataSlot(uint64 /* faceID */) const
{
	// Default slot is 0
	return 0;
}

inline real Primitive::calcExtendedArea() const
{
	return 0.0_r;
}

}// end namespace ph
