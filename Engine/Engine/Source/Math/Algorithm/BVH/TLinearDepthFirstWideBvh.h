#pragma once

#include "Math/Algorithm/BVH/TWideBvhNode.h"
#include "Math/Geometry/TLineSegment.h"
#include "Math/constant.h"
#include "Utility/utility.h"

#include <Common/primitive_type.h>
#include <Common/utility.h>

#include <cstddef>
#include <algorithm>
#include <memory>
#include <array>
#include <string>

namespace ph::math
{

template<std::size_t N, typename Item>
class TBvhInfoNode;

enum class EBvhSplitAxisOrder : uint8f
{
	/*! As the unbalanced split axes described in @cite Fuetterling:2015:Efficient. */
	Unbalanced,

	/*! As the balanced split axes described in @cite Fuetterling:2015:Efficient. */
	Balanced,

	/*! Power-of-2 special case of `Balanced`. */
	BalancedPow2,

	/*! Child nodes are split according to a single axis. This is basically a generalization of
	the binary case to N-wide case. */
	Single
};

/*!
Wide BVH build and traversal routines are inspired by @cite Dammertz:2008:Shallow and
@cite Fuetterling:2015:Efficient. Various changes are applied to adapt their algorithms to our needs.
*/
template<std::size_t N, typename Item, typename Index>
class TLinearDepthFirstWideBvh final
{
public:
	using NodeType = TWideBvhNode<N, Index>;

	/*! Stack size for BVH traversal. The default should be enough for most cases, unless the tree
	is highly unbalanced (and we should avoid this). */
	inline constexpr static auto TRAVERSAL_STACK_SIZE = std::min<std::size_t>(
		sizeof_in_bits<Index>() * 3 / 2, 48);

	template<std::size_t SrcN>
	void build(
		const TBvhInfoNode<SrcN, Item>* const rootNode,
		std::size_t totalInfoNodes,
		std::size_t totalItems);

	template<bool IS_ROBUST = true, typename TesterFunc>
	bool nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const;

	bool isEmpty() const;
	const NodeType& getRoot() const;
	std::size_t numNodes() const;
	std::size_t numItems() const;
	std::size_t memoryUsage() const;
	std::string balancedPow2OrderTableToString() const;

private:
	template<std::size_t SrcN>
	struct TCollapsedNodes
	{
		/*! The resulting nodes after collapsing a treelet. */
		std::array<const TBvhInfoNode<SrcN, Item>*, N> nodes = {};

		/*! Balanced split axes for the nodes. */
		std::array<std::size_t, N> splitAxes = make_array<std::size_t, N>(constant::X_AXIS);
	};

	template<bool IS_ROBUST, typename TesterFunc>
	bool nearestTraversalGeneral(
		const TLineSegment<real>& segment,
		TesterFunc&& intersectionTester) const;

	template<bool IS_ROBUST, EBvhSplitAxisOrder ORDER, typename TesterFunc>
	bool nearestTraversalOrdered(
		const TLineSegment<real>& segment,
		TesterFunc&& intersectionTester) const;

	/*! Directly map informative nodes to wide nodes if the branch factor is the same.
	*/
	void convertNodesRecursive(
		const TBvhInfoNode<N, Item>* infoNode,
		bool& isSingleSplitAxisNodes);

	/*! Increase branch factor from `SrcN` to `N` by flattening treelets.
	*/
	template<std::size_t SrcN>
	void collapseNodesRecursive(
		const TBvhInfoNode<SrcN, Item>* infoNode);

	void refitBuffer(std::size_t nodeBufferSize, std::size_t itemBufferSize);

	/*! Flatten @f$ log_{SrcN}^{N} @f$ levels in the treelet with `infoNode` as root. The split axes
	are in `EBvhSplitAxisOrder::Balanced` order.
	*/
	template<std::size_t SrcN>
	static auto collapseToSingleLevel(
		const TBvhInfoNode<SrcN, Item>* infoNode)
	-> TCollapsedNodes<SrcN>;

	template<std::size_t SrcN>
	static constexpr auto numTreeletLevels()
	-> std::size_t;

	static constexpr auto makeIdentityOrderTable()
	-> std::array<uint8, N>;

	static constexpr auto makeSingleOrderTable()
	-> std::array<std::array<uint8, N>, 2>;

	inline static constexpr auto BALANCED_POW2_ORDER_TABLE_SIZE = 1 << (N - 1);

	static constexpr auto makeBalancedPow2OrderTable()
	-> std::array<std::array<uint8, N>, BALANCED_POW2_ORDER_TABLE_SIZE>;

	std::unique_ptr<NodeType[]> m_nodes;
	std::unique_ptr<Item[]> m_items;
	Index m_numNodes = 0;
	Index m_numItems = 0;
	EBvhSplitAxisOrder m_order = EBvhSplitAxisOrder::Unbalanced;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearDepthFirstWideBvh.ipp"
