#pragma once

#include "Core/Intersectable/Bvh/EBvhType.h"
#include "Core/Intersectable/Bvh/BvhIntersectableInfo.h"
#include "Core/Intersectable/Bvh/BvhLinearNode.h"

#include <memory>
#include <vector>

namespace ph
{

class Intersectables;
class BvhInfoNode;

class BvhBuilder final
{
public:
	static std::size_t calcTotalNodes(const BvhInfoNode* rootNode);
	static std::size_t calcTotalIntersectables(const BvhInfoNode* rootNode);
	static std::size_t calcMaxDepth(const BvhInfoNode* rootNode);

public:
	BvhBuilder(EBvhType type);

	const BvhInfoNode* buildInformativeBinaryBvh(const std::vector<const Intersectable*>& intersectables);
	void buildLinearDepthFirstBinaryBvh(const BvhInfoNode* rootNode, 
	                                    std::vector<BvhLinearNode>* out_linearNodes, 
	                                    std::vector<const Intersectable*>* out_intersectables);

private:
	EBvhType m_type;
	std::vector<std::unique_ptr<BvhInfoNode>> m_infoNodes;
	std::vector<BvhLinearNode> m_linearNodes;
	std::vector<const Intersectable*> m_intersectables;

	enum class ENodeSplitMethod
	{
		EQUAL_INTERSECTABLES, 
		SAH_BUCKETS, 
		SAH_EDGE_SORT
	};

	const BvhInfoNode* buildBinaryBvhInfoNodeRecursive(const std::vector<BvhIntersectableInfo>& intersectables, 
	                                                   ENodeSplitMethod splitMethod);
	void buildBinaryBvhLinearDepthFirstNodeRecursive(const BvhInfoNode* rootNode, 
	                                                 std::size_t* out_nodeIndex);

	bool splitWithEqualIntersectables(const std::vector<BvhIntersectableInfo>& intersectables, 
	                                  int32 splitDimension,
	                                  std::vector<BvhIntersectableInfo>* out_partA,
	                                  std::vector<BvhIntersectableInfo>* out_partB);

	bool splitWithSahBuckets(const std::vector<BvhIntersectableInfo>& intersectables, 
	                         int32 splitDimension,
	                         const AABB3D& primitivesAABB, const AABB3D& centroidsAABB,
	                         std::vector<BvhIntersectableInfo>* out_partA,
	                         std::vector<BvhIntersectableInfo>* out_partB);
};

}// end namespace ph