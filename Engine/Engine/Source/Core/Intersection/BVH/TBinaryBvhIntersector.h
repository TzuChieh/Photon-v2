#pragma once

#include "Core/Intersection/Intersector.h"
#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.h"

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Classic binary BVH acceleration structure.
@tparam Index Type for buffer offsets. Types that can store a larger integer supports more intersectables.
*/
template<typename Index>
class TBinaryBvhIntersector : public Intersector
{
public:
	void update(TSpanView<const Intersectable*> intersectables) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	math::AABB3D calcAABB() const override;

	void rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables);

private:
	math::TLinearDepthFirstBinaryBvh<const Intersectable*, Index> m_bvh;
};

}// end namespace ph

#include "Core/Intersection/BVH/TBinaryBvhIntersector.ipp"
