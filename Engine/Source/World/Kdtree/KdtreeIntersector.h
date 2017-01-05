#pragma once

#include "World/Intersector.h"
#include "World/Kdtree/KdtreeNode.h"
#include "Core/Primitive/Primitive.h"

#include <vector>

namespace ph
{

class KdtreeIntersector final : public Intersector
{
public:
	KdtreeIntersector();
	virtual ~KdtreeIntersector() override;

	virtual void update(const CookedActorStorage& cookedActors) override;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const override;
	virtual bool isIntersecting(const Ray& ray) const override;

private:
	std::vector<const Primitive*> m_nodePrimitiveBuffer;
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph