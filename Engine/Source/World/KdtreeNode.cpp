#include "World/KdtreeNode.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Primitive/Primitive.h"

#include <iostream>
#include <algorithm>
#include <limits>

namespace ph
{

const int32 KdtreeNode::UNKNOWN_AXIS;
const int32 KdtreeNode::X_AXIS;
const int32 KdtreeNode::Y_AXIS;
const int32 KdtreeNode::Z_AXIS;

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

KdtreeNode::KdtreeNode() : 
	m_positiveChild(nullptr), m_negativeChild(nullptr), 
	m_splitAxis(UNKNOWN_AXIS), m_splitPos(0.0f)
{

}

void KdtreeNode::buildTree(const std::vector<const Primitive*>& primitives)
{
	if(primitives.empty())
	{
		m_aabb.setMinVertex(Vector3f(0, 0, 0));
		m_aabb.setMaxVertex(Vector3f(0, 0, 0));
		return;
	}

	primitives[0]->calcAABB(&m_aabb);
	AABB primAABB;
	for(int i = 1; i < primitives.size(); i++)
	{
		primitives[i]->calcAABB(&primAABB);
		m_aabb.unionWith(primAABB);
	}

	buildChildrenNodes(primitives);
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

void KdtreeNode::buildChildrenNodes(const std::vector<const Primitive*>& primitives)
{
	// a SAH based spatial partitioning algorithm

	std::vector<TestPoint> xPoints(primitives.size() * 2);
	std::vector<TestPoint> yPoints(primitives.size() * 2);
	std::vector<TestPoint> zPoints(primitives.size() * 2);

	AABB primAABB;
	for(std::size_t i = 0; i < primitives.size(); i++)
	{
		primitives[i]->calcAABB(&primAABB);

		xPoints[2 * i]     = TestPoint(primAABB.getMinVertex().x, TestPoint::PRIMITIVE_MIN);
		xPoints[2 * i + 1] = TestPoint(primAABB.getMaxVertex().x, TestPoint::PRIMITIVE_MAX);
		yPoints[2 * i]     = TestPoint(primAABB.getMinVertex().y, TestPoint::PRIMITIVE_MIN);
		yPoints[2 * i + 1] = TestPoint(primAABB.getMaxVertex().y, TestPoint::PRIMITIVE_MAX);
		zPoints[2 * i]     = TestPoint(primAABB.getMinVertex().z, TestPoint::PRIMITIVE_MIN);
		zPoints[2 * i + 1] = TestPoint(primAABB.getMaxVertex().z, TestPoint::PRIMITIVE_MAX);
	}

	// the sorting process has to be stable (equal elements shouldn't be reordered)
	std::stable_sort(xPoints.begin(), xPoints.end());
	std::stable_sort(yPoints.begin(), yPoints.end());
	std::stable_sort(zPoints.begin(), zPoints.end());

	float64 xExtent = m_aabb.getMaxVertex().x - m_aabb.getMinVertex().x;
	float64 yExtent = m_aabb.getMaxVertex().y - m_aabb.getMinVertex().y;
	float64 zExtent = m_aabb.getMaxVertex().z - m_aabb.getMinVertex().z;
	float64 noSplitSurfaceArea = 2.0 * (xExtent * yExtent + yExtent * zExtent + zExtent * xExtent);
	float64 noSplitCost = COST_INTERSECTION * static_cast<float64>(primitives.size());

	//		noSplitCost = 0.0;

	float64 minSplitCost = std::numeric_limits<float64>::max();

	std::size_t numNnodePrims, numPnodePrims;
	bool boundaryPrimPassed;

	// analyze x-axis
	numNnodePrims = 0;
	numPnodePrims = primitives.size();
	boundaryPrimPassed = false;

	for(std::size_t i = 0; i < xPoints.size(); i++)
	{
		if(xPoints[i].m_pointType == TestPoint::PRIMITIVE_MIN)
		{
			if(boundaryPrimPassed)
			{
				boundaryPrimPassed = false;
				numPnodePrims--;
			}

			numNnodePrims++;
		}

		if(xPoints[i].m_pointType == TestPoint::PRIMITIVE_MAX)
		{
			if(boundaryPrimPassed)
			{
				numPnodePrims--;
			}

			boundaryPrimPassed = true;
		}

		// check if the test point is a reasonable one
		if(xPoints[i].m_testPoint <= m_aabb.getMinVertex().x || xPoints[i].m_testPoint >= m_aabb.getMaxVertex().x)
			continue;

		float64 pNodeXextent = m_aabb.getMaxVertex().x - xPoints[i].m_testPoint;
		float64 nNodeXextent = xPoints[i].m_testPoint - m_aabb.getMinVertex().x;
		float64 pNodeSurfaceArea = 2.0 * (pNodeXextent * yExtent + yExtent * zExtent + zExtent * pNodeXextent);
		float64 nNodeSurfaceArea = 2.0 * (nNodeXextent * yExtent + yExtent * zExtent + zExtent * nNodeXextent);
		float64 pNodeFrac = pNodeSurfaceArea / noSplitSurfaceArea;
		float64 nNodeFrac = nNodeSurfaceArea / noSplitSurfaceArea;

		float64 splitCost = COST_TRAVERSAL + COST_INTERSECTION * (pNodeFrac*numPnodePrims + nNodeFrac*numNnodePrims);

		if(splitCost < minSplitCost)
		{
			minSplitCost = splitCost;
			m_splitPos = xPoints[i].m_testPoint;
			m_splitAxis = X_AXIS;
		}
	}

	// analyze y-axis
	numNnodePrims = 0;
	numPnodePrims = primitives.size();
	boundaryPrimPassed = false;

	for(int i = 0; i < yPoints.size(); i++)
	{
		if(yPoints[i].m_pointType == TestPoint::PRIMITIVE_MIN)
		{
			if(boundaryPrimPassed)
			{
				boundaryPrimPassed = false;
				numPnodePrims--;
			}

			numNnodePrims++;
		}

		if(yPoints[i].m_pointType == TestPoint::PRIMITIVE_MAX)
		{
			if(boundaryPrimPassed)
			{
				numPnodePrims--;
			}

			boundaryPrimPassed = true;
		}

		// check if the test point is a reasonable one
		if(yPoints[i].m_testPoint <= m_aabb.getMinVertex().y || yPoints[i].m_testPoint >= m_aabb.getMaxVertex().y)
			continue;

		float64 pNodeYextent = m_aabb.getMaxVertex().y - yPoints[i].m_testPoint;
		float64 nNodeYextent = yPoints[i].m_testPoint - m_aabb.getMinVertex().y;
		float64 pNodeSurfaceArea = 2.0 * (xExtent * pNodeYextent + pNodeYextent * zExtent + zExtent * xExtent);
		float64 nNodeSurfaceArea = 2.0 * (xExtent * nNodeYextent + nNodeYextent * zExtent + zExtent * xExtent);
		float64 pNodeFrac = pNodeSurfaceArea / noSplitSurfaceArea;
		float64 nNodeFrac = nNodeSurfaceArea / noSplitSurfaceArea;

		float64 splitCost = COST_TRAVERSAL + COST_INTERSECTION * (pNodeFrac*numPnodePrims + nNodeFrac*numNnodePrims);

		if(splitCost < minSplitCost)
		{
			minSplitCost = splitCost;
			m_splitPos = yPoints[i].m_testPoint;
			m_splitAxis = Y_AXIS;
		}
	}

	// analyze z-axis
	numNnodePrims = 0;
	numPnodePrims = primitives.size();
	boundaryPrimPassed = false;

	for(int i = 0; i < zPoints.size(); i++)
	{
		if(zPoints[i].m_pointType == TestPoint::PRIMITIVE_MIN)
		{
			if(boundaryPrimPassed)
			{
				boundaryPrimPassed = false;
				numPnodePrims--;
			}

			numNnodePrims++;
		}

		if(zPoints[i].m_pointType == TestPoint::PRIMITIVE_MAX)
		{
			if(boundaryPrimPassed)
			{
				numPnodePrims--;
			}

			boundaryPrimPassed = true;
		}

		// check if the test point is a reasonable one
		if(zPoints[i].m_testPoint <= m_aabb.getMinVertex().z || zPoints[i].m_testPoint >= m_aabb.getMaxVertex().z)
			continue;

		float64 pNodeZextent = m_aabb.getMaxVertex().z - zPoints[i].m_testPoint;
		float64 nNodeZextent = zPoints[i].m_testPoint - m_aabb.getMinVertex().z;
		float64 pNodeSurfaceArea = 2.0 * (xExtent * yExtent + yExtent * pNodeZextent + pNodeZextent * xExtent);
		float64 nNodeSurfaceArea = 2.0 * (xExtent * yExtent + yExtent * nNodeZextent + nNodeZextent * xExtent);
		float64 pNodeFrac = pNodeSurfaceArea / noSplitSurfaceArea;
		float64 nNodeFrac = nNodeSurfaceArea / noSplitSurfaceArea;

		float64 splitCost = COST_TRAVERSAL + COST_INTERSECTION * (pNodeFrac*numPnodePrims + nNodeFrac*numNnodePrims);

		if(splitCost < minSplitCost)
		{
			minSplitCost = splitCost;
			m_splitPos = zPoints[i].m_testPoint;
			m_splitAxis = Z_AXIS;
		}
	}

	if(minSplitCost < noSplitCost)
	{
		AABB pChildAABB(m_aabb);
		AABB nChildAABB(m_aabb);
		Vector3f vertex;

		switch(m_splitAxis)
		{
		case X_AXIS:
			vertex = pChildAABB.getMinVertex();
			vertex.x = m_splitPos;
			pChildAABB.setMinVertex(vertex);
			vertex = nChildAABB.getMaxVertex();
			vertex.x = m_splitPos;
			nChildAABB.setMaxVertex(vertex);
			break;

		case Y_AXIS:
			vertex = pChildAABB.getMinVertex();
			vertex.y = m_splitPos;
			pChildAABB.setMinVertex(vertex);
			vertex = nChildAABB.getMaxVertex();
			vertex.y = m_splitPos;
			nChildAABB.setMaxVertex(vertex);
			break;

		case Z_AXIS:
			vertex = pChildAABB.getMinVertex();
			vertex.z = m_splitPos;
			pChildAABB.setMinVertex(vertex);
			vertex = nChildAABB.getMaxVertex();
			vertex.z = m_splitPos;
			nChildAABB.setMaxVertex(vertex);
			break;

		default:
			std::cerr << "KdtreeNode: unidentified split axis detected" << std::endl;
			break;
		}

		std::cout << "split" << std::endl;

		m_positiveChild = buildChildNode(pChildAABB, primitives);
		m_negativeChild = buildChildNode(nChildAABB, primitives);
	}
	else
	{
		m_primitives = primitives;
		std::cout << "size: " << primitives.size() << ", (" << minSplitCost << ", " << noSplitCost << ")" << std::endl;
		std::cout << "--------------------" << std::endl;
	}
}

std::unique_ptr<KdtreeNode> KdtreeNode::buildChildNode(const AABB& childAABB, const std::vector<const Primitive*>& parentPrimitives)
{
	std::vector<const Primitive*> primitives;

	AABB primAABB;
	for(std::size_t i = 0; i < parentPrimitives.size(); i++)
	{
		parentPrimitives[i]->calcAABB(&primAABB);
		if(primAABB.isIntersecting(childAABB))
		{
			primitives.push_back(parentPrimitives[i]);
		}	
	}

	if(primitives.empty())
	{
		return nullptr;
	}

	auto childNode = std::make_unique<KdtreeNode>();
	childNode->m_aabb = childAABB;
	childNode->buildChildrenNodes(primitives);

	return childNode;
}

bool KdtreeNode::traverseAndFindClosestIntersection(const Ray& ray, Intersection* const out_intersection,
                                                    const float32 rayDistMin, const float32 rayDistMax) const
{
	if(!isLeaf())
	{
		std::cout << "not isLeaf()" << std::endl;

		float32 splitAxisRayOrigin = 0.0f;
		float32 splitAxisRayDir    = 0.0f;

		switch(m_splitAxis)
		{
		case X_AXIS:
			splitAxisRayOrigin = ray.getOrigin().x;
			splitAxisRayDir = ray.getDirection().x;
			break;

		case Y_AXIS:
			splitAxisRayOrigin = ray.getOrigin().y;
			splitAxisRayDir = ray.getDirection().y;
			break;

		case Z_AXIS:
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

		if(closestIntersection.getHitPrimitive() != nullptr)
			closestHitSquaredDist = closestIntersection.getHitPosition().sub(ray.getOrigin()).squaredLength();

		for(const auto& primitive : m_primitives)
		{
			out_intersection->clear();

			if(primitive->isIntersecting(ray, out_intersection))
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
	return !m_primitives.empty();
}

}// end namespace ph