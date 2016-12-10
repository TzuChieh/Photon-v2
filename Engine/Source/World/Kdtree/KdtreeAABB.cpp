#include "World/Kdtree/KdtreeAABB.h"

namespace ph
{

KdtreeAABB::KdtreeAABB() : 
	m_aabb()
{

}

KdtreeAABB::KdtreeAABB(const AABB& aabb) : 
	m_aabb(aabb)
{

}

KdtreeAABB::KdtreeAABB(const Vector3f& minVertex, const Vector3f& maxVertex) : 
	m_aabb(minVertex, maxVertex)
{

}

bool KdtreeAABB::isIntersecting(const Ray& ray, float32* const out_rayNearHitDist, float32* const out_rayFarHitDist) const
{
	return m_aabb.isIntersecting(ray, out_rayNearHitDist, out_rayFarHitDist);
}

bool KdtreeAABB::isIntersecting(const AABB& aabb) const
{
	return m_aabb.isIntersecting(aabb);
}

bool KdtreeAABB::trySplitAt(const int32 axis, const float32 splitPos, KdtreeAABB* const out_negativeAABB, KdtreeAABB* const out_positiveAABB) const
{
	float32 minVertex[KDTREE_NUM_AXES];
	float32 maxVertex[KDTREE_NUM_AXES];
	getMinVertex(minVertex);
	getMaxVertex(maxVertex);

	// check if the split point is a reasonable one (within parent AABB)
	if(splitPos <= minVertex[axis] || splitPos >= maxVertex[axis])
	{
		return false;
	}

	minVertex[axis] = splitPos;
	maxVertex[axis] = splitPos;

	out_negativeAABB->m_aabb = m_aabb;
	out_positiveAABB->m_aabb = m_aabb;
	out_negativeAABB->m_aabb.setMaxVertex(Vector3f(maxVertex[KDTREE_X_AXIS], maxVertex[KDTREE_Y_AXIS], maxVertex[KDTREE_Z_AXIS]));
	out_positiveAABB->m_aabb.setMinVertex(Vector3f(minVertex[KDTREE_X_AXIS], minVertex[KDTREE_Y_AXIS], minVertex[KDTREE_Z_AXIS]));

	return true;
}

void KdtreeAABB::getMinVertex(float32* const out_vector3f) const
{
	out_vector3f[KDTREE_X_AXIS] = m_aabb.getMinVertex().x;
	out_vector3f[KDTREE_Y_AXIS] = m_aabb.getMinVertex().y;
	out_vector3f[KDTREE_Z_AXIS] = m_aabb.getMinVertex().z;
}

void KdtreeAABB::getMaxVertex(float32* const out_vector3f) const
{
	out_vector3f[KDTREE_X_AXIS] = m_aabb.getMaxVertex().x;
	out_vector3f[KDTREE_Y_AXIS] = m_aabb.getMaxVertex().y;
	out_vector3f[KDTREE_Z_AXIS] = m_aabb.getMaxVertex().z;
}

float32 KdtreeAABB::getMinVertex(const int32 axis) const
{
	float32 minVertex[KDTREE_NUM_AXES];
	getMinVertex(minVertex);
	return minVertex[axis];
}

float32 KdtreeAABB::getMaxVertex(const int32 axis) const
{
	float32 maxVertex[KDTREE_NUM_AXES];
	getMaxVertex(maxVertex);
	return maxVertex[axis];
}

}// end namespace ph