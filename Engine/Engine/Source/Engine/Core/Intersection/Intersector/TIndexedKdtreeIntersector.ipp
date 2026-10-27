#pragma once

#include "Engine/Core/Intersection/Intersector/TIndexedKdtreeIntersector.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitEvent.h"

#include <optional>
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
isIntersecting(HitEvent& event) const
-> bool
{
	return m_tree.nearestTraversal(
		event.getRay().getSegment(),
		[&event, originalEvent = event](
			const Intersectable* const      intersectable,
			const math::TLineSegment<real>& segment)
		-> std::optional<real>
		{
			PH_ASSERT(intersectable);

			HitEvent trialEvent(segment, originalEvent);
			if(intersectable->isIntersecting(trialEvent))
			{
				event.copyHitAndKeepRay(trialEvent);
				return trialEvent.getHitRayT();
			}
			else
			{
				return std::nullopt;
			}
		});
}

template<typename Index>
inline auto TIndexedKdtreeIntersector<Index>::
isOccluding(const Ray& ray) const
-> bool
{
	return m_tree.occlusionTraversal(
		ray.getSegment(),
		[time = ray.getTime()](
			const Intersectable* const      intersectable,
			const math::TLineSegment<real>& segment)
		-> std::optional<real>
		{
			PH_ASSERT(intersectable);

			return intersectable->isOccluding(Ray(segment, time))
				? std::make_optional(segment.getMinT())
				: std::nullopt;
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
