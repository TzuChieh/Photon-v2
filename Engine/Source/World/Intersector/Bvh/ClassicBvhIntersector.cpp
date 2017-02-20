#include "World/Intersector/Bvh/ClassicBvhIntersector.h"
#include "Core/Intersection.h"
#include "Core/Ray.h"
#include "Core/CookedActorStorage.h"
#include "World/Intersector/Bvh/BvhInfoNode.h"
#include "World/Intersector/Bvh/BvhBuilder.h"

#include <iostream>
#include <limits>

namespace ph
{

const int32 ClassicBvhIntersector::NODE_STACK_SIZE;

ClassicBvhIntersector::~ClassicBvhIntersector() = default;

void ClassicBvhIntersector::update(const CookedActorStorage& cookedActors)
{
	std::vector<const Primitive*> primitives;
	for(const auto& primitive : cookedActors.primitives())
	{
		primitives.push_back(primitive.get());
	}

	BvhBuilder bvhBuilder(EBvhType::HALF);
	const BvhInfoNode* root = bvhBuilder.buildInformativeBinaryBvh(primitives);
	bvhBuilder.buildLinearDepthFirstBinaryBvh(root, &m_nodes, &m_primitives);

	// checking and printing information about the constructed BVH

	const std::size_t treeDepth = BvhBuilder::calcMaxDepth(root);
	if(treeDepth > NODE_STACK_SIZE)
	{
		std::cerr << "warning: at ClassicBvhIntersector::update(), BVH depth exceeds stack size (64)" << std::endl;
	}

	if(m_primitives.empty() || m_nodes.empty())
	{
		std::cerr << "warning: at ClassicBvhIntersector::update(), no primitive or node is present" << std::endl;
	}

	std::cout << "intersector:             classic BVH" << std::endl;
	std::cout << "total primitives inside: " << BvhBuilder::calcTotalPrimitives(root) << " (info), " << 
	             m_primitives.size() << " (linear)" << std::endl;
	std::cout << "total nodes:             " << BvhBuilder::calcTotalNodes(root) << " (info), " <<
	             m_nodes.size() << " (linear)" << std::endl;
	std::cout << "max tree depth:          " << treeDepth << std::endl;
}

bool ClassicBvhIntersector::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	std::size_t todoNodes[NODE_STACK_SIZE];
	int32       numTodoNodes     = 0;
	std::size_t currentNodeIndex = 0;

	Ray bvhRay(ray);
	const int32 isDirNeg[3] = {bvhRay.getDirection().x < 0.0_r, bvhRay.getDirection().y < 0.0_r, bvhRay.getDirection().z < 0.0_r};
	Intersection intersection;

	real minT    = 0.0_r;
	real maxT    = 0.0_r;
	real minHitT = std::numeric_limits<real>::infinity();

	// TODO: thinking of making use of minT & maxT found by AABB intersection

	while(!m_nodes.empty())
	{
		const BvhLinearNode& node = m_nodes[currentNodeIndex];

		if(node.aabb.isIntersectingVolume(bvhRay, &minT, &maxT))
		{
			if(node.isLeaf())
			{
				for(int32 i = 0; i < node.numPrimitives; i++)
				{
					if(m_primitives[node.primitivesOffset + i]->isIntersecting(bvhRay, &intersection))
					{
						const real hitT = intersection.getHitRayT();
						if(hitT < minHitT)
						{
							minHitT = hitT;
							bvhRay.setMaxT(hitT);
							*out_intersection = intersection;
						}
					}
				}

				if(numTodoNodes == 0)
					break;
				else
					currentNodeIndex = todoNodes[--numTodoNodes];
			}
			else
			{
				if(isDirNeg[node.splittedAxis])
				{
					todoNodes[numTodoNodes++] = currentNodeIndex + 1;
					currentNodeIndex = node.secondChildOffset;
				}
				else
				{
					todoNodes[numTodoNodes++] = node.secondChildOffset;
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else
		{
			if(numTodoNodes == 0)
				break;
			else
				currentNodeIndex = todoNodes[--numTodoNodes];
		}
	}
	
	return minHitT != std::numeric_limits<real>::infinity();
}

bool ClassicBvhIntersector::isIntersecting(const Ray& ray) const
{
	// HACK
	Intersection intersection;
	return isIntersecting(ray, &intersection);
}

}// end namespace ph