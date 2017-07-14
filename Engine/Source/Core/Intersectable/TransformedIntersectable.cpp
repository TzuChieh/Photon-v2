#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Core/BoundingVolume/AABB.h"

namespace ph
{

TransformedIntersectable::TransformedIntersectable(std::unique_ptr<Intersectable> intersectable,
                                                   std::unique_ptr<Transform>     localToWorld,
                                                   std::unique_ptr<Transform>     worldToLocal) :
	m_intersectable(std::move(intersectable)),
	m_localToWorld(std::move(localToWorld)),
	m_worldToLocal(std::move(worldToLocal))
{

}

TransformedIntersectable::TransformedIntersectable(TransformedIntersectable&& other) : 
	m_intersectable(std::move(other.m_intersectable)),
	m_localToWorld(std::move(other.m_localToWorld)),
	m_worldToLocal(std::move(other.m_worldToLocal))
{

}

TransformedIntersectable::~TransformedIntersectable() = default;

bool TransformedIntersectable::isIntersecting(const Ray& ray,
                                              Intersection* const out_intersection) const
{
	const Time& time = ray.getTime();

	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	Intersection localIntersection;
	const bool isIntersecting = m_intersectable->isIntersecting(localRay, &localIntersection);
	m_localToWorld->transform(localIntersection, time, out_intersection);

	return isIntersecting;
}

bool TransformedIntersectable::isIntersecting(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	return m_intersectable->isIntersecting(localRay);
}

// FIXME: this is broken under timed environment
bool TransformedIntersectable::isIntersectingVolumeConservative(const AABB& aabb) const
{
	AABB localAABB;
	m_worldToLocal->transform(aabb, &localAABB);

	return m_intersectable->isIntersectingVolumeConservative(localAABB);
}

void TransformedIntersectable::calcAABB(AABB* const out_aabb) const
{
	AABB localAABB;
	AABB worldAABB;
	m_intersectable->calcAABB(&localAABB);
	m_localToWorld->transform(localAABB, &worldAABB);

	// TODO: modify time interval base on transform properties or aabb size

	for(size_t i = 0; i < 101; i++)
	{
		Time time;
		time.absoluteS = 0;// HACK
		time.relativeS = 0;// HACK
		time.relativeT = static_cast<real>(1.0 / 100.0 * i);

		AABB aabb;
		m_localToWorld->transform(localAABB, time, &aabb);
		worldAABB.unionWith(aabb);
	}

	*out_aabb = worldAABB;
}

TransformedIntersectable& TransformedIntersectable::operator = (TransformedIntersectable&& rhs)
{
	m_intersectable = std::move(rhs.m_intersectable);
	m_localToWorld  = std::move(rhs.m_localToWorld);
	m_worldToLocal  = std::move(rhs.m_worldToLocal);

	return *this;
}

}// end namespace ph