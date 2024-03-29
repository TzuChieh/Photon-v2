#include "Actor/LightSource/PointSource.h"
#include "Actor/Geometry/GSphere.h"

namespace ph
{

namespace
{

constexpr real POINT_SOURCE_RADIUS = 0.005_r;

}

PointSource::PointSource() : 
	PointSource(math::Vector3R(1, 1, 1), 100.0_r)
{}

PointSource::PointSource(const math::Vector3R& linearSrgbColor, const real numWatts) :
	AreaSource(linearSrgbColor, numWatts)
{}

PointSource::PointSource(const math::Spectrum& color, real numWatts) :
	AreaSource(color, numWatts)
{}

std::shared_ptr<Geometry> PointSource::genAreas(CookingContext& ctx) const
{
	auto sphere = TSdl<GSphere>::makeResource();
	sphere->setRadius(POINT_SOURCE_RADIUS);

	return sphere;
}

}// end namespace ph
