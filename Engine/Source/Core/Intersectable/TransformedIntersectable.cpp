#include "Core/Intersectable/TransformedIntersectable.h"

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
	return m_intersectable->isIntersecting(ray, out_intersection);
}

bool TransformedIntersectable::isIntersecting(const Ray& ray) const
{
	return m_intersectable->isIntersecting(ray);
}

bool TransformedIntersectable::isIntersectingVolumeConservative(const AABB& aabb) const
{
	return m_intersectable->isIntersectingVolumeConservative(aabb);
}

void TransformedIntersectable::calcAABB(AABB* out_aabb) const
{
	return m_intersectable->calcAABB(out_aabb);
}

TransformedIntersectable& TransformedIntersectable::operator = (TransformedIntersectable&& rhs)
{
	m_intersectable = std::move(rhs.m_intersectable);
	m_localToWorld  = std::move(rhs.m_localToWorld);
	m_worldToLocal  = std::move(rhs.m_worldToLocal);

	return *this;
}

}// end namespace ph