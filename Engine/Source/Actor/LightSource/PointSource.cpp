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
	m_sphereSource(POINT_SOURCE_RADIUS, linearSrgbColor, numWatts)
{}

PointSource::PointSource(const Spectrum& color, real numWatts) :
	m_sphereSource(POINT_SOURCE_RADIUS, color, numWatts)
{}

std::shared_ptr<Geometry> PointSource::genAreas(CookingContext& context) const
{
	return m_sphereSource.genAreas(context);
}

}// end namespace ph
