#pragma once

#include "Core/Intersection/Intersector.h"
#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.h"

#include <Common/primitive_type.h>

namespace ph
{

template<typename IndexType>
class TClassicBvhIntersector : public Intersector
{
public:
	void update(TSpanView<const Intersectable*> intersectables) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	math::AABB3D calcAABB() const override;

	void rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables);

private:
	math::TLinearDepthFirstBinaryBvh<const Intersectable*, IndexType> m_bvh;
};

}// end namespace ph

#include "Core/Intersection/BVH/TClassicBvhIntersector.ipp"
