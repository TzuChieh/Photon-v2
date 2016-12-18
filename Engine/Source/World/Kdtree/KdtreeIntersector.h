#pragma once

#include "World/Intersector.h"
#include "World/Kdtree/KdtreeNode.h"
#include "Entity/Primitive/Primitive.h"

#include <vector>

namespace ph
{

class KdtreeIntersector final : public Intersector
{
public:
	KdtreeIntersector();
	virtual ~KdtreeIntersector() override;

	virtual void update(const std::vector<std::unique_ptr<Primitive>>& primitives) override;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const override;

private:
	std::vector<const Primitive*> m_nodePrimitiveBuffer;
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph