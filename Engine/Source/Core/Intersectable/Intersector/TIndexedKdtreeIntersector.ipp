#pragma once

#include "Core/Intersectable/Intersector/TIndexedKdtreeIntersector.h"
#include "World/Foundation/CookedDataStorage.h"
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

update(const CookedDataStorage& cookedActors)
{
	IndexedIntersectables intersectables;
	for(const auto& intersectable : cookedActors.intersectables())
	{
		// HACK
		if(!intersectable->calcAABB().isFiniteVolume())
		{
			continue;
		}

		intersectables.vec.push_back(intersectable.get());
	}

	const std::size_t numIntersectables = intersectables.vec.size();
	m_tree = Tree(
		numIntersectables,
		std::move(intersectables),
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
