#include "Actor/LightSource/SphereSource.h"
#include "Actor/Geometry/GSphere.h"

namespace ph
{

SphereSource::SphereSource() :
	AreaSource()
{
	setRadius(1.0_r);
}

SphereSource::SphereSource(const real radius, const math::Vector3R& linearSrgbColor, real numWatts) :
	AreaSource(linearSrgbColor, numWatts)
{
	setRadius(radius);
}

SphereSource::SphereSource(const real radius, const math::Spectrum& color, real numWatts) :
	AreaSource(color, numWatts)
{
	setRadius(radius);
}

std::shared_ptr<Geometry> SphereSource::genAreas(CookingContext& ctx) const
{
	return std::make_shared<GSphere>(m_radius);
}

void SphereSource::setRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_radius = radius;
}

}// end namespace ph
