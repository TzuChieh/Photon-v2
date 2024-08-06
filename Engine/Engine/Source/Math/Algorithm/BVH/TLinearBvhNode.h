#pragma once

#include "Math/Geometry/TAABB3D.h"

#include <Common/primitive_type.h>

namespace ph::math
{

template<typename Item>
class TLinearBvhNode final
{
public:
	static auto makeInternal(
		const AABB3D& nodeAABB,
		std::size_t secondChildOffset,
		std::size_t splitAxis)
	-> TLinearBvhNode;

	static auto makeLeaf(
		const AABB3D& nodeAABB,
		std::size_t itemOffset,
		std::size_t numItems)
	-> TLinearBvhNode;

public:
	AABB3D aabb;

	union
	{
		// TODO: templatize int type
		std::size_t secondChildOffset;// for internal
		std::size_t itemOffset;       // for leaf
	};

	// FIXME: use axis type
	union
	{
		uint32 splitAxis;// for internal
		uint32 numItems; // for leaf
	};

	TLinearBvhNode();

	bool isLeaf() const;
	bool isInternal() const;

private:
	// FIXME: try to make more compact like kdtree
	bool m_isLeaf;
};

}// end namespace ph::math

#include "Math/Algorithm/BVH/TLinearBvhNode.ipp"
