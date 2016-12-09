#pragma once

#include "Common/primitive_type.h"
#include "World/Kdtree/KdtreeAABB.h"

#include <memory>
#include <vector>

namespace ph
{

class Triangle;
class Intersection;

class KdtreeNode final
{
public:
	KdtreeNode(std::vector<const Triangle*>* triangleBuffer);

	void buildTree(const std::vector<const Triangle*>& triangles);
	bool findClosestIntersection(const Ray& ray, Intersection* const out_intersection) const;

private:
	std::unique_ptr<KdtreeNode> m_positiveChild;
	std::unique_ptr<KdtreeNode> m_negativeChild;

	std::vector<const Triangle*>* m_triangleBuffer;
	KdtreeAABB m_aabb;

	int32   m_splitAxis;
	float32 m_splitPos;
	std::size_t m_nodeBufferStartIndex;
	std::size_t m_nodeBufferEndIndex;

	void buildChildrenNodes(const std::vector<const Triangle*>& triangles);
	std::unique_ptr<KdtreeNode> buildChildNode(const KdtreeAABB& childAABB, const std::vector<const Triangle*>& parentTriangles);
	bool traverseAndFindClosestIntersection(const Ray& ray, Intersection* const out_intersection, 
	                                        const float32 rayDistMin, const float32 rayDistMax) const;
	void analyzeSplitCostSAH(const std::vector<const Triangle*>& triangles, const int32 axis, float64* const out_minCost, float32* const out_splitPoint) const;
	bool isLeaf() const;

private:
	static constexpr float64 COST_TRAVERSAL    = 1.0;
	static constexpr float64 COST_INTERSECTION = 1.0;
};

}// end namespace ph