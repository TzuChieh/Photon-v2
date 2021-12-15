#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"

#include <vector>

namespace ph
{

/*
	A collective of primitives that are each other's superposition. 

	Primitives are said to be superpositioning if they represents 
	the same object in space and are exactly the same to each other 
	in shape. Primitives must also be able to share intersection cache
	(it is best to ensure that they are the same type).

	This primitive will not work well if geometric properties of the
	primitives are not close enough to one another.

	This primitive satisfied the "channel property".
*/
class SuperpositionedPrimitive final : public Primitive
{
public:
	SuperpositionedPrimitive(const PrimitiveMetadata* metadata, 
	                         const Primitive*         mainPrimitive);

	SuperpositionedPrimitive(const PrimitiveMetadata*             metadata, 
	                         const std::vector<const Primitive*>& primitives, 
	                         std::size_t                          mainPrimitiveIndex);

	bool isOccluding(const Ray& ray) const override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool mayIntersectVolume(const math::AABB3D& aabb) const override;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;
	math::AABB3D calcAABB() const override;
	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	real calcExtendedArea() const override;

	void genPositionSample(SampleFlow& sampleFlow, PositionSample* out_sample) const override;

private:
	std::vector<const Primitive*> m_primitives;
	const Primitive*              m_mainPrimitive;
};

}// end namespace ph
