#pragma once

#include "World/Intersector.h"
#include "World/Kdtree/KdtreeNode.h"

#include <vector>

namespace ph
{

class Triangle;

class KdtreeIntersector final : public Intersector
{
public:
	KdtreeIntersector();
	virtual ~KdtreeIntersector() override;

	virtual void construct() override;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const override;

private:
	std::vector<const Triangle*> m_nodeTriangleBuffer;
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph