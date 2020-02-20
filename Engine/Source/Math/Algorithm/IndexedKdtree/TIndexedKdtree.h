#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Utility/utility.h"
#include "Math/Algorithm/IndexedKdtree/IndexedItemEndpoint.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Common/assertion.h"
#include "Math/Geometry/TLineSegment.h"
#include "Math/Algorithm/IndexedKdtree/IndexedKdtreeParams.h"

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>

namespace ph::math
{

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index = std::size_t>
class TIndexedKdtree final
{
private:
	using Node = TIndexedKdtreeNode<Index>;
	using Item = decltype(std::declval<IndexToItem>()(std::declval<Index>()));

public:
	TIndexedKdtree(
		std::size_t         numItems,
		IndexToItem         indexToItem,
		ItemToAABB          itemToAABB,
		IndexedKdtreeParams params = IndexedKdtreeParams());

	template<typename ItemSegmentIntersector>
	bool nearestTraversal(const TLineSegment<real>& segment, ItemSegmentIntersector&& intersetor) const;

	AABB3D getAABB() const;
	bool isEmpty() const;

private:
	void build(ItemToAABB itemToAABB, IndexedKdtreeParams params);

	void buildNodeRecursive(
		std::size_t nodeIndex,
		const AABB3D& nodeAABB,
		const Index* nodeItemIndices,
		std::size_t numNodeItems,
		std::size_t currentNodeDepth,
		std::size_t currentBadRefines,
		const std::vector<AABB3D>& itemAABBs,
		std::size_t maxNodeDepth,
		IndexedKdtreeParams params,
		Index* negativeItemIndicesCache,
		Index* positiveItemIndicesCache,
		std::array<std::unique_ptr<IndexedItemEndpoint[]>, 3>& endpointsCache);

private:
	std::size_t        m_numItems;
	IndexToItem        m_indexToItem;
	AABB3D             m_rootAABB;
	std::vector<Node>  m_nodeBuffer;// TODO: compact this after build
	std::size_t        m_numNodes;// TODO: remove this
	std::vector<Index> m_itemIndices;
};

}// end namespace ph::math

#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.ipp"
