#pragma once

#include "World/Intersector/Intersector.h"
#include "World/Intersector/Kdtree/KdtreeNode.h"
#include "Core/Intersectable/Intersectable.h"

#include <vector>

namespace ph
{

class KdtreeIntersector final : public Intersector
{
public:
	KdtreeIntersector();
	virtual ~KdtreeIntersector() override;

	virtual void update(const CookedActorStorage& cookedActors) override;
	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;
	virtual bool isIntersecting(const Ray& ray) const override;

private:
	std::vector<const Intersectable*> m_nodeIntersectableBuffer;
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph