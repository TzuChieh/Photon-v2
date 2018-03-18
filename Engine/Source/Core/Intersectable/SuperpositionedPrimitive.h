#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"

#include <vector>

namespace ph
{

/*
	A collective of primitives that are each other's superposition. 
	Primitives are said to be superpositioning if they represents 
	the same primitive and are very similar to each other in shape.
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

	virtual ~SuperpositionedPrimitive() override;

	virtual bool isIntersecting(const Ray& ray) const override;
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	virtual bool isIntersectingVolumeConservative(const AABB3D& aabb) const override;

	virtual void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;
	virtual void calcAABB(AABB3D* out_aabb) const override;
	virtual real calcPositionSamplePdfA(const Vector3R& position) const override;
	virtual real calcExtendedArea() const override;

	virtual void genPositionSample(PositionSample* out_sample) const override;

private:
	std::vector<const Primitive*> m_primitives;
	const Primitive*              m_mainPrimitive;
};

}// end namespace ph