#include "Actor/Dome/APreethamDome.h"
#include "DataIO/SDL/InputPacket.h"
#include "Common/Logger.h"
#include "Core/Texture/Sky/PreethamTexture.h"
#include "Math/math.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("Actor Dome (Preetham)"));
}

APreethamDome::APreethamDome() :
	ADome()
{}

APreethamDome::APreethamDome(const APreethamDome& other) :
	ADome(other)
{}

std::shared_ptr<TTexture<Spectrum>> APreethamDome::loadRadianceFunction(CookingContext& context)
{
	return std::make_shared<PreethamTexture>(
		math::to_radians(180.0_r),
		math::to_radians(45.0_r),
		1.1_r);
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
	ADome(packet)
{}

}// end namespace ph
