#pragma once

#include "Core/Intersectable/Intersector/TIndexedKdtreeIntersector.h"
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
update(std::span<const Intersectable> intersectables)
{
	IndexedIntersectables indexedIntersectables;
	for(const auto& intersectable : intersectables)
	{
		// HACK
		if(!intersectable.calcAABB().isFiniteVolume())
		{
			continue;
		}

		indexedIntersectables.vec.push_back(&intersectable);
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
				if(trialProbe.getHitRayT() < probe.getHitRayT())
				{
					probe = trialProbe;
				}

				return trialProbe.getHitRayT();
			}
			else
			{
				return {};
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
