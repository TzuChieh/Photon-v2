#pragma once

#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeIntersector.h"
#include "Actor/CookedDataStorage.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"

#include <vector>

namespace ph
{

template<typename IndexedKdtree>
void TIndexedKdtreeIntersector<IndexedKdtree>::update(const CookedDataStorage& cookedActors)
{
	std::vector<const Intersectable*> intersectables;
	for(const auto& intersectable : cookedActors.intersectables())
	{
		// HACK
		math::AABB3D aabb;
		intersectable->calcAABB(&aabb);
		if(!aabb.isFiniteVolume())
		{
			continue;
		}

		intersectables.push_back(intersectable.get());
	}
	m_tree.build(std::move(intersectables));
}

template<typename IndexedKdtree>
bool TIndexedKdtreeIntersector<IndexedKdtree>::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	return m_tree.isIntersecting(ray, probe);
}

}// end namespace ph
