#pragma once

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::math
{

enum class EBvhNodeSplitMethod
{
	EqualItems,
	EqualIntervals,
	SAH_Buckets_OneAxis,
	SAH_Buckets_MultiAxis,
	SAH_EdgeSort_OneAxis
};

class BvhParams final
{
public:
	/*! Maximum number of items in a leaf node. */
	uint32 maxNodeItems = 1;

	/*! Number of buckets to use for bucket-based SAH splitting. */
	uint32 numSahBuckets = 64;

	/*! Cost for traversing a node. */
	float32 traversalCost = 1.0f / 8.0f;

	/*! Cost for interacting with an item in a node. */
	float32 interactCost = 1.0f;

	/*! The algorithm for splitting a node. */
	EBvhNodeSplitMethod splitMethod = EBvhNodeSplitMethod::SAH_Buckets_OneAxis;
};

}// end namespace ph::math
