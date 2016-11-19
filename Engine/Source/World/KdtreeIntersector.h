#pragma once

#include "World/Intersector.h"
#include "World/KdtreeNode.h"

namespace ph
{

class KdtreeIntersector final : public Intersector
{
public:
	virtual ~KdtreeIntersector() override;

	virtual void construct(const std::vector<std::unique_ptr<Primitive>>& primitives) override;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const override;

private:
	KdtreeNode m_rootKdtreeNode;
};

}// end namespace ph