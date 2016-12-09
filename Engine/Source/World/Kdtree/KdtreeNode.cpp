#include "World/Kdtree/KdtreeNode.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Geometry/Triangle.h"

#include <iostream>
#include <algorithm>
#include <limits>

namespace ph
{

constexpr float64 KdtreeNode::COST_TRAVERSAL;
constexpr float64 KdtreeNode::COST_INTERSECTION;

class TestPoint final
{
public:
	static const int32 PRIMITIVE_MIN = 1;
	static const int32 PRIMITIVE_MAX = 2;

public:
	float32 m_testPoint;
	int32   m_pointType;

	TestPoint() : 
		m_testPoint(0.0f), m_pointType(PRIMITIVE_MIN)
	{

	}

	TestPoint(const float32 testPoint, const int32 pointType) :
		m_testPoint(testPoint), m_pointType(pointType)
	{

	}

	inline bool operator < (const TestPoint& other) const
	{
		return m_testPoint < other.m_testPoint;
	}
};

const int32 TestPoint::PRIMITIVE_MIN;
const int32 TestPoint::PRIMITIVE_MAX;

KdtreeNode::KdtreeNode(std::vector<const Triangle*>* triangleBuffer) :
	m_positiveChild(nullptr), m_negativeChild(nullptr), 
	m_splitAxis(KDTREE_UNKNOWN_AXIS), m_splitPos(0.0f),
	m_triangleBuffer(triangleBuffer), m_nodeBufferStartIndex(-1), m_nodeBufferEndIndex(-1)
{
	if(!triangleBuffer)
	{
		std::cerr << "warning: at KdtreeNode::KdtreeNode(), specified buffer is null" << std::endl;
	}
}

void KdtreeNode::buildTree(const std::vector<const Triangle*>& triangles)
{
	if(triangles.empty())
	{
		m_aabb = KdtreeAABB(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
		return;
	}

	AABB nodeAABB;
	triangles[0]->calcAABB(&nodeAABB);
	AABB primAABB;
	for(int i = 1; i < triangles.size(); i++)
	{
		triangles[i]->calcAABB(&primAABB);
		nodeAABB.unionWith(primAABB);
	}

	m_aabb = KdtreeAABB(nodeAABB);

	buildChildrenNodes(triangles);
}

bool KdtreeNode::findClosestIntersection(const Ray& ray, Intersection* const out_intersection) const
{
	float32 rayNearHitDist;
	float32 rayFarHitDist;

	if(!m_aabb.isIntersecting(ray, &rayNearHitDist, &rayFarHitDist))
	{
		// ray missed root node's aabb
		return false;
	}

	float32 nearHitDist = rayNearHitDist < 0.0f ? 0.0f : rayNearHitDist;
	float32 farHitDist = rayFarHitDist;

	return traverseAndFindClosestIntersection(ray, out_intersection, nearHitDist, farHitDist);
}

void KdtreeNode::analyzeSplitCostSAH(const std::vector<const Triangle*>& triangles, const int32 axis, float64* const out_minCost, float32* const out_splitPoint) const
{
	std::vector<TestPoint> testAxisPoints(triangles.size() * 2);

	AABB primAABB;
	KdtreeAABB primKdtreeAABB;
	for(std::size_t i = 0; i < triangles.size(); i++)
	{
		triangles[i]->calcAABB(&primAABB);
		primKdtreeAABB = KdtreeAABB(primAABB);

		float32 minVertex[KDTREE_NUM_AXES];
		float32 maxVertex[KDTREE_NUM_AXES];
		primKdtreeAABB.getMinVertex(minVertex);
		primKdtreeAABB.getMaxVertex(maxVertex);

		testAxisPoints[2 * i]     = TestPoint(minVertex[axis], TestPoint::PRIMITIVE_MIN);
		testAxisPoints[2 * i + 1] = TestPoint(maxVertex[axis], TestPoint::PRIMITIVE_MAX);
	}

	// the sorting process has to be stable (equal elements shouldn't be reordered)
	std::stable_sort(testAxisPoints.begin(), testAxisPoints.end());

	float64 noSplitSurfaceArea = m_aabb.getSurfaceArea();
	float64 noSplitCost = COST_INTERSECTION * static_cast<float64>(triangles.size());
	

	std::size_t numNnodePrims, numPnodePrims;
	bool boundaryPrimPassed;

	// analyze on specified axis
	numNnodePrims = 0;
	numPnodePrims = triangles.size();
	boundaryPrimPassed = false;

	float32 minVertex[KDTREE_NUM_AXES];
	float32 maxVertex[KDTREE_NUM_AXES];
	m_aabb.getMinVertex(minVertex);
	m_aabb.getMaxVertex(maxVertex);

	float32 minAabbAxisPoint = minVertex[axis];
	float32 maxAabbAxisPoint = maxVertex[axis];

	*out_minCost = std::numeric_limits<float64>::max();
	*out_splitPoint = (minAabbAxisPoint + maxAabbAxisPoint) / 2.0f;

	for(std::size_t i = 0; i < testAxisPoints.size(); i++)
	{
		if(testAxisPoints[i].m_pointType == TestPoint::PRIMITIVE_MIN)
		{
			if(boundaryPrimPassed)
			{
				boundaryPrimPassed = false;
				numPnodePrims--;
			}

			numNnodePrims++;
		}

		if(testAxisPoints[i].m_pointType == TestPoint::PRIMITIVE_MAX)
		{
			if(boundaryPrimPassed)
			{
				numPnodePrims--;
			}

			boundaryPrimPassed = true;
		}

		KdtreeAABB nAABB, pAABB;
		if(!m_aabb.trySplitAt(axis, testAxisPoints[i].m_testPoint, &nAABB, &pAABB))
		{
			continue;
		}

		float64 pNodeFrac = pAABB.getSurfaceArea() / noSplitSurfaceArea;
		float64 nNodeFrac = nAABB.getSurfaceArea() / noSplitSurfaceArea;

		float64 splitCost = COST_TRAVERSAL + COST_INTERSECTION * (pNodeFrac*numPnodePrims + nNodeFrac*numNnodePrims);

		if(splitCost < *out_minCost)
		{
			*out_minCost = splitCost;
			*out_splitPoint = testAxisPoints[i].m_testPoint;
		}
	}
}

void KdtreeNode::buildChildrenNodes(const std::vector<const Triangle*>& triangles)
{
	// a SAH based spatial partitioning algorithm

	float64 xAxisMinSplitCost, yAxisMinSplitCost, zAxisMinSplitCost;
	float32 xAxisSplitPoint, yAxisSplitPoint, zAxisSplitPoint;
	analyzeSplitCostSAH(triangles, KDTREE_X_AXIS, &xAxisMinSplitCost, &xAxisSplitPoint);
	analyzeSplitCostSAH(triangles, KDTREE_Y_AXIS, &yAxisMinSplitCost, &yAxisSplitPoint);
	analyzeSplitCostSAH(triangles, KDTREE_Z_AXIS, &zAxisMinSplitCost, &zAxisSplitPoint);

	float64 noSplitCost = COST_INTERSECTION * static_cast<float64>(triangles.size());
	float64 minAxisSplitCost = fmin(fmin(xAxisMinSplitCost, yAxisMinSplitCost), zAxisMinSplitCost);

	if(minAxisSplitCost < noSplitCost)
	{
		if(xAxisMinSplitCost < yAxisMinSplitCost)
		{
			if(xAxisMinSplitCost < zAxisMinSplitCost)
			{
				m_splitAxis = KDTREE_X_AXIS;
				m_splitPos = xAxisSplitPoint;
			}
			else
			{
				m_splitAxis = KDTREE_Z_AXIS;
				m_splitPos = zAxisSplitPoint;
			}
		}
		else
		{
			if(yAxisMinSplitCost < zAxisMinSplitCost)
			{
				m_splitAxis = KDTREE_Y_AXIS;
				m_splitPos = yAxisSplitPoint;
			}
			else
			{
				m_splitAxis = KDTREE_Z_AXIS;
				m_splitPos = zAxisSplitPoint;
			}
		}

		KdtreeAABB pChildAABB, nChildAABB;
		if(!m_aabb.trySplitAt(m_splitAxis, m_splitPos, &nChildAABB, &pChildAABB))
		{
			std::cerr << "warning: at KdtreeNode::buildChildrenNodes(), invalid split detected" << std::endl;
		}

		m_positiveChild = buildChildNode(pChildAABB, triangles);
		m_negativeChild = buildChildNode(nChildAABB, triangles);
	}
	else
	{
		m_nodeBufferStartIndex = m_triangleBuffer->size();
		m_nodeBufferEndIndex = m_nodeBufferStartIndex + triangles.size();
		for(const auto& triangle : triangles)
		{
			m_triangleBuffer->push_back(triangle);
		}
	}
}

std::unique_ptr<KdtreeNode> KdtreeNode::buildChildNode(const KdtreeAABB& childAABB, const std::vector<const Triangle*>& parentTriangles)
{
	std::vector<const Triangle*> triangles;

	AABB primAABB;
	for(std::size_t i = 0; i < parentTriangles.size(); i++)
	{
		parentTriangles[i]->calcAABB(&primAABB);
		if(childAABB.isIntersecting(primAABB))
		{
			triangles.push_back(parentTriangles[i]);
		}	
	}

	if(triangles.empty())
	{
		return nullptr;
	}

	auto childNode = std::make_unique<KdtreeNode>(m_triangleBuffer);
	childNode->m_aabb = childAABB;
	childNode->buildChildrenNodes(triangles);

	return childNode;
}

bool KdtreeNode::traverseAndFindClosestIntersection(const Ray& ray, Intersection* const out_intersection,
                                                    const float32 rayDistMin, const float32 rayDistMax) const
{
	if(!isLeaf())
	{
		float32 splitAxisRayOrigin = 0.0f;
		float32 splitAxisRayDir    = 0.0f;

		switch(m_splitAxis)
		{
		case KDTREE_X_AXIS:
			splitAxisRayOrigin = ray.getOrigin().x;
			splitAxisRayDir = ray.getDirection().x;
			break;

		case KDTREE_Y_AXIS:
			splitAxisRayOrigin = ray.getOrigin().y;
			splitAxisRayDir = ray.getDirection().y;
			break;

		case KDTREE_Z_AXIS:
			splitAxisRayOrigin = ray.getOrigin().z;
			splitAxisRayDir = ray.getDirection().z;
			break;

		default:
			std::cerr << "KdtreeNode: unidentified split axis detected" << std::endl;
			break;
		}

		KdtreeNode* nearHitNode;
		KdtreeNode* farHitNode;

		if(m_splitPos > splitAxisRayOrigin)
		{
			nearHitNode = m_negativeChild.get();
			farHitNode = m_positiveChild.get();
		}
		else
		{
			nearHitNode = m_positiveChild.get();
			farHitNode = m_negativeChild.get();
		}

		// The result can be NaN (the ray is lying on the splitting plane). In such case, traverse both
		// positive and negative node (handled in Case III).
		float32 raySplitPlaneDist = (m_splitPos - splitAxisRayOrigin) / splitAxisRayDir;

		// Case I: Split plane is beyond ray's range or behind ray origin, only near node is hit.
		if(raySplitPlaneDist >= rayDistMax || raySplitPlaneDist < 0.0f)
		{
			if(nearHitNode != nullptr)
			{
				return nearHitNode->traverseAndFindClosestIntersection(ray, out_intersection, rayDistMin, rayDistMax);
			}
		}
		// Case II: Split plane is between ray origin and near intersection point, only far node is hit.
		else if(raySplitPlaneDist <= rayDistMin && raySplitPlaneDist > 0.0f)
		{
			if(farHitNode != nullptr)
			{
				return farHitNode->traverseAndFindClosestIntersection(ray, out_intersection, rayDistMin, rayDistMax);
			}
		}
		// Case III: Split plane is within ray's range, and both near and far node are hit.
		else
		{
			if(nearHitNode != nullptr)
			{
				if(nearHitNode->traverseAndFindClosestIntersection(ray, out_intersection, rayDistMin, raySplitPlaneDist))
				{
					return true;
				}
			}

			if(farHitNode != nullptr)
			{
				if(farHitNode->traverseAndFindClosestIntersection(ray, out_intersection, raySplitPlaneDist, rayDistMax))
				{
					return true;
				}
			}
		}

		return false;
	}
	else
	{
		Intersection closestIntersection(*out_intersection);

		Vector3f temp;

		// TODO: infinity may be unsafe on some machine
		float32 closestHitSquaredDist = std::numeric_limits<float32>::infinity();

		if(closestIntersection.getHitTriangle() != nullptr)
			closestHitSquaredDist = closestIntersection.getHitPosition().sub(ray.getOrigin()).squaredLength();

		for(std::size_t triIndex = m_nodeBufferStartIndex; triIndex < m_nodeBufferEndIndex; triIndex++)
		{
			out_intersection->clear();

			if((*m_triangleBuffer)[triIndex]->isIntersecting(ray, out_intersection))
			{
				out_intersection->getHitPosition().sub(ray.getOrigin(), &temp);
				float32 squaredHitDist = temp.squaredLength();

				if(squaredHitDist < closestHitSquaredDist)
				{
					closestHitSquaredDist = squaredHitDist;
					closestIntersection = *out_intersection;
				}
			}
		}

		*out_intersection = closestIntersection;

		// Notice that rayDistMax can be NaN or +infinity, in such cases the return value (does the 
		// closest intersection in the entire tree found) can be false even if we've actually found 
		// one and stored it in the intersection. Since these are rare situations, and to properly 
		// handle them may slow down the algorithm quite a bit, so I assumed that simply ignoring 
		// these cases won't generate any noticeable visual artifacts.
		return (closestHitSquaredDist < rayDistMax * rayDistMax);
	}
}

bool KdtreeNode::isLeaf() const
{
	return m_nodeBufferStartIndex != m_nodeBufferEndIndex;
}

}// end namespace ph