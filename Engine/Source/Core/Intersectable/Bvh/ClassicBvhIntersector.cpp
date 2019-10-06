#include "Core/Intersectable/Bvh/ClassicBvhIntersector.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Actor/CookedDataStorage.h"
#include "Core/Intersectable/Bvh/BvhInfoNode.h"
#include "Core/Intersectable/Bvh/BvhBuilder.h"
#include "Math/Geometry/TAABB3D.h"

#include <iostream>
#include <limits>

namespace ph
{

const int32 ClassicBvhIntersector::NODE_STACK_SIZE;

ClassicBvhIntersector::~ClassicBvhIntersector() = default;

void ClassicBvhIntersector::update(const CookedDataStorage& cookedActors)
{
	std::vector<const Intersectable*> intersectables;
	for(const auto& intersectable : cookedActors.intersectables())
	{
		// HACK
		AABB3D aabb;
		intersectable->calcAABB(&aabb);
		if(!aabb.isFiniteVolume())
		{
			continue;
		}

		intersectables.push_back(intersectable.get());
	}

	rebuildWithIntersectables(std::move(intersectables));

	// printing information about the constructed BVH

	/*std::cout << "intersector:             classic BVH" << std::endl;
	std::cout << "total primitives inside: " << BvhBuilder::calcTotalIntersectables(root) << " (info), " << 
	             m_intersectables.size() << " (linear)" << std::endl;
	std::cout << "total nodes:             " << BvhBuilder::calcTotalNodes(root) << " (info), " <<
	             m_nodes.size() << " (linear)" << std::endl;
	std::cout << "max tree depth:          " << treeDepth << std::endl;*/
}

bool ClassicBvhIntersector::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	const int32 isDirNeg[3] = {ray.getDirection().x < 0, ray.getDirection().y < 0, ray.getDirection().z < 0};

	std::size_t todoNodes[NODE_STACK_SIZE];
	int32       numTodoNodes     = 0;
	std::size_t currentNodeIndex = 0;

	Ray bvhRay(ray);
	HitProbe closestProbe;

	real minT    = 0.0_r;
	real maxT    = 0.0_r;
	real minHitT = std::numeric_limits<real>::max();

	// TODO: possibly make use of minT & maxT found by AABB intersection?

	while(!m_nodes.empty())
	{
		const BvhLinearNode& node = m_nodes[currentNodeIndex];

		if(node.aabb.isIntersectingVolume(bvhRay, &minT, &maxT))
		{
			if(node.isLeaf())
			{
				for(int32 i = 0; i < node.numPrimitives; i++)
				{
					HitProbe currentProbe(probe);
					if(m_intersectables[node.primitivesOffset + i]->isIntersecting(bvhRay, currentProbe))
					{
						const real hitT = currentProbe.getHitRayT();
						if(hitT < minHitT)
						{
							minHitT = hitT;
							bvhRay.setMaxT(hitT);
							closestProbe = currentProbe;
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
	
	// FIXME: need better condition
	if(minHitT < std::numeric_limits<real>::max())
	{
		probe = closestProbe;
		return true;
	}
	else
	{
		return false;
	}
}

void ClassicBvhIntersector::calcAABB(AABB3D* const out_aabb) const
{
	if(m_intersectables.empty())
	{
		*out_aabb = AABB3D();
		return;
	}

	m_intersectables.front()->calcAABB(out_aabb);
	for(auto intersectable : m_intersectables)
	{
		AABB3D aabb;
		intersectable->calcAABB(&aabb);
		out_aabb->unionWith(aabb);
	}
}

void ClassicBvhIntersector::rebuildWithIntersectables(std::vector<const Intersectable*> intersectables)
{
	BvhBuilder bvhBuilder(EBvhType::SAH_BUCKET);
	const BvhInfoNode* root = bvhBuilder.buildInformativeBinaryBvh(intersectables);
	bvhBuilder.buildLinearDepthFirstBinaryBvh(root, &m_nodes, &m_intersectables);

	// TODO: try to turn some checking into assertions

	const std::size_t treeDepth = BvhBuilder::calcMaxDepth(root);
	if(treeDepth > NODE_STACK_SIZE)
	{
		std::cerr << "warning: at ClassicBvhIntersector::update(), " 
		          << "BVH depth exceeds stack size (" << NODE_STACK_SIZE << ")" << std::endl;
	}

	if(m_intersectables.empty() || m_nodes.empty())
	{
		std::cerr << "warning: at ClassicBvhIntersector::update(), " 
		          << "no intersectable or node is present" << std::endl;
	}
}

}// end namespace ph