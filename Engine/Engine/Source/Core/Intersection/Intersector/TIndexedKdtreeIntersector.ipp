#pragma once

#include "Core/Intersection/Intersector/TIndexedKdtreeIntersector.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"

#include <utility>

namespace ph
{

template<typename Index>
inline TIndexedKdtreeIntersector<Index>::
TIndexedKdtreeIntersector(const math::IndexedKdtreeParams params) :

	Intersector(),

	m_tree(
		0, 
		IndexedIntersectables(),
		IntersectableAABBCalculator(),
		params),
	m_params(params)
{}

template<typename Index>
inline void TIndexedKdtreeIntersector<Index>::
update(TSpanView<const Intersectable*> intersectables)
{
	IndexedIntersectables indexedIntersectables;
	for(const Intersectable* intersectable : intersectables)
	{
		indexedIntersectables.vec.push_back(intersectable);
	}

	const std::size_t numIntersectables = indexedIntersectables.vec.size();
	m_tree = Tree(
		numIntersectables,
		std::move(indexedIntersectables),
		IntersectableAABBCalculator(),
		m_params);
}

template<typename Index>
inline auto TIndexedKdtreeIntersector<Index>::
isIntersecting(const Ray& ray, HitProbe& probe) const
-> bool
{
	return m_tree.nearestTraversal(
		ray.getSegment(), 
		[ray, &probe, originalProbe = probe](
			const Intersectable* const      intersectable,
			const math::TLineSegment<real>& segment)
		-> std::optional<real>
		{
			PH_ASSERT(intersectable);

			const Ray raySegment(segment, ray.getTime());

			HitProbe trialProbe = originalProbe;
			if(intersectable->isIntersecting(raySegment, trialProbe))
			{
				probe = trialProbe;
				return trialProbe.getHitRayT();
			}
			else
			{
				return std::nullopt;
			}
		});
}

template<typename Index>
inline auto TIndexedKdtreeIntersector<Index>::
calcAABB() const
-> math::AABB3D
{
	return m_tree.getAABB();
}

}// end namespace ph
