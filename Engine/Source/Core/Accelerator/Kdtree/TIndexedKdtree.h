#pragma once

#include "Core/Bound/AABB3D.h"

#include <vector>
#include <utility>

namespace ph
{

template<typename Item, typename Index>
class TIndexedKdtree
{
public:
	TIndexedKdtree(
		std::vector<Item>&& items, 
		int traversalCost, 
		int intersectionCost,
		float emptyBonus,
		std::size_t maxNodeItems, 
		std::size_t maxNodeDepth);

private:
	std::vector<Item> m_items;
	int m_traversalCost;
	int m_intersectionCost;
	float m_emptyBonus;
	std::size_t m_maxNodeItems;
	std::size_t m_maxNodeDepth;
};

// In-header Implementations:

template<typename Item, typename Index>
inline TIndexedKdtree<Item, Index>::TIndexedKdtree(

	std::vector<Item>&& items,
	int traversalCost,
	int intersectionCost,
	float emptyBonus,
	std::size_t maxNodeItems,
	std::size_t maxNodeDepth) :

	m_items(std::move(items)),
	m_traversalCost(traversalCost),
	m_intersectionCost(intersectionCost),
	m_emptyBonus(emptyBonus),
	m_maxNodeItems(maxNodeItems),
	m_maxNodeDepth(maxNodeDepth)
{

}

}// end namespace ph