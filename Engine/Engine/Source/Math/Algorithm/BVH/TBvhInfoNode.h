#pragma once

#include "Math/Geometry/TAABB3D.h"
#include "Math/constant.h"
#include "Utility/TSpan.h"

namespace ph::math
{

template<typename Item>
class TBvhItemInfo;

/*! @brief Binary BVH node packed with additional information.
This node type is typically used for building other types of BVH or for algorithmic analysis.
*/
template<typename Item>
class TBvhInfoNode final
{
public:
	using ItemInfo = TBvhItemInfo<Item>;

	static auto makeBinaryInternal(
		const TBvhInfoNode* child1,
		const TBvhInfoNode* child2,
		std::size_t splitAxis) -> TBvhInfoNode;
	
	static auto makeBinaryLeaf(
		TSpanView<ItemInfo> leafItems,
	    const AABB3D& leafAabb) -> TBvhInfoNode;

public:
	const TBvhInfoNode* children[2];
	TSpanView<ItemInfo> items;
	AABB3D              aabb;
	uint8               splitAxis;

	TBvhInfoNode();

	bool isBinaryLeaf() const;
	bool isBinaryInternal() const;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TBvhInfoNode.ipp"
