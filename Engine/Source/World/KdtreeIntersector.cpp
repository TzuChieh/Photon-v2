#include "World/KdtreeIntersector.h"

#include <iostream>

namespace ph
{

KdtreeIntersector::~KdtreeIntersector() = default;

void KdtreeIntersector::construct(const std::vector<std::unique_ptr<Primitive>>& primitives)
{
	std::vector<const Primitive*> primPtrs;
	for(const auto& primitive : primitives)
	{
		primPtrs.push_back(primitive.get());
	}

	m_rootKdtreeNode.buildTree(primPtrs);
}

bool KdtreeIntersector::isIntersecting(const Ray& ray, Intersection* out_intersection) const
{
	//std::cout << "isIntersecting" << std::endl;
	bool isFound = m_rootKdtreeNode.findClosestIntersection(ray, out_intersection);
	//std::cout << "found? " << isFound << std::endl;

	return isFound;
}

}// end namespace ph