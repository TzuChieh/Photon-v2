#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Core/Quantity/Spectrum.h"
#include "Actor/Geometry/Geometry.h"

#include <memory>

namespace ph
{

class Image;

class AreaSource : public LightSource
{
public:
	AreaSource();
	AreaSource(const math::Vector3R& linearSrgbColor, real numWatts);
	AreaSource(const SampledSpectrum& color, real numWatts);

	virtual std::shared_ptr<Geometry> genAreas(CookingContext& context) const = 0;

	std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

	std::shared_ptr<Geometry> genGeometry(CookingContext& context) const final override;

private:
	SampledSpectrum m_color;
	real            m_numWatts;
};

}// end namespace ph
