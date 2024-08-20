#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"
#include "Math/Geometry/TLineSegment.h"
#include "Utility/utility.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <algorithm>
#include <memory>

namespace ph::math
{

template<std::size_t N, typename Item>
class TBvhInfoNode;

template<std::size_t N, typename Item, typename Index>
class TLinearDepthFirstWideBvh final
{
public:
	using NodeType = TWideBvhNode<N, Item, Index>;

	/*! Stack size for BVH traversal. The default should be enough for most cases, unless the tree
	is highly unbalanced (and we should avoid this). */
	inline constexpr static auto TRAVERSAL_STACK_SIZE = std::min<std::size_t>(
		sizeof_in_bits<Index>() * 3 / 2, 48);

	template<std::size_t SrcN>
	void build(
		const TBvhInfoNode<SrcN, Item>* const rootNode,
		std::size_t totalInfoNodes,
		std::size_t totalItems);

	template<typename TesterFunc, bool IS_ROBUST = true>
	bool nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const;

	bool isEmpty() const;
	const NodeType& getRoot() const;
	std::size_t numNodes() const;
	std::size_t numItems() const;

private:
	/*! Directly map informative nodes to wide nodes if the branch factor is the same.
	*/
	void convertChildNodesRecursive(
		const TBvhInfoNode<N, Item>* infoNode);

	template<std::size_t SrcN>
	void collapseNodesRecursive(
		const TBvhInfoNode<SrcN, Item>* infoNode);

	void refitBuffer(std::size_t nodeBufferSize, std::size_t itemBufferSize);

	std::unique_ptr<NodeType[]> m_nodes;
	std::unique_ptr<Item[]> m_items;
	Index m_numNodes = 0;
	Index m_numItems = 0;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.ipp"
