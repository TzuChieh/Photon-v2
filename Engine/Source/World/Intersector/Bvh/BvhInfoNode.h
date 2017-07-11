#pragma once

#include "Common/primitive_type.h"
#include "Core/BoundingVolume/AABB.h"
#include "World/Intersector/Bvh/BvhIntersectableInfo.h"

#include <vector>
#include <cstddef>

namespace ph
{

class Intersectable;

class BvhInfoNode final
{
public:
	static BvhInfoNode makeBinaryInternal(const BvhInfoNode* child1, const BvhInfoNode* child2, const int32 splitAxis);
	static BvhInfoNode makeBinaryLeaf(const std::vector<BvhIntersectableInfo>& leafIntersectables, const AABB& leafAabb);

public:
	const BvhInfoNode*                children[2];
	AABB                              aabb;
	int32                             splitAxis;
	std::vector<BvhIntersectableInfo> intersectables;

	BvhInfoNode();

	bool isBinaryLeaf() const;
	bool isBinaryInternal() const;
};

}// end namespace ph