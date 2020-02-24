#pragma once

#include "Core/Intersectable/Intersector.h"
#include "Common/primitive_type.h"
#include "Core/Intersectable/Bvh/BvhLinearNode.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersectable;

class ClassicBvhIntersector : public Intersector
{
public:
	void update(const CookedDataStorage& cookedActors) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	math::AABB3D calcAABB() const override;

	void rebuildWithIntersectables(std::vector<const Intersectable*> intersectables);

private:
	std::vector<const Intersectable*> m_intersectables;
	std::vector<BvhLinearNode>        m_nodes;

	static const int32 NODE_STACK_SIZE = 64;
};

}// end namespace ph
