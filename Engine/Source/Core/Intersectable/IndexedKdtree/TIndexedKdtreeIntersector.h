#pragma once

#include "Core/Intersectable/Intersector.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtree.h"

namespace ph
{

template<typename IndexedKdtree>
class TIndexedKdtreeIntersector : public Intersector
{
public:
	TIndexedKdtreeIntersector();

	void update(const CookedDataStorage& cookedActors) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	void calcAABB(math::AABB3D* out_aabb) const override;

private:
	IndexedKdtree m_tree;
};

// In-header Implementations:

template<typename IndexedKdtree>
inline TIndexedKdtreeIntersector<IndexedKdtree>::TIndexedKdtreeIntersector() :
	Intersector(),
	m_tree(1, 80, 0.5_r, 1)
{}

template<typename IndexedKdtree>
inline void TIndexedKdtreeIntersector<IndexedKdtree>::calcAABB(math::AABB3D* const out_aabb) const
{
	m_tree.getAABB(out_aabb);
}

}// end namespace ph

#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeIntersector.ipp"
