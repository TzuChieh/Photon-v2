#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtreeNode.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedItemEndpoint.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Math/Geometry/TLineSegment.h"
#include "Math/Algorithm/IndexedKdtree/IndexedKdtreeParams.h"
#include "Utility/utility.h"

#include <Common/assertion.h>

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>
#include <type_traits>
#include <optional>

namespace ph::math
{

template<typename TesterFunc, typename Item>
concept CItemSegmentIntersectionTesterVanilla = requires (TesterFunc func, Item item)
{
	{ func(item, TLineSegment<real>{}) } -> std::same_as<std::optional<real>>;
};

template<typename TesterFunc, typename Item>
concept CItemSegmentIntersectionTesterWithIndex = requires (
	TesterFunc func, 
	Item item, 
	TLineSegment<real> segment,
	std::size_t itemIndex)
{
	{ func(item, segment, itemIndex) } -> std::same_as<std::optional<real>>;
};

template<typename TesterFunc, typename Item>
concept CItemSegmentIntersectionTester = 
	CItemSegmentIntersectionTesterVanilla<TesterFunc, Item> ||
	CItemSegmentIntersectionTesterWithIndex<TesterFunc, Item>;

template<
	typename IndexToItem,
	typename ItemToAABB,
	typename Index = std::size_t>
class TIndexedKdtree final
{
public:
	static_assert(std::is_invocable_v<IndexToItem, Index>);
	using Item = decltype(std::declval<IndexToItem>()(std::declval<Index>()));

private:
	using Node = TIndexedKdtreeNode<Index>;
	using ItemEndpoint = TIndexedItemEndpoint<Index>;

	static_assert(std::is_invocable_r_v<AABB3D, ItemToAABB, Item>);

public:
	TIndexedKdtree(
		std::size_t         numItems,
		IndexToItem         indexToItem,
		ItemToAABB          itemToAABB,
		IndexedKdtreeParams params = IndexedKdtreeParams());

	template<typename TesterFunc>
	bool nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const;

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
		std::array<std::unique_ptr<ItemEndpoint[]>, 3>& endpointsCache);

private:
	std::size_t        m_numItems;
	IndexToItem        m_indexToItem;
	AABB3D             m_rootAABB;
	std::vector<Node>  m_nodeBuffer;// TODO: compact this after build or use array
	std::size_t        m_numNodes;// TODO: remove this
	std::vector<Index> m_itemIndices;// TODO: compact this after build or use array
};

}// end namespace ph::math

#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.ipp"
