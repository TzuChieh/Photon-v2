#pragma once

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
	std::size_t maxNodeItems = 1;

	/*! Cost for traversing a node. */
	float traversalCost = 1.0f / 8.0f;

	/*! Cost for interacting with an item in a node. */
	float interactCost = 1.0f;

	/*! The algorithm for splitting a node. */
	EBvhNodeSplitMethod splitMethod = EBvhNodeSplitMethod::SAH_Buckets_OneAxis;
};

}// end namespace ph::math
