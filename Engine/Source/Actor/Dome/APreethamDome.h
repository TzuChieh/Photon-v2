#pragma once

#include "Actor/ADome.h"
#include "Common/primitive_type.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief Model the sky of the scene with an image.

Using Preetham model to generate absolute energy from sky.
*/
class APreethamDome : public ADome
{
public:
	APreethamDome();

	APreethamDome(
		real sunPhiDegrees,
		real sunThetaDegrees,
		real turbidity);

	APreethamDome(const APreethamDome& other) = default;

	std::shared_ptr<TTexture<math::Spectrum>> loadRadianceFunction(ActorCookingContext& ctx) override;
	math::Vector2S getResolution() const override;

	APreethamDome& operator = (APreethamDome rhs);

	friend void swap(APreethamDome& first, APreethamDome& second);

private:
	real m_sunPhi;
	real m_sunTheta;
	real m_turbidity;

	static void checkTurbidity(real turbidity);

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<APreethamDome>)
	{
		ClassType clazz("preetham-dome");
		clazz.docName("Preetham Dome Actor");
		clazz.description(
			"Using Preetham model to generate absolute energy from sky.");
		clazz.baseOn<ADome>();

		TSdlReal<OwnerType> sunPhi("yaw-pitch-row-degrees", &OwnerType::m_yawPitchRollDegrees);
		yawPitchRollDegrees.description(
			"Direction that this observer is looking at in yaw pitch form. "
			"yaw: Rotation around +y axis in [-180, 180]; "
			"pitch: Declination from the horizon in [-90, 90]; "
			"row: Rotation around +z axis in [-180, 180].");
		yawPitchRollDegrees.defaultTo({ 0, 0, 0 });
		yawPitchRollDegrees.optional();
		clazz.addField(yawPitchRollDegrees);

		return clazz;
	}
};

// In-header Implementations:

inline math::Vector2S APreethamDome::getResolution() const
{
	return {0, 0};
}

}// end namespace ph
