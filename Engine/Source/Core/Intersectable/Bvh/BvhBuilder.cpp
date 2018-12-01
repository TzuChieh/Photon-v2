#include "Core/Intersectable/Bvh/BvhBuilder.h"
#include "Core/Intersectable/Bvh/BvhInfoNode.h"
#include "Core/Intersectable/Intersectable.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <iostream>
#include <algorithm>

namespace ph
{

class BvhSahBucket final
{
public:
	AABB3D      aabb;
	std::size_t numIntersectables;

	BvhSahBucket() : aabb(), numIntersectables(0) {}
	bool isEmpty() { return numIntersectables == 0; }
};

BvhBuilder::BvhBuilder(const EBvhType type) : 
	m_type(type)
{}

const BvhInfoNode* BvhBuilder::buildInformativeBinaryBvh(const std::vector<const Intersectable*>& intersectables)
{
	m_infoNodes.clear();
	m_infoNodes.shrink_to_fit();

	std::vector<BvhIntersectableInfo> intersectableInfos(intersectables.size());
	for(std::size_t i = 0; i < intersectables.size(); i++)
	{
		intersectableInfos[i] = BvhIntersectableInfo(intersectables[i], i);
	}

	const BvhInfoNode* rootNode = nullptr;

	if(m_type == EBvhType::HALF)
	{
		rootNode = buildBinaryBvhInfoNodeRecursive(intersectableInfos, 
		                                           ENodeSplitMethod::EQUAL_INTERSECTABLES);
	}
	else if(m_type == EBvhType::SAH_BUCKET)
	{
		rootNode = buildBinaryBvhInfoNodeRecursive(intersectableInfos, 
		                                           ENodeSplitMethod::SAH_BUCKETS);
	}
	else
	{
		std::cerr << "warning: at BvhBuilder::buildInformativeBinaryBvh(), " 
		          << "unsupported BVH type specified" << std::endl;
	}

	return rootNode;
}

void BvhBuilder::buildLinearDepthFirstBinaryBvh(const BvhInfoNode* const rootNode,
                                                std::vector<BvhLinearNode>* const out_linearNodes,
                                                std::vector<const Intersectable*>* const out_intersectables)
{
	PH_ASSERT_MSG(rootNode != nullptr, "input node cannot be nullptr");

	m_linearNodes.clear();
	m_linearNodes.shrink_to_fit();
	m_linearNodes.reserve(calcTotalNodes(rootNode));
	m_intersectables.clear();
	m_intersectables.shrink_to_fit();
	m_intersectables.reserve(calcTotalIntersectables(rootNode));

	buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode, nullptr);

	out_linearNodes->clear();
	out_linearNodes->shrink_to_fit();
	out_intersectables->clear();
	out_intersectables->shrink_to_fit();

	m_linearNodes.swap(*out_linearNodes);
	m_intersectables.swap(*out_intersectables);
}

const BvhInfoNode* BvhBuilder::buildBinaryBvhInfoNodeRecursive(const std::vector<BvhIntersectableInfo>& intersectables, 
                                                               const ENodeSplitMethod splitMethod)
{
	m_infoNodes.push_back(std::make_unique<BvhInfoNode>());
	BvhInfoNode* node = m_infoNodes.back().get();

	AABB3D nodeAABB(intersectables.empty() ? AABB3D() : intersectables.front().aabb);
	for(const auto& intersectable : intersectables)
	{
		nodeAABB = AABB3D::makeUnioned(nodeAABB, intersectable.aabb);
	}

	if(intersectables.size() <= 1)
	{
		*node = BvhInfoNode::makeBinaryLeaf(intersectables, nodeAABB);

		if(intersectables.empty())
		{
			std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), " 
			          << "leaf node without primitive detected" << std::endl;
		}
	}
	else
	{
		AABB3D centroidsAABB(intersectables.front().aabbCentroid);
		for(const auto& intersectable : intersectables)
		{
			centroidsAABB = AABB3D::makeUnioned(centroidsAABB, intersectable.aabbCentroid);
		}

		Vector3R extents = centroidsAABB.getExtents();
		if(extents.hasNegativeComponent())
		{
			std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), " 
			          << "negative AABB extent detected" << std::endl;
			extents.absLocal();
		}

		const int32 maxDimension = extents.maxDimension();

		if(centroidsAABB.getMinVertex()[maxDimension] == centroidsAABB.getMaxVertex()[maxDimension])
		{
			*node = BvhInfoNode::makeBinaryLeaf(intersectables, nodeAABB);
		}
		else
		{
			bool isSplitSuccess = false;
			std::vector<BvhIntersectableInfo> intersectablesA;
			std::vector<BvhIntersectableInfo> intersectablesB;

			switch(splitMethod)
			{

			case ENodeSplitMethod::EQUAL_INTERSECTABLES:
				isSplitSuccess = splitWithEqualIntersectables(intersectables, maxDimension, 
				                                              &intersectablesA, &intersectablesB);
				break;

			case ENodeSplitMethod::SAH_BUCKETS:
				isSplitSuccess = splitWithSahBuckets(intersectables, maxDimension, 
				                                     nodeAABB, centroidsAABB, 
				                                     &intersectablesA, &intersectablesB);
				break;

			default:
				std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), " 
				          << "unknown split method detected" << std::endl;
				isSplitSuccess = false;
				break;

			}// end switch split method

			if(isSplitSuccess && (intersectablesA.empty() || intersectablesB.empty()))
			{
				std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), " 
				          << "bad split detected" << std::endl;
				isSplitSuccess = false;
			}

			if(isSplitSuccess)
			{
				//std::cout << "#A: " << primitivesA.size() << "| #B: " << primitivesB.size() << std::endl;

				*node = BvhInfoNode::makeBinaryInternal(buildBinaryBvhInfoNodeRecursive(intersectablesA, splitMethod),
				                                        buildBinaryBvhInfoNodeRecursive(intersectablesB, splitMethod),
				                                        maxDimension);
			}
			else
			{
				*node = BvhInfoNode::makeBinaryLeaf(intersectables, nodeAABB);
			}
		}
	}

	return node;
}


void BvhBuilder::buildBinaryBvhLinearDepthFirstNodeRecursive(
	const BvhInfoNode* const rootNode,
	std::size_t* const out_nodeIndex)
{
	std::size_t nodeIndex = m_linearNodes.size();

	if(rootNode->isBinaryLeaf())
	{
		m_linearNodes.push_back(BvhLinearNode::makeLeaf(rootNode->aabb, 
		                                                m_intersectables.size(), 
		                                                static_cast<int32>(rootNode->intersectables.size())));

		for(const auto& intersectableInfo : rootNode->intersectables)
		{
			m_intersectables.push_back(intersectableInfo.intersectable);
		}
	}
	else if(rootNode->isBinaryInternal())
	{
		m_linearNodes.push_back(BvhLinearNode());

		std::size_t secondChildOffset = static_cast<std::size_t>(-1);
		buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode->children[0], nullptr);
		buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode->children[1], &secondChildOffset);
		m_linearNodes[nodeIndex] = BvhLinearNode::makeInternal(rootNode->aabb, 
		                                                       secondChildOffset, 
		                                                       rootNode->splitAxis);
	}
	else
	{
		std::cerr << "warning: at BvhBuilder::buildBinaryBvhLinearDepthFirstNodeRecursive(), " 
		          << "only binary node is acceptable" << std::endl;
		nodeIndex = static_cast<std::size_t>(-1);
	}

	if(out_nodeIndex)
	{
		*out_nodeIndex = nodeIndex;
	}
}

std::size_t BvhBuilder::calcTotalNodes(const BvhInfoNode* const rootNode)
{
	std::size_t result = 1;
	result += rootNode->children[0] ? calcTotalNodes(rootNode->children[0]) : 0;
	result += rootNode->children[1] ? calcTotalNodes(rootNode->children[1]) : 0;
	return result;
}

std::size_t BvhBuilder::calcTotalIntersectables(const BvhInfoNode* const rootNode)
{
	std::size_t result = rootNode->intersectables.size();
	result += rootNode->children[0] ? calcTotalIntersectables(rootNode->children[0]) : 0;
	result += rootNode->children[1] ? calcTotalIntersectables(rootNode->children[1]) : 0;
	return result;
}

std::size_t BvhBuilder::calcMaxDepth(const BvhInfoNode* const rootNode)
{
	std::size_t depth = rootNode->children[0] || rootNode->children[1] ? 1 : 0;
	std::size_t depthA = rootNode->children[0] ? calcMaxDepth(rootNode->children[0]) : 0;
	std::size_t depthB = rootNode->children[1] ? calcMaxDepth(rootNode->children[1]) : 0;
	depth += std::max(depthA, depthB);
	return depth;
}

bool BvhBuilder::splitWithEqualIntersectables(const std::vector<BvhIntersectableInfo>& intersectables, 
                                              const int32 splitDimension, 
                                              std::vector<BvhIntersectableInfo>* const out_partA,
                                              std::vector<BvhIntersectableInfo>* const out_partB)
{
	if(intersectables.size() < 2)
	{
		std::cerr << "warning: at BvhBuilder::splitWithEqualPrimitives(), " 
		          << "number of primitives < 2, cannot split" << std::endl;
		return false;
	}

	const std::size_t midIndex = intersectables.size() / 2 - 1;
	std::vector<BvhIntersectableInfo> sortedIntersectables(intersectables);

	std::nth_element(sortedIntersectables.begin(), 
	                 sortedIntersectables.begin() + midIndex, 
	                 sortedIntersectables.end(),
	[splitDimension](const BvhIntersectableInfo& a, const BvhIntersectableInfo& b)
	{
		return a.aabbCentroid[splitDimension] < b.aabbCentroid[splitDimension];
	});

	out_partA->clear();
	out_partB->clear();
	out_partA->shrink_to_fit();
	out_partB->shrink_to_fit();

	out_partA->insert(out_partA->end(), 
	                  sortedIntersectables.begin(), sortedIntersectables.begin() + midIndex + 1);
	out_partB->insert(out_partB->end(), 
	                  sortedIntersectables.begin() + midIndex + 1, sortedIntersectables.end());

	return true;
}

bool BvhBuilder::splitWithSahBuckets(const std::vector<BvhIntersectableInfo>& intersectables, 
                                     const int32 splitDimension,
                                     const AABB3D& primitivesAABB, const AABB3D& centroidsAABB,
                                     std::vector<BvhIntersectableInfo>* const out_partA,
                                     std::vector<BvhIntersectableInfo>* const out_partB)
{
	if(intersectables.size() < 2)
	{
		std::cerr << "warning: at BvhBuilder::splitWithSahBuckets(), " 
		          << "number of intersectables < 2, cannot split" << std::endl;
		return false;
	}

	const int32 numBuckets = 64;

	const int32    dim         = splitDimension;
	const Vector3R extents     = centroidsAABB.getExtents();
	const real     splitExtent = extents[dim];

	BvhSahBucket buckets[numBuckets];
	for(const auto& intersectable : intersectables)
	{
		if(splitExtent < 0.0_r)
		{
			std::cerr << "warning: at BvhBuilder::splitWithSahBuckets(), " 
			          << "intersectable AABB split extent < 0, cannot split" << std::endl;
			return false;
		}

		const real factor = (intersectable.aabbCentroid[dim] - centroidsAABB.getMinVertex()[dim]) / splitExtent;
		int32 bucketIndex = static_cast<int32>(factor * numBuckets);
		bucketIndex = (bucketIndex == numBuckets) ? bucketIndex - 1 : bucketIndex;

		buckets[bucketIndex].aabb = (buckets[bucketIndex].isEmpty()) ?
		                            intersectable.aabb : buckets[bucketIndex].aabb.unionWith(intersectable.aabb);
		buckets[bucketIndex].numIntersectables++;
	}

	const real traversalCost = 1.0_r / 8.0_r;
	const real intersectCost = 1.0_r;

	real costs[numBuckets - 1] = {0.0_r};
	for(int32 i = 0; i < numBuckets - 1; i++)
	{
		std::size_t numIntersectablesA = 0;
		AABB3D aabbA;
		for(int32 j = 0; j <= i; j++)
		{
			if(!buckets[j].isEmpty())
			{
				aabbA = aabbA.isPoint() ? buckets[j].aabb : AABB3D::makeUnioned(aabbA, buckets[j].aabb);
			}
			
			numIntersectablesA += buckets[j].numIntersectables;
		}

		std::size_t numIntersectablesB = 0;
		AABB3D aabbB;
		for(int32 j = i + 1; j < numBuckets; j++)
		{
			if(!buckets[j].isEmpty())
			{
				aabbB = aabbB.isPoint() ? buckets[j].aabb : AABB3D::makeUnioned(aabbB, buckets[j].aabb);
			}

			numIntersectablesB += buckets[j].numIntersectables;
		}

		const real probTestingA = aabbA.getSurfaceArea() / primitivesAABB.getSurfaceArea();
		const real probTestingB = aabbB.getSurfaceArea() / primitivesAABB.getSurfaceArea();

		costs[i] = traversalCost + 
		           static_cast<real>(numIntersectablesA) * intersectCost * probTestingA +
		           static_cast<real>(numIntersectablesB) * intersectCost * probTestingB;
	}

	int32 minCostIndex = 0;
	for(int32 i = 1; i < numBuckets - 1; i++)
	{
		if(costs[i] < costs[minCostIndex])
		{
			minCostIndex = i;
		}
	}

	const real minSplitCost = costs[minCostIndex];
	const real noSplitCost  = intersectCost * static_cast<real>(intersectables.size());

	const std::size_t maxIntersectables = 256;

	if(minSplitCost < noSplitCost || intersectables.size() > maxIntersectables)
	{
		out_partA->clear();
		out_partB->clear();
		out_partA->shrink_to_fit();
		out_partB->shrink_to_fit();

		for(const auto& intersectable : intersectables)
		{
			const real factor = (intersectable.aabbCentroid[dim] - centroidsAABB.getMinVertex()[dim]) / splitExtent;
			int32 bucketIndex = static_cast<int32>(factor * numBuckets);
			bucketIndex = (bucketIndex == numBuckets) ? bucketIndex - 1 : bucketIndex;

			if(bucketIndex <= minCostIndex)
				out_partA->push_back(intersectable);
			else
				out_partB->push_back(intersectable);
		}

		return true;
	}
	else
	{
		return false;
	}
}

}// end namespace ph