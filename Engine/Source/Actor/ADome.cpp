#include "Actor/ADome.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/CookingContext.h"
#include "World/VisualWorldInfo.h"
#include "Actor/ALight.h"
#include "Actor/LightSource/DomeSource.h"
#include "FileIO/SDL/InputPacket.h"

namespace ph
{

const Logger ADome::logger(LogSender("Actor Dome"));

ADome::ADome() : 
	ADome(Path())
{}

ADome::ADome(const Path& sphericalEnvMap) :
	PhysicalActor(),
	m_sphericalEnvMap(sphericalEnvMap)
{}

ADome::ADome(const ADome& other) : 
	PhysicalActor(other),
	m_sphericalEnvMap(other.m_sphericalEnvMap)
{}

CookedUnit ADome::cook(CookingContext& context) const
{
	auto lightSource = std::make_shared<DomeSource>(m_sphericalEnvMap);

	auto lightActor = std::make_unique<ALight>();
	lightActor->setBaseTransform(m_localToWorld);
	lightActor->setLightSource(lightSource);
	context.addChildActor(std::move(lightActor));

	return CookedUnit();
}

ADome& ADome::operator = (ADome rhs)
{
	swap(*this, rhs);

	return *this;
}

void swap(ADome& first, ADome& second)
{
	// enable ADL
	using std::swap;

	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_sphericalEnvMap,            second.m_sphericalEnvMap);
}

// command interface

ADome::ADome(const InputPacket& packet) : 
	PhysicalActor(packet),
	m_sphericalEnvMap()
{
	m_sphericalEnvMap = packet.getStringAsPath("env-map", Path(), DataTreatment::REQUIRED());
}

SdlTypeInfo ADome::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "dome");
}

void ADome::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<ADome>(packet);
	}));

	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<ADome>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<ADome>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<ADome>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

}// end namespace ph