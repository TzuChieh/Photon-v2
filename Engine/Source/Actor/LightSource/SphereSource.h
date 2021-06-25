#pragma once

#include "Actor/LightSource/AreaSource.h"

namespace ph
{

class SphereSource final : public AreaSource
{
public:
	SphereSource();
	SphereSource(real radius, const math::Vector3R& linearSrgbColor, real numWatts);
	SphereSource(real radius, const SampledSpectrum& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(CookingContext& context) const override;

	void setRadius(real radius);

private:
	real m_radius;
};

}// end namespace ph
