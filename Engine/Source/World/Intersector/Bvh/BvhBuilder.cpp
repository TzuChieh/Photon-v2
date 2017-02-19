#include "World/Intersector/Bvh/BvhBuilder.h"
#include "World/Intersector/Bvh/BvhInfoNode.h"
#include "Core/Primitive/Primitive.h"
#include "Math/TVector3.h"

#include <iostream>
#include <algorithm>

namespace ph
{

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

const BvhInfoNode* BvhBuilder::buildBinaryBvhInfoNodeRecursive(std::vector<BvhPrimitiveInfo>& primitives, const ENodeSplitMethod splitMethod)
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
		AABB centroidAABB(primitives.front().aabbCentroid);
		for(const auto primitive : primitives)
		{
			centroidAABB = AABB::makeUnioned(centroidAABB, primitive.aabbCentroid);
		}

		Vector3R extents = centroidAABB.calcExtents();
		if(extents.hasNegativeComponent())
		{
			std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), negative AABB extent detected" << std::endl;
			extents.absLocal();
		}

		const int32 maxDimension = extents.maxDimension();

		if(centroidAABB.getMinVertex()[maxDimension] == centroidAABB.getMaxVertex()[maxDimension])
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
			{
				const std::size_t midIndex = primitives.size() / 2 - 1;
				std::nth_element(primitives.begin(), primitives.begin() + midIndex, primitives.end(),
					[maxDimension](const BvhPrimitiveInfo& a, const BvhPrimitiveInfo& b)
					{
						return a.aabbCentroid[maxDimension] < b.aabbCentroid[maxDimension];
					});

				primitivesA.insert(primitivesA.end(), primitives.begin(), primitives.begin() + midIndex + 1);
				primitivesB.insert(primitivesB.end(), primitives.begin() + midIndex + 1, primitives.end());
				isSplitSuccess = true;
				break;
			}

			default:
			{
				std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), unknown split method detected" << std::endl;
				isSplitSuccess = false;
				break;
			}

			}// end switch split method

			if(isSplitSuccess)
			{
				//std::cout << "#A: " << primitivesA.size() << "| #B: " << primitivesB.size() << std::endl;

				*node = BvhInfoNode::makeBinaryInternal(buildBinaryBvhInfoNodeRecursive(primitivesA, splitMethod),
				                                        buildBinaryBvhInfoNodeRecursive(primitivesB, splitMethod), 
				                                        maxDimension);
			}
			else
			{
				std::cerr << "warning: at BvhBuilder::buildBinaryBvhNodeRecursive(), node split unsuccessful, making leaf" << std::endl;
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

}// end namespace ph