#include "Core/Intersectable/Kdtree/KdtreeAABB.h"
#include "Math/TVector3.h"

namespace ph
{

KdtreeAABB::KdtreeAABB() : 
	m_aabb()
{}

KdtreeAABB::KdtreeAABB(const math::AABB3D& aabb) :
	m_aabb(aabb)
{}

KdtreeAABB::KdtreeAABB(const math::Vector3R& minVertex, const math::Vector3R& maxVertex) :
	m_aabb(minVertex, maxVertex)
{}

bool KdtreeAABB::isIntersectingVolume(const Ray& ray, real* const out_rayNearHitDist, real* const out_rayFarHitDist) const
{
	return m_aabb.isIntersectingVolume(ray.getSegment(), out_rayNearHitDist, out_rayFarHitDist);
}

bool KdtreeAABB::isIntersectingVolume(const math::AABB3D& aabb) const
{
	return m_aabb.isIntersectingVolume(aabb);
}

bool KdtreeAABB::trySplitAt(const int32 axis, const real splitPos, KdtreeAABB* const out_negativeAABB, KdtreeAABB* const out_positiveAABB) const
{
	real minVertex[KDTREE_NUM_AXES];
	real maxVertex[KDTREE_NUM_AXES];
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
	out_negativeAABB->m_aabb.setMaxVertex(math::Vector3R(maxVertex[KDTREE_X_AXIS], maxVertex[KDTREE_Y_AXIS], maxVertex[KDTREE_Z_AXIS]));
	out_positiveAABB->m_aabb.setMinVertex(math::Vector3R(minVertex[KDTREE_X_AXIS], minVertex[KDTREE_Y_AXIS], minVertex[KDTREE_Z_AXIS]));

	return true;
}

void KdtreeAABB::getMinVertex(real* const out_vector3f) const
{
	out_vector3f[KDTREE_X_AXIS] = m_aabb.getMinVertex().x;
	out_vector3f[KDTREE_Y_AXIS] = m_aabb.getMinVertex().y;
	out_vector3f[KDTREE_Z_AXIS] = m_aabb.getMinVertex().z;
}

void KdtreeAABB::getMaxVertex(real* const out_vector3f) const
{
	out_vector3f[KDTREE_X_AXIS] = m_aabb.getMaxVertex().x;
	out_vector3f[KDTREE_Y_AXIS] = m_aabb.getMaxVertex().y;
	out_vector3f[KDTREE_Z_AXIS] = m_aabb.getMaxVertex().z;
}

real KdtreeAABB::getMinVertex(const int32 axis) const
{
	real minVertex[KDTREE_NUM_AXES];
	getMinVertex(minVertex);
	return minVertex[axis];
}

real KdtreeAABB::getMaxVertex(const int32 axis) const
{
	real maxVertex[KDTREE_NUM_AXES];
	getMaxVertex(maxVertex);
	return maxVertex[axis];
}

void KdtreeAABB::getAABB(math::AABB3D* const out_aabb) const
{
	*out_aabb = m_aabb;
}

}// end namespace ph
