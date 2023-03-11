#pragma once

#include "Core/Intersectable/Intersector.h"
#include "Core/Intersectable/Kdtree/KdtreeNode.h"
#include "Core/Intersectable/Intersectable.h"

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
