#pragma once

#include "Math/Algorithm/BVH/TLinearBvhNode.h"
#include "Math/Geometry/TLineSegment.h"
#include "Utility/utility.h"

#include <Common/primitive_type.h>

#include <algorithm>
#include <memory>

namespace ph::math
{

template<typename Item, typename Index>
class TLinearDepthFirstBinaryBvh final
{
public:
	using NodeType = TLinearBvhNode<Item, Index>;

	/*! Stack size for BVH traversal. The default should be enough for most cases, unless the tree
	is highly unbalanced (and we should avoid this). */
	inline constexpr static auto TRAVERSAL_STACK_SIZE = std::min<std::size_t>(
		sizeof_in_bits<Index>() * 3 / 2, 48);

	std::unique_ptr<NodeType[]> nodes;
	std::unique_ptr<Item[]> items;
	Index numNodes = 0;
	Index numItems = 0;

	template<typename TesterFunc, bool IS_ROBUST = true>
	bool nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const;

	bool isEmpty() const;
	const NodeType& getRoot() const;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.ipp"
