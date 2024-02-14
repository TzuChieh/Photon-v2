#include "Core/Intersection/Bvh/ClassicBvhIntersector.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"
#include "Core/Intersection/Bvh/BvhInfoNode.h"
#include "Core/Intersection/Bvh/BvhBuilder.h"
#include "Math/Geometry/TAABB3D.h"

#include <iostream>
#include <limits>

namespace ph
{

const int32 ClassicBvhIntersector::NODE_STACK_SIZE;

void ClassicBvhIntersector::update(TSpanView<const Intersectable*> intersectables)
{
	std::vector<const Intersectable*> treeIntersectables;
	for(const auto& intersectable : intersectables)
	{
		// HACK
		if(!intersectable->calcAABB().isFiniteVolume())
		{
			continue;
		}

		treeIntersectables.push_back(intersectable);
	}

	rebuildWithIntersectables(treeIntersectables);

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
	const int32 isDirNeg[3] = {ray.getDirection().x() < 0, ray.getDirection().y() < 0, ray.getDirection().z() < 0};

	std::size_t todoNodes[NODE_STACK_SIZE];
	int32       numTodoNodes     = 0;
	std::size_t currentNodeIndex = 0;

	Ray bvhRay(ray);
	HitProbe closestProbe;
	real minHitT = std::numeric_limits<real>::max();

	// TODO: possibly make use of minT & maxT found by AABB intersection?

	while(!m_nodes.empty())
	{
		const BvhLinearNode& node = m_nodes[currentNodeIndex];

		if(node.aabb.isIntersectingVolume(bvhRay.getSegment()))
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

math::AABB3D ClassicBvhIntersector::calcAABB() const
{
	if(m_intersectables.empty())
	{
		// FIXME: return an invalid one or?
		return math::AABB3D();
	}

	math::AABB3D unionedAabb = m_intersectables.front()->calcAABB();
	for(auto intersectable : m_intersectables)
	{
		unionedAabb.unionWith(intersectable->calcAABB());
	}

	return unionedAabb;
}

void ClassicBvhIntersector::rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables)
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
