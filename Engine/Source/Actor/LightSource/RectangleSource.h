#pragma once

#include "Actor/LightSource/AreaSource.h"

namespace ph
{

class RectangleSource final : public AreaSource, public TCommandInterface<RectangleSource>
{
public:
	RectangleSource();
	RectangleSource(real width, real height, const Vector3R& linearSrgbColor, real numWatts);
	RectangleSource(real width, real height, const SampledSpectralStrength& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(CookingContext& context) const override;

	void setDimension(real width, real height);

private:
	real m_width;
	real m_height;

// command interface
public:
	explicit RectangleSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source      </category>
	<type_name> rectangle         </type_name>
	<extend>    light-source.area </extend>

	<name> Rectangle Source </name>
	<description>
		This type of light emits energy from a rectangular shape. Note that energy
		is only allowed to emit from one side of the rectangle, not both sides.
	</description>

	<command type="creator">
		<input name="width" type="real">
			<description>The width of the rectangle.</description>
		</input>
		<input name="height" type="real">
			<description>The height of the rectangle.</description>
		</input>
	</command>

	</SDL_interface>
*/