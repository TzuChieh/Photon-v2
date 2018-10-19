#pragma once

#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/IndexedKdtree/TIndexedKdtreeNode.h"

#include <vector>
#include <utility>
#include <cstddef>

namespace ph
{

template<typename Item, typename T, typename CenterCalculator>
class TNearestNeighborKdtree
{
public:
	using Node = TIndexedKdtreeNode<Item, false>;

	void build(std::vector<Item>&& items);

	template<typename KNNResult>
	void findKNearest(const TVector3<T>& location, std::size_t k, KNNResult& results) const;

private:
	std::vector<Item> m_items;
};

// In-header Implementations:

template<typename Item, typename T, typename CenterCalculator>
inline void TNearestNeighborKdtree<Item, T, CenterCalculator>::
	build(std::vector<Item>&& items)
{
	m_items = std::move(items);
	if(m_items.empty())
	{
		return;
	}

	// TODO
}

template<typename Item, typename T, typename CenterCalculator>
template<typename KNNResult>
inline void TNearestNeighborKdtree<Item, T, CenterCalculator>::
	findKNearest(const TVector3<T>& location, const std::size_t k, KNNResult& results) const
{
	PH_ASSERT(k > 0);

	// TODO
}

}// end namespace ph