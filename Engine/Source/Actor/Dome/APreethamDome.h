#pragma once

#include "Actor/ADome.h"
#include "Common/primitive_type.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>

namespace ph
{

/*! @brief Model the sky of the scene with an image.

Using Preetham model to generate absolute energy from sky.
*/
class APreethamDome : public ADome, public TCommandInterface<APreethamDome>
{
public:
	APreethamDome();

	APreethamDome(
		real sunPhiDegrees,
		real sunThetaDegrees,
		real turbidity);

	APreethamDome(const APreethamDome& other);

	std::shared_ptr<TTexture<Spectrum>> loadRadianceFunction(CookingContext& context) override;
	math::Vector2S getResolution() const override;

	APreethamDome& operator = (APreethamDome rhs);

	friend void swap(APreethamDome& first, APreethamDome& second);

private:
	real m_sunPhi;
	real m_sunTheta;
	real m_turbidity;

	static void checkTurbidity(real turbidity);

// command interface
public:
	explicit APreethamDome(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline math::Vector2S APreethamDome::getResolution() const
{
	return {0, 0};
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor          </category>
	<type_name> preetham-dome  </type_name>
	<extend>    actor.dome     </extend>

	<name> Preetham Dome Actor </name>
	<description>
		Using Preetham model to generate absolute energy from sky.
	</description>

	<command type="creator">
		<input name="turbidity" type="real">
			<description>
				Turbidity of the atmosphere.
			</description>
		</input>
		<input name="standard-time-24h" type="real">
			<description>
				Standard time in 24H.
			</description>
		</input>
		<input name="standard-meridian-degrees" type="real">
			<description>
				Standard meridian in degrees.
			</description>
		</input>
		<input name="site-latitude-decimal" type="real">
			<description>
				Site latitude in decimal.
			</description>
		</input>
		<input name="site-longitude-decimal" type="real">
			<description>
				Site longitude in decimal.
			</description>
		</input>
		<input name="julian-date" type="integer">
			<description>
				The day of the year as an integer in the range [1, 366].
			</description>
		</input>
	</command>

	</SDL_interface>
*/
