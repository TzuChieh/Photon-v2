#include "Actor/Light/ASphereLight.h"
#include "Actor/Geometry/GSphere.h"

#include <Common/assertion.h>

namespace ph
{

std::shared_ptr<Geometry> ASphereLight::getArea(const CookingContext& ctx) const
{
	auto sphere = TSdl<GSphere>::makeResource();
	sphere->setRadius(m_radius);
	return sphere;
}

void ASphereLight::setRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_radius = radius;
}

}// end namespace ph
