#pragma once

#include "Core/Intersection/Intersector.h"
#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.h"
#include "Math/Geometry/TAABB3D.h"

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

/*! @brief Wide BVH acceleration structure supporting arbitrary branch factor.
@tparam N Branch factor of the BVH.
@tparam Index Type for buffer offsets. Types that can store a larger integer supports more intersectables.
*/
template<std::size_t N, typename Index>
class TWideBvhIntersector : public Intersector
{
public:
	void update(TSpanView<const Intersectable*> intersectables) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	math::AABB3D calcAABB() const override;

	void rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables);

private:
	math::TLinearDepthFirstWideBvh<N, const Intersectable*, Index> m_bvh;
	math::AABB3D m_rootAABB = math::AABB3D::makeEmpty();
};

}// end namespace ph

#include "Core/Intersection/BVH/TWideBvhIntersector.ipp"
