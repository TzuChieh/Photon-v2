#include "Core/Intersection/TransformedIntersectable.h"
#include "Math/Geometry/TAABB3D.h"

namespace ph
{

TransformedIntersectable::TransformedIntersectable()

	: Intersectable()

	, m_intersectable(nullptr)
	, m_localToWorld (nullptr)
	, m_worldToLocal (nullptr)
{}

TransformedIntersectable::TransformedIntersectable(
	const Intersectable* const   intersectable,
	const math::Transform* const localToWorld,
	const math::Transform* const worldToLocal)

	: m_intersectable(intersectable)
	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
{
	PH_ASSERT(intersectable);
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

bool TransformedIntersectable::mayOverlapVolume(const math::AABB3D& aabb) const
{
	// FIXME: this is broken under timed environment

	math::AABB3D localAABB;
	m_worldToLocal->transform(aabb, &localAABB);
	return m_intersectable->mayOverlapVolume(localAABB);
}

math::AABB3D TransformedIntersectable::calcAABB() const
{
	// FIXME: static intersectable do not need to consider time

	const math::AABB3D localAABB = m_intersectable->calcAABB();
		
	math::AABB3D worldAABB;
	m_localToWorld->transform(localAABB, &worldAABB);

	// TODO: modify time interval base on transform properties or aabb size

	// TODO: motions

	//for(size_t i = 0; i < 101; i++)
	//{
	//	Time time;
	//	time.absoluteS = 0;// HACK
	//	time.relativeS = 0;// HACK
	//	time.relativeT = static_cast<real>(1.0 / 100.0 * i);

	//	AABB3D aabb;
	//	m_localToWorld->transform(localAABB, time, &aabb);
	//	worldAABB.unionWith(aabb);
	//}

	return worldAABB;
}

}// end namespace ph
