#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Actor/Geometry/Geometry.h"

#include <memory>

namespace ph
{

class Image;

class AreaSource : public LightSource, public TCommandInterface<AreaSource>
{
public:
	AreaSource();
	AreaSource(const Vector3R& linearSrgbColor, real numWatts);
	AreaSource(const SampledSpectralStrength& color, real numWatts);
	virtual ~AreaSource() override;

	virtual std::shared_ptr<Geometry> genAreas(CookingContext& context) const = 0;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

	virtual std::shared_ptr<Geometry> genGeometry(CookingContext& context) const final override;

private:
	SampledSpectralStrength m_color;
	real                    m_numWatts;

// command interface
public:
	AreaSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source              </category>
	<type_name> area                      </type_name>
	<extend>    light-source.light-source </extend>

	<name> Area Source </name>
	<description>
		This type of light source has a finite area. Energy is allowed to emit
		as long as the emitting source is within the area.
	</description>

	<command type="creator" intent="blueprint">
		<input name="linear-srgb" type="vector3">
			<description>The color of this light source in linear-SRGB.</description>
		</input>
		<input name="watts" type="real">
			<description>Energy emitted by this light source.</description>
		</input>
	</command>

	</SDL_interface>
*/