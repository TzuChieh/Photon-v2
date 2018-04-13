#pragma once

#include "Actor/LightSource/AreaSource.h"
#include "Actor/LightSource/SphereSource.h"

namespace ph
{

class PointSource final : public AreaSource, public TCommandInterface<PointSource>
{
public:
	PointSource();
	PointSource(const Vector3R& linearSrgbColor, real numWatts);
	PointSource(const SampledSpectralStrength& color, real numWatts);
	virtual ~PointSource() override;

	virtual std::shared_ptr<Geometry> genAreas(CookingContext& context) const override;

private:
	SphereSource m_sphereSource;

// command interface
public:
	PointSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph