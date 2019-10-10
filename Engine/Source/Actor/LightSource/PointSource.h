#pragma once

#include "Actor/LightSource/AreaSource.h"
#include "Actor/LightSource/SphereSource.h"

namespace ph
{

class PointSource : public AreaSource, public TCommandInterface<PointSource>
{
public:
	PointSource();
	PointSource(const math::Vector3R& linearSrgbColor, real numWatts);
	PointSource(const SampledSpectralStrength& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(CookingContext& context) const override;

private:
	SphereSource m_sphereSource;

// command interface
public:
	explicit PointSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source      </category>
	<type_name> point             </type_name>
	<extend>    light-source.area </extend>

	<name> Point Source </name>
	<description>
		Power emitting source from a small but not infinitesimal region. Resembling
		a small light bulb.
	</description>

	<command type="creator"/>

	</SDL_interface>
*/
