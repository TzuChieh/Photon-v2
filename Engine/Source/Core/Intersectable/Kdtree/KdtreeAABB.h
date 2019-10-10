#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Core/Ray.h"
#include "Core/Intersectable/Kdtree/kdtree_core.h"
#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <iostream>

namespace ph
{

class KdtreeAABB final
{
public:
	KdtreeAABB();
	explicit KdtreeAABB(const math::AABB3D& aabb);
	KdtreeAABB(const math::Vector3R& minVertex, const math::Vector3R& maxVertex);

	bool isIntersectingVolume(const Ray& ray, real* const out_rayNearHitDist, real* const out_rayFarHitDist) const;
	bool isIntersectingVolume(const math::AABB3D& aabb) const;
	bool trySplitAt(const int32 axis, const real splitPos, KdtreeAABB* const out_negativeAABB, KdtreeAABB* const out_positiveAABB) const;

	void getMinVertex(real* const out_vector3f) const;
	void getMaxVertex(real* const out_vector3f) const;
	real getMinVertex(const int32 axis) const;
	real getMaxVertex(const int32 axis) const;
	void getAABB(math::AABB3D* const out_aabb) const;

	inline real getExtent(const int32 axis) const
	{
		real minVertex[KDTREE_NUM_AXES];
		real maxVertex[KDTREE_NUM_AXES];
		getMinVertex(minVertex);
		getMaxVertex(maxVertex);
		return maxVertex[axis] - minVertex[axis];
	}

	inline real getSurfaceArea() const
	{
		const real xExtent = m_aabb.getMaxVertex().x - m_aabb.getMinVertex().x;
		const real yExtent = m_aabb.getMaxVertex().y - m_aabb.getMinVertex().y;
		const real zExtent = m_aabb.getMaxVertex().z - m_aabb.getMinVertex().z;
		return 2.0_r * (xExtent * yExtent + yExtent * zExtent + zExtent * xExtent);
	}

private:
	math::AABB3D m_aabb;
};

}// end namespace ph
