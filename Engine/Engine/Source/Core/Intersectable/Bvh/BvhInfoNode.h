#pragma once

#include "Common/primitive_type.h"
#include "Math/Geometry/TAABB3D.h"
#include "Core/Intersectable/Bvh/BvhIntersectableInfo.h"

#include <vector>
#include <cstddef>

namespace ph
{

class Intersectable;

class BvhInfoNode final
{
public:
	using AABB3D = math::AABB3D;

	static BvhInfoNode makeBinaryInternal(const BvhInfoNode* child1, const BvhInfoNode* child2, int32 splitAxis);
	static BvhInfoNode makeBinaryLeaf(const std::vector<BvhIntersectableInfo>& leafIntersectables, 
	                                  const AABB3D& leafAabb);

public:
	const BvhInfoNode*                children[2];
	AABB3D                            aabb;
	int32                             splitAxis;
	std::vector<BvhIntersectableInfo> intersectables;

	BvhInfoNode();

	bool isBinaryLeaf() const;
	bool isBinaryInternal() const;
};

}// end namespace ph
