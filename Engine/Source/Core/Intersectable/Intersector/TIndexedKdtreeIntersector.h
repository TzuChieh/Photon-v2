#pragma once

#include "Core/Intersectable/Intersector.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Intersectable.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.h"

#include <vector>

namespace ph
{

template<typename Index = uint32>
class TIndexedKdtreeIntersector : public Intersector
{
public:
	explicit TIndexedKdtreeIntersector(math::IndexedKdtreeParams params = math::IndexedKdtreeParams());

	void update(const CookedDataStorage& cookedActors) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	void calcAABB(math::AABB3D* out_aabb) const override;

private:
	struct IndexedIntersectables
	{
		std::vector<const Intersectable*> vec;

		const Intersectable* operator () (const std::size_t index) const
		{
			PH_ASSERT_LT(index, vec.size());

			return vec[index];
		}
	};

	struct IntersectableAABBCalculator
	{
		math::AABB3D operator () (const Intersectable* const intersectable) const
		{
			PH_ASSERT(intersectable);

			math::AABB3D aabb;
			intersectable->calcAABB(&aabb);
			return aabb;
		}
	};

	using Tree = math::TIndexedKdtree<
		IndexedIntersectables, 
		IntersectableAABBCalculator, 
		Index>;

	Tree                      m_tree;
	math::IndexedKdtreeParams m_params;
};

}// end namespace ph

#include "Core/Intersectable/Intersector/TIndexedKdtreeIntersector.ipp"
