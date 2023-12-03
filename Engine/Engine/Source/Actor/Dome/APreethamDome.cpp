#include "Actor/Dome/APreethamDome.h"
#include "Common/logging.h"
#include "Core/Texture/Sky/PreethamTexture.h"
#include "Math/math.h"
#include "Math/time.h"
#include "Math/Geometry/location.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PreethamDomeActor, Actor);

std::shared_ptr<TTexture<math::Spectrum>> APreethamDome::loadRadianceFunction(
	const CookingContext& ctx, DomeRadianceFunctionInfo* const out_info) const
{
	checkTurbidity(m_turbidity);

	if(out_info)
	{
		out_info->isAnalytical = true;
		out_info->resolution = {0, 0};
	}

	const math::Vector2R sunPhiTheta = calcSunSphericalCoordinates();
	return std::make_shared<PreethamTexture>(
		sunPhiTheta.x(),
		sunPhiTheta.y(),
		m_turbidity);
}

math::Vector2R APreethamDome::calcSunSphericalCoordinates() const
{
	math::Vector2R sunPhiTheta(0, 0);
	if(m_sunPhiThetaDegrees.has_value())
	{
		PH_LOG(PreethamDomeActor,
			"Using direct specification of sun position [{} deg, {} deg] (spherical coordinates) "
			"in the sky. Note that this may not be physically correct since not every position "
			"in the sky is possible for the sun given a location on Earth.",
			m_sunPhiThetaDegrees->x(), m_sunPhiThetaDegrees->y());

		sunPhiTheta.x() = math::to_radians(m_sunPhiThetaDegrees->x());
		sunPhiTheta.y() = math::to_radians(m_sunPhiThetaDegrees->y());
	}
	else
	{
		const auto siteLatitude = math::to_radians(m_siteLatitudeDegrees);
		const auto siteLongitude = math::to_radians(m_siteLongitudeDegrees);
		const auto standardMeridian = math::to_radians(m_standardMeridianDegrees);

		const auto solarTime24H = math::mean_solar_time_to_solar_time_24H(
			m_standardTime24H,
			standardMeridian,
			siteLongitude,
			static_cast<real>(m_julianDate));

		sunPhiTheta = math::sun_sky_phi_theta(
			solarTime24H,
			static_cast<real>(m_julianDate),
			siteLatitude);
	}

	return sunPhiTheta;
}

void APreethamDome::checkTurbidity(const real turbidity)
{
	if(turbidity < 2 || turbidity > 10)
	{
		PH_LOG(PreethamDomeActor,
			"turbidity values not in [2, 10] may cause rendering artifacts as "
			"the fitting might break (this is the range being tested in the paper)");
	}
}

}// end namespace ph
