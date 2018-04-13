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
	virtual ~ClassicBvhIntersector() override;

	virtual void update(const CookedDataStorage& cookedActors) override;
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	virtual void calcAABB(AABB3D* out_aabb) const override;

private:
	std::vector<const Intersectable*> m_intersectables;
	std::vector<BvhLinearNode>        m_nodes;

	static const int32 NODE_STACK_SIZE = 64;
};

}// end namespace ph