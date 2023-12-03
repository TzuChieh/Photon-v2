#pragma once

#include "Core/Intersectable/Intersector.h"

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

private:
	std::vector<const Intersectable*> m_intersectables;
};

}// end namespace ph
