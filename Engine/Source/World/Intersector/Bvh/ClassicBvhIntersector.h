#pragma once

#include "World/Intersector/Intersector.h"
#include "Common/primitive_type.h"
#include "World/Intersector/Bvh/BvhLinearNode.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;

class ClassicBvhIntersector : public Intersector
{
public:
	virtual ~ClassicBvhIntersector() override;

	virtual void update(const CookedActorStorage& cookedActors) override;
	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const override;
	virtual bool isIntersecting(const Ray& ray) const override;

private:
	std::vector<const Primitive*> m_primitives;
	std::vector<BvhLinearNode>    m_nodes;

	static const int32 NODE_STACK_SIZE = 32;
};

}// end namespace ph