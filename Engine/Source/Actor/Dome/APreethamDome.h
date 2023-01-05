#pragma once

#include "Actor/ADome.h"
#include "Common/primitive_type.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector2.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>
#include <optional>

namespace ph
{

/*! @brief Model the sky of the scene with an image.

Using Preetham model to generate absolute energy from sky.
*/
class APreethamDome : public ADome
{
public:
	std::shared_ptr<TTexture<math::Spectrum>> loadRadianceFunction(ActorCookingContext& ctx) override;
	math::Vector2S getResolution() const override;

private:
	math::Vector2R calcSunSphericalCoordinates() const;

	static void checkTurbidity(real turbidity);

	real m_turbidity;
	real m_standardTime24H;
	real m_standardMeridianDegrees;
	real m_siteLatitudeDegrees;
	real m_siteLongitudeDegrees;
	integer m_julianDate;

	std::optional<math::Vector2R> m_sunPhiThetaDegrees;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<APreethamDome>)
	{
		ClassType clazz("preetham-dome");
		clazz.docName("Preetham Dome Actor");
		clazz.description(
			"Using Preetham model to generate absolute energy from sky.");
		clazz.baseOn<ADome>();

		TSdlReal<OwnerType> turbidity("turbidity", &OwnerType::m_turbidity);
		turbidity.description("Turbidity of the atmosphere.");
		turbidity.defaultTo(3.0_r);
		turbidity.optional();
		clazz.addField(turbidity);

		TSdlReal<OwnerType> standardTime24H("standard-time-24h", &OwnerType::m_standardTime24H);
		standardTime24H.description("Standard time in 24H.");
		standardTime24H.defaultTo(12.0_r);
		standardTime24H.optional();
		clazz.addField(standardTime24H);

		TSdlReal<OwnerType> standardMeridianDegrees("standard-meridian-degrees", &OwnerType::m_standardMeridianDegrees);
		standardMeridianDegrees.description("Standard meridian in degrees.");
		standardMeridianDegrees.defaultTo(0.0_r);
		standardMeridianDegrees.optional();
		clazz.addField(standardMeridianDegrees);

		TSdlReal<OwnerType> siteLatitudeDegrees("site-latitude-degrees", &OwnerType::m_siteLatitudeDegrees);
		siteLatitudeDegrees.description(
			"Site latitude in [-90, 90] degrees (\"+\" implies N and \"-\" implies S).");
		siteLatitudeDegrees.defaultTo(0.0_r);
		siteLatitudeDegrees.optional();
		clazz.addField(siteLatitudeDegrees);

		TSdlReal<OwnerType> siteLongitudeDegrees("site-longitude-degrees", &OwnerType::m_siteLongitudeDegrees);
		siteLongitudeDegrees.description(
			"Site longitude in [0, 360] degrees (with prime meridian being 0)");
		siteLongitudeDegrees.defaultTo(0.0_r);
		siteLongitudeDegrees.optional();
		clazz.addField(siteLongitudeDegrees);

		TSdlInteger<OwnerType> julianDate("julian-date", &OwnerType::m_julianDate);
		julianDate.description("The day of the year as an integer in the range [1, 366].");
		julianDate.defaultTo(0.0_r);
		julianDate.optional();
		clazz.addField(julianDate);

		TSdlOptionalVector2R<OwnerType> sunPhiThetaDegrees("sun-phi-theta-degrees", &OwnerType::m_sunPhiThetaDegrees);
		sunPhiThetaDegrees.description(
			"Directly specify sun position in the sky in spherical coordinates. Note that this option "
			"may not be physically correct since not every position in the sky is possible for the sun "
			"given a location on Earth.");
		clazz.addField(sunPhiThetaDegrees);

		return clazz;
	}
};

// In-header Implementations:

inline math::Vector2S APreethamDome::getResolution() const
{
	return {0, 0};
}

}// end namespace ph
