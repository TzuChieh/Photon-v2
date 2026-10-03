#pragma once

#include "Engine/Core/Intersection/Intersector.h"

#include <vector>

namespace ph
{

class BruteForceIntersector : public Intersector
{
public:
	void update(TSpanView<const Intersectable*> intersectables) override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	bool isOccluding(const Ray& ray) const override;
	math::AABB3D calcAABB() const override;
	
	void forEachIntersection(const Ray& ray, const HitVisitor& visitor) const override;
	bool supportsForEachIntersection() const override;

private:
	std::vector<const Intersectable*> m_intersectables;
};

}// end namespace ph
