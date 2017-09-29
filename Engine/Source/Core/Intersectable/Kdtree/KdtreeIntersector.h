#pragma once

#include "Core/Intersectable/Intersector.h"
#include "Core/Intersectable/Kdtree/KdtreeNode.h"
#include "Core/Intersectable/Intersectable.h"

#include <vector>

namespace ph
{

class IntersectionProbe;
class IntersectionDetail;

class KdtreeIntersector final : public Intersector
{
public:
	KdtreeIntersector();
	virtual ~KdtreeIntersector() override;

	virtual void update(const CookedActorStorage& cookedActors) override;
	virtual bool isIntersecting(const Ray& ray, IntersectionProbe* out_probe) const override;

private:
	std::vector<const Intersectable*> m_nodeIntersectableBuffer;
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph