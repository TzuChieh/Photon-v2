#pragma once

#include "Model/BoundingVolume/AABB.h"
#include "World/Kdtree/kdtree_core.h"
#include "Common/primitive_type.h"

#include <iostream>

namespace ph
{

class Vector3f;

class KdtreeAABB final
{
public:
	KdtreeAABB();
	explicit KdtreeAABB(const AABB& aabb);
	KdtreeAABB(const Vector3f& minVertex, const Vector3f& maxVertex);

	bool isIntersecting(const Ray& ray, float32* const out_rayNearHitDist, float32* const out_rayFarHitDist) const;
	bool isIntersecting(const AABB& aabb) const;
	bool trySplitAt(const int32 axis, const float32 splitPos, KdtreeAABB* const out_negativeAABB, KdtreeAABB* const out_positiveAABB) const;

	inline void getMinVertex(float32* const out_vector3f) const
	{
		out_vector3f[KDTREE_X_AXIS] = m_aabb.getMinVertex().x;
		out_vector3f[KDTREE_Y_AXIS] = m_aabb.getMinVertex().y;
		out_vector3f[KDTREE_Z_AXIS] = m_aabb.getMinVertex().z;
	}

	inline void getMaxVertex(float32* const out_vector3f) const
	{
		out_vector3f[KDTREE_X_AXIS] = m_aabb.getMaxVertex().x;
		out_vector3f[KDTREE_Y_AXIS] = m_aabb.getMaxVertex().y;
		out_vector3f[KDTREE_Z_AXIS] = m_aabb.getMaxVertex().z;
	}

	inline float32 getExtent(const int32 axis) const
	{
		float32 minVertex[KDTREE_NUM_AXES];
		float32 maxVertex[KDTREE_NUM_AXES];
		getMinVertex(minVertex);
		getMaxVertex(maxVertex);
		return maxVertex[axis] - minVertex[axis];
	}

	inline float32 getSurfaceArea() const
	{
		const float32 xExtent = m_aabb.getMaxVertex().x - m_aabb.getMinVertex().x;
		const float32 yExtent = m_aabb.getMaxVertex().y - m_aabb.getMinVertex().y;
		const float32 zExtent = m_aabb.getMaxVertex().z - m_aabb.getMinVertex().z;
		return 2.0f * (xExtent * yExtent + yExtent * zExtent + zExtent * xExtent);
	}

private:
	AABB m_aabb;
};

}// end namespace ph