#pragma once

#include "Actor/LightSource/AreaSource.h"

namespace ph
{

class RectangleSource final : public AreaSource
{
public:
	RectangleSource();
	RectangleSource(real width, real height, const math::Vector3R& linearSrgbColor, real numWatts);
	RectangleSource(real width, real height, const Spectrum& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(ActorCookingContext& ctx) const override;

	void setDimension(real width, real height);

private:
	real m_width;
	real m_height;
};

}// end namespace ph
