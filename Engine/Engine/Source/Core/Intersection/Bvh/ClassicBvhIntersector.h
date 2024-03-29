#pragma once

#include "Core/Intersection/Intersector.h"
#include "Core/Intersection/Bvh/BvhLinearNode.h"

#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

class ClassicBvhIntersector : public Intersector
{
public:
	void update(TSpanView<const Intersectable*> intersectables) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	math::AABB3D calcAABB() const override;

	void rebuildWithIntersectables(TSpanView<const Intersectable*> intersectables);

private:
	std::vector<const Intersectable*> m_intersectables;
	std::vector<BvhLinearNode>        m_nodes;

	static const int32 NODE_STACK_SIZE = 64;
};

}// end namespace ph
