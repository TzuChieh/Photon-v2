#include "World/Intersector/Bvh/BvhBuilder.h"
#include "World/Intersector/Bvh/BvhInfoNode.h"
#include "Core/Primitive/Primitive.h"
#include "Math/TVector3.h"

#include <iostream>
#include <algorithm>

namespace ph
{

class BvhSahBucket final
{
public:
	AABB        aabb;
	std::size_t numPrimitives;

	BvhSahBucket() : aabb(), numPrimitives(0) {}
	bool isEmpty() { return numPrimitives == 0; }
};

BvhBuilder::BvhBuilder(const EBvhType type) : 
	m_type(type)
{

}

const BvhInfoNode* BvhBuilder::buildInformativeBinaryBvh(const std::vector<const Primitive*>& primitives)
{
	m_infoNodes.clear();
	m_infoNodes.shrink_to_fit();

	std::vector<BvhPrimitiveInfo> primitiveInfos(primitives.size());
	for(std::size_t i = 0; i < primitives.size(); i++)
	{
		primitiveInfos[i] = BvhPrimitiveInfo(primitives[i], i);
	}

	const BvhInfoNode* rootNode = nullptr;

	if(m_type == EBvhType::HALF)
	{
		rootNode = buildBinaryBvhInfoNodeRecursive(primitiveInfos, ENodeSplitMethod::EQUAL_PRIMITIVES);
	}
	else if(m_type == EBvhType::SAH_BUCKET)
	{
		rootNode = buildBinaryBvhInfoNodeRecursive(primitiveInfos, ENodeSplitMethod::SAH_BUCKETS);
	}
	else
	{
		std::cerr << "warning: at BvhBuilder::buildInformativeBinaryBvh(), unsupported BVH type specified" << std::endl;
	}

	return rootNode;
}

void BvhBuilder::buildLinearDepthFirstBinaryBvh(const BvhInfoNode* rootNode,
                                                std::vector<BvhLinearNode>* const out_linearNodes,
                                                std::vector<const Primitive*>* const out_primitives)
{
	if(rootNode == nullptr)
	{
		std::cerr << "warning: at BvhBuilder::buildLinearDepthFirstBinaryBvh(), input node is nullptr" << std::endl;
	}

	m_linearNodes.clear();
	m_linearNodes.shrink_to_fit();
	m_linearNodes.reserve(calcTotalNodes(rootNode));
	m_primitives.clear();
	m_primitives.shrink_to_fit();
	m_primitives.reserve(calcTotalPrimitives(rootNode));

	buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode, 0);

	out_linearNodes->clear();
	out_linearNodes->shrink_to_fit();
	out_primitives->clear();
	out_primitives->shrink_to_fit();

	m_linearNodes.swap(*out_linearNodes);
	m_primitives.swap(*out_primitives);
}

const BvhInfoNode* BvhBuilder::buildBinaryBvhInfoNodeRecursive(const std::vector<BvhPrimitiveInfo>& primitives, const ENodeSplitMethod splitMethod)
{
	m_infoNodes.push_back(std::make_unique<BvhInfoNode>());
	BvhInfoNode* node = m_infoNodes.back().get();

	AABB nodeAABB(primitives.empty() ? AABB() : primitives.front().aabb);
	for(const auto primitive : primitives)
	{
		nodeAABB = AABB::makeUnioned(nodeAABB, primitive.aabb);
	}

	if(primitives.size() <= 1)
	{
		*node = BvhInfoNode::makeBinaryLeaf(primitives, nodeAABB);

		if(primitives.empty())
		{
			std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), leaf node without primitive detected" << std::endl;
		}
	}
	else
	{
		AABB centroidsAABB(primitives.front().aabbCentroid);
		for(const auto primitive : primitives)
		{
			centroidsAABB = AABB::makeUnioned(centroidsAABB, primitive.aabbCentroid);
		}

		Vector3R extents = centroidsAABB.calcExtents();
		if(extents.hasNegativeComponent())
		{
			std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), negative AABB extent detected" << std::endl;
			extents.absLocal();
		}

		const int32 maxDimension = extents.maxDimension();

		if(centroidsAABB.getMinVertex()[maxDimension] == centroidsAABB.getMaxVertex()[maxDimension])
		{
			*node = BvhInfoNode::makeBinaryLeaf(primitives, nodeAABB);
		}
		else
		{
			bool isSplitSuccess = false;
			std::vector<BvhPrimitiveInfo> primitivesA;
			std::vector<BvhPrimitiveInfo> primitivesB;

			switch(splitMethod)
			{

			case ENodeSplitMethod::EQUAL_PRIMITIVES:
				isSplitSuccess = splitWithEqualPrimitives(primitives, maxDimension, &primitivesA, &primitivesB);
				break;

			case ENodeSplitMethod::SAH_BUCKETS:
				isSplitSuccess = splitWithSahBuckets(primitives, maxDimension, nodeAABB, centroidsAABB, &primitivesA, &primitivesB);
				break;

			default:
				std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), unknown split method detected" << std::endl;
				isSplitSuccess = false;
				break;

			}// end switch split method

			if(isSplitSuccess && (primitivesA.empty() || primitivesB.empty()))
			{
				std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), bad split detected" << std::endl;
				isSplitSuccess = false;
			}

			if(isSplitSuccess)
			{
				//std::cout << "#A: " << primitivesA.size() << "| #B: " << primitivesB.size() << std::endl;

				*node = BvhInfoNode::makeBinaryInternal(buildBinaryBvhInfoNodeRecursive(primitivesA, splitMethod),
				                                        buildBinaryBvhInfoNodeRecursive(primitivesB, splitMethod), 
				                                        maxDimension);
			}
			else
			{
				*node = BvhInfoNode::makeBinaryLeaf(primitives, nodeAABB);
			}
		}
	}

	return node;
}


void BvhBuilder::buildBinaryBvhLinearDepthFirstNodeRecursive(const BvhInfoNode* rootNode, std::size_t* out_nodeIndex)
{
	std::size_t nodeIndex = m_linearNodes.size();

	if(rootNode->isBinaryLeaf())
	{
		m_linearNodes.push_back(BvhLinearNode::makeLeaf(rootNode->aabb, 
		                                                m_primitives.size(), 
		                                                static_cast<int32>(rootNode->primitives.size())));

		for(const auto& primitiveInfo : rootNode->primitives)
		{
			m_primitives.push_back(primitiveInfo.primitive);
		}
	}
	else if(rootNode->isBinaryInternal())
	{
		m_linearNodes.push_back(BvhLinearNode());

		std::size_t secondChildOffset = static_cast<std::size_t>(-1);
		buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode->children[0], nullptr);
		buildBinaryBvhLinearDepthFirstNodeRecursive(rootNode->children[1], &secondChildOffset);
		m_linearNodes[nodeIndex] = BvhLinearNode::makeInternal(rootNode->aabb, secondChildOffset, rootNode->splitAxis);
	}
	else
	{
		std::cerr << "warning: at BvhBuilder::buildBinaryBvhLinearDepthFirstNodeRecursive(), only binary node is acceptable" << std::endl;
		nodeIndex = static_cast<std::size_t>(-1);
	}

	if(out_nodeIndex)
	{
		*out_nodeIndex = nodeIndex;
	}
}

std::size_t BvhBuilder::calcTotalNodes(const BvhInfoNode* rootNode)
{
	std::size_t result = 1;
	result += rootNode->children[0] ? calcTotalNodes(rootNode->children[0]) : 0;
	result += rootNode->children[1] ? calcTotalNodes(rootNode->children[1]) : 0;
	return result;
}

std::size_t BvhBuilder::calcTotalPrimitives(const BvhInfoNode* rootNode)
{
	std::size_t result = rootNode->primitives.size();
	result += rootNode->children[0] ? calcTotalPrimitives(rootNode->children[0]) : 0;
	result += rootNode->children[1] ? calcTotalPrimitives(rootNode->children[1]) : 0;
	return result;
}

std::size_t BvhBuilder::calcMaxDepth(const BvhInfoNode* rootNode)
{
	std::size_t depth = rootNode->children[0] || rootNode->children[1] ? 1 : 0;
	std::size_t depthA = rootNode->children[0] ? calcMaxDepth(rootNode->children[0]) : 0;
	std::size_t depthB = rootNode->children[1] ? calcMaxDepth(rootNode->children[1]) : 0;
	depth += std::max(depthA, depthB);
	return depth;
}

bool BvhBuilder::splitWithEqualPrimitives(const std::vector<BvhPrimitiveInfo>& primitives, const int32 splitDimension, 
                                          std::vector<BvhPrimitiveInfo>* const out_partA,
                                          std::vector<BvhPrimitiveInfo>* const out_partB)
{
	if(primitives.size() < 2)
	{
		std::cerr << "warning: at BvhBuilder::splitWithEqualPrimitives(), number of primitives < 2, cannot split" << std::endl;
		return false;
	}

	const std::size_t midIndex = primitives.size() / 2 - 1;
	std::vector<BvhPrimitiveInfo> sortedPrims(primitives);

	std::nth_element(sortedPrims.begin(), sortedPrims.begin() + midIndex, sortedPrims.end(),
		[splitDimension](const BvhPrimitiveInfo& a, const BvhPrimitiveInfo& b)
		{
			return a.aabbCentroid[splitDimension] < b.aabbCentroid[splitDimension];
		});

	out_partA->clear();
	out_partB->clear();
	out_partA->shrink_to_fit();
	out_partB->shrink_to_fit();

	out_partA->insert(out_partA->end(), sortedPrims.begin(), sortedPrims.begin() + midIndex + 1);
	out_partB->insert(out_partB->end(), sortedPrims.begin() + midIndex + 1, sortedPrims.end());

	return true;
}

bool BvhBuilder::splitWithSahBuckets(const std::vector<BvhPrimitiveInfo>& primitives, const int32 splitDimension, 
                                     const AABB& primitivesAABB, const AABB& centroidsAABB, 
                                     std::vector<BvhPrimitiveInfo>* const out_partA,
                                     std::vector<BvhPrimitiveInfo>* const out_partB)
{
	if(primitives.size() < 2)
	{
		std::cerr << "warning: at BvhBuilder::splitWithSahBuckets(), number of primitives < 2, cannot split" << std::endl;
		return false;
	}

	const int32 numBuckets = 64;

	const int32    dim         = splitDimension;
	const Vector3R extents     = centroidsAABB.calcExtents();
	const real     splitExtent = extents[dim];

	BvhSahBucket buckets[numBuckets];
	for(const auto& primitive : primitives)
	{
		if(splitExtent < 0.0_r)
		{
			std::cerr << "warning: at BvhBuilder::splitWithSahBuckets(), primitive AABB split extent < 0, cannot split" << std::endl;
			return false;
		}

		const real factor = (primitive.aabbCentroid[dim] - centroidsAABB.getMinVertex()[dim]) / splitExtent;
		int32 bucketIndex = static_cast<int32>(factor * numBuckets);
		bucketIndex = (bucketIndex == numBuckets) ? bucketIndex - 1 : bucketIndex;

		buckets[bucketIndex].aabb = (buckets[numBuckets].isEmpty()) ? 
		                            primitive.aabb : buckets[bucketIndex].aabb.unionWith(primitive.aabb);
		buckets[bucketIndex].numPrimitives++;
	}

	const real traversalCost = 1.0_r / 8.0_r;
	const real intersectCost = 1.0_r;

	real costs[numBuckets - 1] = {0.0_r};
	for(int32 i = 0; i < numBuckets - 1; i++)
	{
		std::size_t numPrimitivesA = 0;
		AABB aabbA;
		for(int32 j = 0; j <= i; j++)
		{
			if(!buckets[j].isEmpty())
			{
				aabbA = aabbA.isPoint() ? buckets[j].aabb : AABB::makeUnioned(aabbA, buckets[j].aabb);
			}
			
			numPrimitivesA += buckets[j].numPrimitives;
		}

		std::size_t numPrimitivesB = 0;
		AABB aabbB;
		for(int32 j = i + 1; j < numBuckets; j++)
		{
			if(!buckets[j].isEmpty())
			{
				aabbB = aabbB.isPoint() ? buckets[j].aabb : AABB::makeUnioned(aabbB, buckets[j].aabb);
			}

			numPrimitivesB += buckets[j].numPrimitives;
		}

		const real probTestingA = aabbA.calcSurfaceArea() / primitivesAABB.calcSurfaceArea();
		const real probTestingB = aabbB.calcSurfaceArea() / primitivesAABB.calcSurfaceArea();

		costs[i] = traversalCost + 
		           static_cast<real>(numPrimitivesA) * intersectCost * probTestingA + 
		           static_cast<real>(numPrimitivesB) * intersectCost * probTestingB;
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
	const real noSplitCost  = intersectCost * static_cast<real>(primitives.size());

	const std::size_t maxPrimitives = 256;

	if(minSplitCost < noSplitCost || primitives.size() > maxPrimitives)
	{
		out_partA->clear();
		out_partB->clear();
		out_partA->shrink_to_fit();
		out_partB->shrink_to_fit();

		for(const auto& primitive : primitives)
		{
			const real factor = (primitive.aabbCentroid[dim] - centroidsAABB.getMinVertex()[dim]) / splitExtent;
			int32 bucketIndex = static_cast<int32>(factor * numBuckets);
			bucketIndex = (bucketIndex == numBuckets) ? bucketIndex - 1 : bucketIndex;

			if(bucketIndex <= minCostIndex)
				out_partA->push_back(primitive);
			else
				out_partB->push_back(primitive);
		}

		return true;
	}
	else
	{
		return false;
	}
}

}// end namespace ph