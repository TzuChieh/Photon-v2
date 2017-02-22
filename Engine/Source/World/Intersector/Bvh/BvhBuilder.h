#pragma once

#include "World/Intersector/Bvh/EBvhType.h"
#include "World/Intersector/Bvh/BvhPrimitiveInfo.h"
#include "World/Intersector/Bvh/BvhLinearNode.h"

#include <memory>
#include <vector>

namespace ph
{

class Primitive;
class BvhInfoNode;

class BvhBuilder final
{
public:
	static std::size_t calcTotalNodes(const BvhInfoNode* rootNode);
	static std::size_t calcTotalPrimitives(const BvhInfoNode* rootNode);
	static std::size_t calcMaxDepth(const BvhInfoNode* rootNode);

public:
	BvhBuilder(const EBvhType type);

	const BvhInfoNode* buildInformativeBinaryBvh(const std::vector<const Primitive*>& primitives);
	void buildLinearDepthFirstBinaryBvh(const BvhInfoNode* rootNode, 
	                                    std::vector<BvhLinearNode>* const out_linearNodes, 
	                                    std::vector<const Primitive*>* const out_primitives);

private:
	EBvhType m_type;
	std::vector<std::unique_ptr<BvhInfoNode>> m_infoNodes;
	std::vector<BvhLinearNode> m_linearNodes;
	std::vector<const Primitive*> m_primitives;

	enum class ENodeSplitMethod
	{
		EQUAL_PRIMITIVES, 
		SAH_BUCKETS, 
		SAH_EDGE_SORT
	};

	const BvhInfoNode* buildBinaryBvhInfoNodeRecursive(const std::vector<BvhPrimitiveInfo>& primitives, const ENodeSplitMethod splitMethod);
	void buildBinaryBvhLinearDepthFirstNodeRecursive(const BvhInfoNode* rootNode, std::size_t* out_nodeIndex);

	bool splitWithEqualPrimitives(const std::vector<BvhPrimitiveInfo>& primitives, const int32 splitDimension, 
	                              std::vector<BvhPrimitiveInfo>* const out_partA, 
	                              std::vector<BvhPrimitiveInfo>* const out_partB);

	bool splitWithSahBuckets(const std::vector<BvhPrimitiveInfo>& primitives, const int32 splitDimension, 
	                         const AABB& primitivesAABB, const AABB& centroidsAABB, 
	                         std::vector<BvhPrimitiveInfo>* const out_partA,
	                         std::vector<BvhPrimitiveInfo>* const out_partB);
};

}// end namespace ph