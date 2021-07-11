#pragma once

#include "Actor/LightSource/AreaSource.h"
#include "Actor/LightSource/SphereSource.h"

namespace ph
{

class PointSource : public AreaSource
{
public:
	PointSource();
	PointSource(const math::Vector3R& linearSrgbColor, real numWatts);
	PointSource(const Spectrum& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(ActorCookingContext& ctx) const override;

private:
	SphereSource m_sphereSource;
};

}// end namespace ph
