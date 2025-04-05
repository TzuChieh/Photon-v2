#pragma once

#include "Engine/Core/Intersection/Intersector.h"
#include "Engine/Core/Intersection/Kdtree/KdtreeNode.h"
#include "Engine/Core/Intersection/Intersectable.h"

#include <vector>

namespace ph
{

class KdtreeIntersector : public Intersector
{
public:
	KdtreeIntersector();

	void update(TSpanView<const Intersectable*> intersectables) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	math::AABB3D calcAABB() const override;

private:
	std::vector<const Intersectable*> m_nodeIntersectableBuffer;
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph
