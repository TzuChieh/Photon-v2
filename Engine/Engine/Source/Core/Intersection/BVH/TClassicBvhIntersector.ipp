#pragma once

#include "Core/Intersection/BVH/TClassicBvhIntersector.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Math/Algorithm/BVH/TBvhBuilder.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <optional>

namespace ph
{

template<typename Index>
inline void TClassicBvhIntersector<Index>
::update(TSpanView<const Intersectable*> intersectables)
{
	rebuildWithIntersectables(intersectables);
}

template<typename Index>
inline bool TClassicBvhIntersector<Index>
::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	return m_bvh.nearestTraversal(
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
inline auto TClassicBvhIntersector<Index>
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
inline void TClassicBvhIntersector<Index>
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
		"intersector: Classic BVH ({}-byte index), total intersectables: {}, total nodes: {}, "
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
