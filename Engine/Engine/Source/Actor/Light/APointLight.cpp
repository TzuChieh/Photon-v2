#include "Actor/Light/APointLight.h"
#include "Actor/Geometry/GSphere.h"
#include "Common/primitive_type.h"

namespace ph
{

namespace
{

constexpr real POINT_SOURCE_RADIUS = 0.005_r;

}

std::shared_ptr<Geometry> APointLight::getArea(const CookingContext& ctx) const
{
	auto sphere = TSdl<GSphere>::makeResource();
	sphere->setRadius(POINT_SOURCE_RADIUS);
	return sphere;
}

}// end namespace ph
