#include "Core/Intersectable/Kdtree/KdtreeNode.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Core/Intersectable/Intersectable.h"
#include "Math/TVector3.h"

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
	static const int32 INTERSECTABLE_MIN = 1;
	static const int32 INTERSECTABLE_MAX = 2;

public:
	real  m_testPoint;
	int32 m_pointType;

	TestPoint() : 
		m_testPoint(0.0_r), m_pointType(INTERSECTABLE_MIN)
	{

	}

	TestPoint(const real testPoint, const int32 pointType) :
		m_testPoint(testPoint), m_pointType(pointType)
	{

	}

	inline bool operator < (const TestPoint& other) const
	{
		return m_testPoint < other.m_testPoint;
	}
};

const int32 TestPoint::INTERSECTABLE_MIN;
const int32 TestPoint::INTERSECTABLE_MAX;

KdtreeNode::KdtreeNode(std::vector<const Intersectable*>* intersectableBuffer) :
	m_positiveChild(nullptr), m_negativeChild(nullptr), 
	m_splitAxis(KDTREE_UNKNOWN_AXIS), m_splitPos(0.0_r),
	m_intersectableBuffer(intersectableBuffer), 
	m_nodeBufferStartIndex(-1), m_nodeBufferEndIndex(-1)
{
	if(!intersectableBuffer)
	{
		std::cerr << "warning: at KdtreeNode::KdtreeNode(), " 
		          << "specified intersectable buffer is null" << std::endl;
	}
}

void KdtreeNode::buildTree(const std::vector<const Intersectable*>& intersectables)
{
	if(intersectables.empty())
	{
		m_aabb = KdtreeAABB(Vector3R(0, 0, 0), Vector3R(0, 0, 0));
		return;
	}

	AABB3D nodeAABB;
	intersectables[0]->calcAABB(&nodeAABB);
	AABB3D isableAABB;
	for(int i = 1; i < intersectables.size(); i++)
	{
		intersectables[i]->calcAABB(&isableAABB);
		nodeAABB.unionWith(isableAABB);
	}

	m_aabb = KdtreeAABB(nodeAABB);

	buildChildrenNodes(intersectables);
}

bool KdtreeNode::findClosestIntersection(const Ray& ray, Intersection* const out_intersection) const
{
	real rayNearHitDist;
	real rayFarHitDist;

	if(!m_aabb.isIntersectingVolume(ray, &rayNearHitDist, &rayFarHitDist))
	{
		// ray missed root node's aabb
		return false;
	}

	return traverseAndFindClosestIntersection(ray, out_intersection, rayNearHitDist, rayFarHitDist);
}

void KdtreeNode::analyzeSplitCostSAH(const std::vector<const Intersectable*>& intersectables,
                                     const int32 axis, 
                                     float64* const out_minCost, 
                                     real* const out_splitPoint) const
{
	std::vector<TestPoint> testAxisPoints(intersectables.size() * 2);

	AABB3D isableAABB;
	KdtreeAABB isableKdtreeAABB;
	for(std::size_t i = 0; i < intersectables.size(); i++)
	{
		intersectables[i]->calcAABB(&isableAABB);
		isableKdtreeAABB = KdtreeAABB(isableAABB);

		testAxisPoints[2 * i]     = TestPoint(isableKdtreeAABB.getMinVertex(axis), TestPoint::INTERSECTABLE_MIN);
		testAxisPoints[2 * i + 1] = TestPoint(isableKdtreeAABB.getMaxVertex(axis), TestPoint::INTERSECTABLE_MAX);
	}

	// the sorting process has to be stable (equal elements shouldn't be reordered)
	std::stable_sort(testAxisPoints.begin(), testAxisPoints.end());

	float64 noSplitSurfaceArea = m_aabb.getSurfaceArea();
	float64 noSplitCost = COST_INTERSECTION * static_cast<float64>(intersectables.size());

	std::size_t numNnodeIsables, numPnodeIsables;
	bool boundaryIsablePassed;

	// analyze on specified axis
	numNnodeIsables = 0;
	numPnodeIsables = intersectables.size();
	boundaryIsablePassed = false;

	real minAabbAxisPoint = m_aabb.getMinVertex(axis);
	real maxAabbAxisPoint = m_aabb.getMaxVertex(axis);

	*out_minCost = std::numeric_limits<float64>::max();
	*out_splitPoint = (minAabbAxisPoint + maxAabbAxisPoint) * 0.5_r;

	for(std::size_t i = 0; i < testAxisPoints.size(); i++)
	{
		if(testAxisPoints[i].m_pointType == TestPoint::INTERSECTABLE_MIN)
		{
			if(boundaryIsablePassed)
			{
				boundaryIsablePassed = false;
				numPnodeIsables--;
			}

			numNnodeIsables++;
		}

		if(testAxisPoints[i].m_pointType == TestPoint::INTERSECTABLE_MAX)
		{
			if(boundaryIsablePassed)
			{
				numPnodeIsables--;
			}

			boundaryIsablePassed = true;
		}

		KdtreeAABB nAABB, pAABB;
		if(!m_aabb.trySplitAt(axis, testAxisPoints[i].m_testPoint, &nAABB, &pAABB))
		{
			continue;
		}

		float64 pNodeFrac = pAABB.getSurfaceArea() / noSplitSurfaceArea;
		float64 nNodeFrac = nAABB.getSurfaceArea() / noSplitSurfaceArea;

		float64 splitCost = COST_TRAVERSAL + COST_INTERSECTION * (pNodeFrac*numPnodeIsables + nNodeFrac*numNnodeIsables);

		if(splitCost < *out_minCost)
		{
			*out_minCost = splitCost;
			*out_splitPoint = testAxisPoints[i].m_testPoint;
		}
	}
}

void KdtreeNode::buildChildrenNodes(const std::vector<const Intersectable*>& intersectables)
{
	// a SAH based spatial partitioning algorithm

	float64 xAxisMinSplitCost, yAxisMinSplitCost, zAxisMinSplitCost;
	real xAxisSplitPoint, yAxisSplitPoint, zAxisSplitPoint;
	analyzeSplitCostSAH(intersectables, KDTREE_X_AXIS, &xAxisMinSplitCost, &xAxisSplitPoint);
	analyzeSplitCostSAH(intersectables, KDTREE_Y_AXIS, &yAxisMinSplitCost, &yAxisSplitPoint);
	analyzeSplitCostSAH(intersectables, KDTREE_Z_AXIS, &zAxisMinSplitCost, &zAxisSplitPoint);

	float64 noSplitCost = COST_INTERSECTION * static_cast<float64>(intersectables.size());
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
			std::cerr << "warning: at KdtreeNode::buildChildrenNodes(), " 
			          << "invalid split detected" << std::endl;
		}

		m_positiveChild = buildChildNode(pChildAABB, intersectables);
		m_negativeChild = buildChildNode(nChildAABB, intersectables);
	}
	else
	{
		m_nodeBufferStartIndex = m_intersectableBuffer->size();
		m_nodeBufferEndIndex = m_nodeBufferStartIndex + intersectables.size();
		for(const auto& intersectable : intersectables)
		{
			m_intersectableBuffer->push_back(intersectable);
		}
	}
}

std::unique_ptr<KdtreeNode> KdtreeNode::buildChildNode(const KdtreeAABB& childAABB, 
                                                       const std::vector<const Intersectable*>& parentIntersectables)
{
	std::vector<const Intersectable*> intersectables;

	AABB3D childNodeAABB;
	childAABB.getAABB(&childNodeAABB);

	for(const Intersectable* intersectable : parentIntersectables)
	{
		if(intersectable->isIntersectingVolumeConservative(childNodeAABB))
		{
			intersectables.push_back(intersectable);
		}	
	}

	if(intersectables.empty())
	{
		return nullptr;
	}

	auto childNode = std::make_unique<KdtreeNode>(m_intersectableBuffer);
	childNode->m_aabb = childAABB;
	childNode->buildChildrenNodes(intersectables);

	return childNode;
}

bool KdtreeNode::traverseAndFindClosestIntersection(const Ray& ray, 
                                                    Intersection* const out_intersection,
                                                    const real rayDistMin, 
                                                    const real rayDistMax) const
{
	if(!isLeaf())
	{
		real splitAxisRayOrigin = 0.0_r;
		real splitAxisRayDir    = 0.0_r;

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
		real raySplitPlaneDist = (m_splitPos - splitAxisRayOrigin) / splitAxisRayDir;

		// Case I: Split plane is beyond ray's range or behind ray origin, only near node is hit.
		if(raySplitPlaneDist >= rayDistMax || raySplitPlaneDist < 0.0_r)
		{
			if(nearHitNode != nullptr)
			{
				return nearHitNode->traverseAndFindClosestIntersection(ray, out_intersection, rayDistMin, rayDistMax);
			}
		}
		// Case II: Split plane is between ray origin and near intersection point, only far node is hit.
		else if(raySplitPlaneDist <= rayDistMin && raySplitPlaneDist > 0.0_r)
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
		const Ray segmentRay(ray.getOrigin(), ray.getDirection(), rayDistMin, rayDistMax);
		Vector3R temp;

		// TODO: infinity may be unsafe on some machine
		real closestHitSquaredDist = std::numeric_limits<real>::infinity();

		if(closestIntersection.getHitPrimitive() != nullptr)
			closestHitSquaredDist = closestIntersection.getHitPosition().sub(ray.getOrigin()).lengthSquared();

		for(std::size_t isableIndex = m_nodeBufferStartIndex; isableIndex < m_nodeBufferEndIndex; isableIndex++)
		{
			if((*m_intersectableBuffer)[isableIndex]->isIntersecting(segmentRay, out_intersection))
			{
				out_intersection->getHitPosition().sub(ray.getOrigin(), &temp);
				const real squaredHitDist = temp.lengthSquared();

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