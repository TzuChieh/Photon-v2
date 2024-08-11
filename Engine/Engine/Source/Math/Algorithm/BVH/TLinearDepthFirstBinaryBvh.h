#pragma once

#include "Math/Algorithm/BVH/TBinaryBvhNode.h"
#include "Math/Geometry/TLineSegment.h"

#include <Common/primitive_type.h>
#include <Common/utility.h>

#include <cstddef>
#include <algorithm>
#include <memory>

namespace ph::math
{

template<typename Item>
class TBvhInfoNode;

template<typename Item, typename Index>
class TLinearDepthFirstBinaryBvh final
{
public:
	using NodeType = TBinaryBvhNode<Item, Index>;

	/*! Stack size for BVH traversal. The default should be enough for most cases, unless the tree
	is highly unbalanced (and we should avoid this). */
	inline constexpr static auto TRAVERSAL_STACK_SIZE = std::min<std::size_t>(
		sizeof_in_bits<Index>() * 3 / 2, 48);

	void build(
		const TBvhInfoNode<Item>* const rootNode,
		std::size_t totalInfoNodes,
		std::size_t totalItems);

	template<typename TesterFunc, bool IS_ROBUST = true>
	bool nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const;

	bool isEmpty() const;
	const NodeType& getRoot() const;
	std::size_t numNodes() const;
	std::size_t numItems() const;

private:
	void buildNodeRecursive(
		const TBvhInfoNode<Item>* infoNode);

	std::unique_ptr<NodeType[]> m_nodes;
	std::unique_ptr<Item[]> m_items;
	Index m_numNodes = 0;
	Index m_numItems = 0;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.ipp"
