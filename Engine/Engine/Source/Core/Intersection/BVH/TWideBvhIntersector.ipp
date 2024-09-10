#pragma once

#include "Core/Intersection/BVH/TWideBvhIntersector.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Math/Algorithm/BVH/TBvhBuilder.h"
#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/math_basics.h>

#include <optional>

namespace ph
{

template<std::size_t N, typename Index>
inline void TWideBvhIntersector<N, Index>
::update(TSpanView<const Intersectable*> intersectables)
{
	rebuildWithIntersectables(intersectables);
}

template<std::size_t N, typename Index>
inline bool TWideBvhIntersector<N, Index>
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

template<std::size_t N, typename Index>
inline auto TWideBvhIntersector<N, Index>
::calcAABB() const
-> math::AABB3D
{
	return m_rootAABB;
}

template<std::size_t N, typename Index>
inline void TWideBvhIntersector<N, Index>
::rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables)
{
	constexpr auto itemToAABB =
		[](const Intersectable* item)
		{
			return item->calcAABB();
		};

	math::BvhParams params;
	//params.splitMethod = math::EBvhNodeSplitMethod::EqualItems;
	params.splitMethod = math::EBvhNodeSplitMethod::SAH_Buckets_OneAxis;
	params.numSahBuckets = 32;
	//params.numSahBuckets = 16;

	//math::TBvhBuilder<N, const Intersectable*, decltype(itemToAABB)> builder{params};
	math::TBvhBuilder<2, const Intersectable*, decltype(itemToAABB)> builder{params};

	auto const rootInfoNode = builder.buildInformativeBvh(intersectables);
	m_bvh.build(rootInfoNode, builder.totalInfoNodes(), builder.totalItems());

	// Wide BVH uses fat nodes, we need to calculate root AABB by ourselves
	m_rootAABB = math::AABB3D::makeEmpty();
	if(!m_bvh.isEmpty())
	{
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			m_rootAABB.unionWith(m_bvh.getRoot().getAABB(ci));
		}
	}

	/*if constexpr(math::is_power_of_2(N))
	{
		PH_DEFAULT_LOG(Note, "{}", m_bvh.balancedPow2OrderTableToString());
	}*/

	// Check the constructed BVH and print some information
#if PH_DEBUG
	const std::size_t treeDepth = builder.calcMaxDepth(rootInfoNode);
	
	PH_DEFAULT_LOG(Note,
		"intersector: BVH{} ({}-byte index), total intersectables: {}, total nodes: {}, "
		"max tree depth: {}, memory usage: {} GiB", N, sizeof(Index), m_bvh.numItems(),
		m_bvh.numNodes(), treeDepth, math::bytes_to_GiB<double>(m_bvh.memoryUsage()));

	if(treeDepth > m_bvh.TRAVERSAL_STACK_SIZE)
	{
		PH_DEFAULT_LOG(Error,
			"BVH{} depth ({}) exceeds traversal stack size ({})",
			N, treeDepth, m_bvh.TRAVERSAL_STACK_SIZE);
	}
#endif
}

}// end namespace ph
