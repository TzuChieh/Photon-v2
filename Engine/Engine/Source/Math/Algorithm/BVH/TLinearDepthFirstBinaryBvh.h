#pragma once

#include "Math/Algorithm/BVH/TLinearBvhNode.h"
#include "Math/Geometry/TLineSegment.h"
#include "Utility/utility.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph::math
{

template<typename Item, typename IndexType>
class TLinearDepthFirstBinaryBvh final
{
public:
	using NodeType = TLinearBvhNode<Item>;

	inline constexpr static auto TRAVERSAL_STACK_SIZE = sizeof_in_bits<IndexType>();

	std::unique_ptr<NodeType[]> nodes;
	std::unique_ptr<Item[]> items;
	IndexType numNodes = 0;
	IndexType numItems = 0;

	template<typename TesterFunc>
	bool nearestTraversal(const TLineSegment<real>& segment, TesterFunc&& intersectionTester) const;

	bool isEmpty() const;
	const NodeType& getRoot() const;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearDepthFirstBinaryBvh.ipp"
