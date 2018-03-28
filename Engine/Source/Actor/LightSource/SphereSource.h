#pragma once

#include "Actor/LightSource/AreaSource.h"

namespace ph
{

class SphereSource final : public AreaSource, public TCommandInterface<SphereSource>
{
public:
	SphereSource();
	SphereSource(real radius, const Vector3R& linearSrgbColor, real numWatts);
	SphereSource(real radius, const SampledSpectralStrength& color, real numWatts);
	virtual ~SphereSource() override;

	virtual std::vector<std::unique_ptr<Geometry>> genAreas() const override;

	void setRadius(real radius);

private:
	real m_radius;

// command interface
public:
	SphereSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph