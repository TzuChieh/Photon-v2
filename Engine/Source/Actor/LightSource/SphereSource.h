#pragma once

#include "Actor/LightSource/AreaSource.h"

namespace ph
{

class SphereSource final : public AreaSource, public TCommandInterface<SphereSource>
{
public:
	SphereSource();
	SphereSource(real radius, const math::Vector3R& linearSrgbColor, real numWatts);
	SphereSource(real radius, const SampledSpectralStrength& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(CookingContext& context) const override;

	void setRadius(real radius);

private:
	real m_radius;

// command interface
public:
	explicit SphereSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source      </category>
	<type_name> sphere            </type_name>
	<extend>    light-source.area </extend>

	<name> Sphere Source </name>
	<description>
		This type of light emits energy from a spherical shape.
	</description>

	<command type="creator">
		<input name="radius" type="real">
			<description>The radius of the sphere.</description>
		</input>
	</command>

	</SDL_interface>
*/
