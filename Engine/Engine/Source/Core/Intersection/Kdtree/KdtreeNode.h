#pragma once

#include "Core/Intersection/Kdtree/KdtreeAABB.h"

#include <Common/primitive_type.h>

#include <memory>
#include <vector>

namespace ph
{

class Intersectable;
class HitProbe;

class KdtreeNode final
{
public:
	KdtreeNode(std::vector<const Intersectable*>* intersectableBuffer);

	void buildTree(const std::vector<const Intersectable*>& intersectables);
	bool findClosestIntersection(const Ray& ray, HitProbe& probe) const;
	KdtreeAABB getAABB() const;

private:
	std::unique_ptr<KdtreeNode> m_positiveChild;
	std::unique_ptr<KdtreeNode> m_negativeChild;

	std::vector<const Intersectable*>* m_intersectableBuffer;
	KdtreeAABB m_aabb;

	int32 m_splitAxis;
	real  m_splitPos;
	std::size_t m_nodeBufferStartIndex;
	std::size_t m_nodeBufferEndIndex;

	void buildChildrenNodes(const std::vector<const Intersectable*>& intersectables);
	std::unique_ptr<KdtreeNode> buildChildNode(const KdtreeAABB& childAABB, 
	                                           const std::vector<const Intersectable*>& parentIntersectables);
	bool traverseAndFindClosestIntersection(const Ray& ray, HitProbe& probe,
	                                        real rayDistMin, real rayDistMax) const;
	void analyzeSplitCostSAH(const std::vector<const Intersectable*>& intersectables, int32 axis,
	                         float64* out_minCost, real* out_splitPoint) const;
	bool isLeaf() const;

private:
	static constexpr float64 COST_TRAVERSAL    = 1.0;
	static constexpr float64 COST_INTERSECTION = 1.0;
};

// In-header Implementations:

inline KdtreeAABB KdtreeNode::getAABB() const
{
	return m_aabb;
}

}// end namespace ph
