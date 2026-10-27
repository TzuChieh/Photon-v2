#pragma once

#include "Engine/Core/Intersection/BVH/TBinaryBvhIntersector.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/Algorithm/BVH/TBvhBuilder.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Math/math.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <optional>

namespace ph
{

template<typename Index>
inline void TBinaryBvhIntersector<Index>
::update(TSpanView<const Intersectable*> intersectables)
{
	rebuildWithIntersectables(intersectables);
}

template<typename Index>
inline bool TBinaryBvhIntersector<Index>
::isIntersecting(HitEvent& event) const
{
	return m_bvh.nearestTraversal(
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
inline bool TBinaryBvhIntersector<Index>
::isOccluding(const Ray& ray) const
{
	return m_bvh.occlusionTraversal(
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
inline auto TBinaryBvhIntersector<Index>
::calcAABB() const
-> math::AABB3D
{
	if(m_bvh.isEmpty())
	{
		return math::AABB3D::makeEmpty();
	}

	return m_bvh.getRoot().getAABB();
}

template<typename Index>
inline void TBinaryBvhIntersector<Index>
::forEachIntersection(const Ray& ray, const HitVisitor& visitor) const
{
	m_bvh.exhaustiveTraversal(
		ray.getSegment(),
		[&ray, &visitor](const Intersectable* intersectable, const math::TLineSegment<real>& segment)
		{
			PH_ASSERT(intersectable);

			forEachIntersectionInTarget(*intersectable, Ray(segment, ray.getTime()), visitor);
		});
}

template<typename Index>
inline bool TBinaryBvhIntersector<Index>::supportsForEachIntersection() const
{
	return true;
}

template<typename Index>
inline void TBinaryBvhIntersector<Index>
::rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables)
{
	constexpr auto itemToAABB =
		[](const Intersectable* item)
		{
			return item->calcAABB();
		};

	math::TBvhBuilder<2, const Intersectable*, decltype(itemToAABB)> builder{};

	auto const rootInfoNode = builder.buildInformativeBvh(intersectables);
	m_bvh.build(rootInfoNode, builder.totalInfoNodes(), builder.totalItems());

	// Check the constructed linear BVH and print some information
#if PH_DEBUG
	const std::size_t treeDepth = builder.calcMaxDepth(rootInfoNode);
	
	PH_DEFAULT_LOG(Note,
		"intersector: Binary BVH ({}-byte index), total intersectables: {}, total nodes: {}, "
		"max tree depth: {}, memory usage: {} GiB", sizeof(Index), m_bvh.numItems(),
		m_bvh.numNodes(), treeDepth, math::bytes_to_GiB<double>(m_bvh.memoryUsage()));

	if(treeDepth > m_bvh.TRAVERSAL_STACK_SIZE)
	{
		PH_DEFAULT_LOG(Error,
			"BVH depth ({}) exceeds traversal stack size ({})",
			treeDepth, m_bvh.TRAVERSAL_STACK_SIZE);
	}
#endif
}

}// end namespace ph
