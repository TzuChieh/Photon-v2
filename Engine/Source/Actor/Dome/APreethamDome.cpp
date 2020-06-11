#include "Actor/Dome/APreethamDome.h"
#include "DataIO/SDL/InputPacket.h"
#include "Common/Logger.h"
#include "Core/Texture/Sky/PreethamTexture.h"
#include "Math/math.h"
#include "Math/time.h"
#include "Math/Geometry/location.h"

namespace ph
{

namespace
{
	Logger logger(LogSender("Actor Dome (Preetham)"));
}

APreethamDome::APreethamDome() :
	APreethamDome(
		0.0_r,
		45.0_r,
		3.0_r)
{}

APreethamDome::APreethamDome(
	const real sunPhiDegrees,
	const real sunThetaDegrees,
	const real turbidity) :

	ADome(),

	m_sunPhi   (math::to_radians(sunPhiDegrees)),
	m_sunTheta (math::to_radians(sunThetaDegrees)),
	m_turbidity(turbidity)
{}

APreethamDome::APreethamDome(const APreethamDome& other) :
	ADome(other)
{}

std::shared_ptr<TTexture<Spectrum>> APreethamDome::loadRadianceFunction(CookingContext& context)
{
	checkTurbidity(m_turbidity);

	return std::make_shared<PreethamTexture>(
		m_sunPhi,
		m_sunTheta,
		m_turbidity);
}

void APreethamDome::checkTurbidity(const real turbidity)
{
	if(turbidity < 2 || turbidity > 10)
	{
		logger.log(
			"turbidity values not in [2, 10] may cause rendering artifacts as "
			"the fitting might break (this is the range being tested in the paper)");
	}
}

APreethamDome& APreethamDome::operator = (APreethamDome rhs)
{
	swap(*this, rhs);

	return *this;
}

void swap(APreethamDome& first, APreethamDome& second)
{
	// Enable ADL
	using std::swap;

	swap(static_cast<ADome&>(first), static_cast<ADome&>(second));
}

// command interface

APreethamDome::APreethamDome(const InputPacket& packet) :

	ADome(packet),

	m_sunPhi   (math::to_radians(0.0_r)),
	m_sunTheta (math::to_radians(45.0_r)),
	m_turbidity(3.0_r)
{
	m_turbidity = packet.getReal("turbidity", m_turbidity);

	if(packet.hasReal("sun-phi-degrees") && packet.hasReal("sun-theta-degrees"))
	{
		m_sunPhi   = math::to_radians(packet.getReal("sun-phi-degrees"));
		m_sunTheta = math::to_radians(packet.getReal("sun-theta-degrees"));
	}
	else
	{
		const auto standardTime24H = packet.getReal("standard-time-24h", 
			12, DataTreatment::REQUIRED());

		const auto standardMeridianDegrees = packet.getReal("standard-meridian-degrees", 
			0, DataTreatment::REQUIRED());

		const auto siteLatitudeDegrees = packet.getReal("site-latitude-decimal",
			0, DataTreatment::REQUIRED());

		const auto siteLongitudeDegrees = packet.getReal("site-longitude-decimal",
			0, DataTreatment::REQUIRED());

		const auto julianDate = packet.getInteger("julian-date",
			1, DataTreatment::REQUIRED());

		const auto siteLatitude     = math::to_radians(siteLatitudeDegrees);
		const auto siteLongitude    = math::to_radians(siteLongitudeDegrees);
		const auto standardMeridian = math::to_radians(standardMeridianDegrees);

		const auto solarTime24H = math::mean_solar_time_to_solar_time_24H(
			standardTime24H,
			standardMeridian,
			siteLongitude,
			static_cast<real>(julianDate));

		const auto phiTheta = math::sun_sky_phi_theta(
			solarTime24H,
			static_cast<real>(julianDate),
			siteLatitude);

		m_sunPhi   = phiTheta.x;
		m_sunTheta = phiTheta.y;

		logger.log(
			"standard time = " + std::to_string(standardTime24H) + "H with "
			"standard meridian = " + std::to_string(standardMeridianDegrees) + "d "
			"on julian date = " + std::to_string(julianDate) + ", at "
			"location (" + std::to_string(siteLatitudeDegrees) + "d, " + 
			std::to_string(siteLongitudeDegrees) + "d) has local sun phi-theta = " + 
			phiTheta.toString());
	}
}

}// end namespace ph
