#include "Actor/LightSource/PointSource.h"

namespace ph
{

namespace
{

static constexpr real POINT_SOURCE_RADIUS = 0.005_r;

}

PointSource::PointSource() : 
	PointSource(math::Vector3R(1, 1, 1), 100.0_r)
{}

PointSource::PointSource(const math::Vector3R& linearSrgbColor, const real numWatts) :
	SphereSource(POINT_SOURCE_RADIUS, linearSrgbColor, numWatts)
{}

PointSource::PointSource(const Spectrum& color, real numWatts) :
	SphereSource(POINT_SOURCE_RADIUS, color, numWatts)
{}

}// end namespace ph
