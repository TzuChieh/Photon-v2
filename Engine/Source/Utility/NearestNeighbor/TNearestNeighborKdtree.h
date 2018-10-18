#pragma once

#include "Common/assertion.h"

#include <vector>
#include <utility>
#include <cstddef>

namespace ph
{

template<typename Item, typename DistanceComparator>
class TNearestNeighborKdtree
{
public:
	void build(std::vector<Item>&& items);

	template<typename KNNResult>
	void findKNearest(std::size_t k, KNNResult& results) const;

private:
	std::vector<Item> m_items;
};

// In-header Implementations:

template<typename Item, typename DistanceComparator>
inline void TNearestNeighborKdtree<Item, DistanceComparator>::
	build(std::vector<Item>&& items)
{
	m_items = std::move(items);
	if(m_items.empty())
	{
		return;
	}

	// TODO
}

template<typename Item, typename DistanceComparator>
template<typename KNNResult>
inline void TNearestNeighborKdtree<Item, DistanceComparator>::
	findKNearest(const std::size_t k, KNNResult& results) const
{
	PH_ASSERT(k > 0);

	// TODO
}

}// end namespace ph