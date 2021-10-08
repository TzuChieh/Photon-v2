#include "Actor/Dome/APreethamDome.h"
#include "Common/logging.h"
#include "Core/Texture/Sky/PreethamTexture.h"
#include "Math/math.h"
#include "Math/time.h"
#include "Math/Geometry/location.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PreethamDomeActor, Actor);

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

std::shared_ptr<TTexture<math::Spectrum>> APreethamDome::loadRadianceFunction(ActorCookingContext& ctx)
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
		PH_LOG(PreethamDomeActor,
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

	swap(first.m_sunPhi,    second.m_sunPhi);
	swap(first.m_sunTheta,  second.m_sunTheta);
	swap(first.m_turbidity, second.m_turbidity);
}

}// end namespace ph
