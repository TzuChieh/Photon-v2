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
	BvhParams();

	EBvhNodeSplitMethod getSplitMethod() const;
	std::size_t getMaxNodeItems() const;
	float getTraversalCost() const;
	float getInteractCost() const;

private:
	std::size_t         m_maxNodeItems;
	float               m_traversalCost;
	float               m_interactCost;
	EBvhNodeSplitMethod m_splitMethod;
};

// In-header Implementations:

inline BvhParams::BvhParams()
	: m_maxNodeItems (1)
	, m_traversalCost(1.0f / 8.0f)
	, m_interactCost (1.0f)
	, m_splitMethod  (EBvhNodeSplitMethod::SAH_Buckets_OneAxis)
{}

inline EBvhNodeSplitMethod BvhParams::getSplitMethod() const
{
	return m_splitMethod;
}

inline std::size_t BvhParams::getMaxNodeItems() const
{
	return m_maxNodeItems;
}

inline float BvhParams::getTraversalCost() const
{
	return m_traversalCost;
}

inline float BvhParams::getInteractCost() const
{
	return m_interactCost;
}

}// end namespace ph::math
