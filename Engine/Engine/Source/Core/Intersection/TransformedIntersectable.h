#pragma once

#include "Core/Intersection/Intersectable.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class TransformedIntersectable : public Intersectable
{
public:
	TransformedIntersectable();

	TransformedIntersectable(
		const Intersectable*   intersectable, 
		const math::Transform* localToWorld,
		const math::Transform* worldToLocal);

	bool isOccluding(const Ray& ray) const override;
	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                                    HitDetail* out_detail) const override;
	bool mayOverlapVolume(const math::AABB3D& aabb) const override;
	math::AABB3D calcAABB() const override;

protected:
	const Intersectable*   m_intersectable;
	const math::Transform* m_localToWorld;
	const math::Transform* m_worldToLocal;
};

}// end namespace ph
