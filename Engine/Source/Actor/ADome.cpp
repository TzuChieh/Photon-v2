#include "Actor/ADome.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/CookingContext.h"
#include "World/VisualWorldInfo.h"
#include "Actor/ALight.h"
#include "Actor/LightSource/DomeSource.h"
#include "FileIO/InputPacket.h"

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

ADome::~ADome() = default;

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

SdlTypeInfo ADome::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "dome");
}

void ADome::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ADome>(ciLoad);
	cmdRegister.setLoader(loader);

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

std::unique_ptr<ADome> ADome::ciLoad(const InputPacket& packet)
{
	const Path& envMapPath = packet.getStringAsPath("env-map", Path(), DataTreatment::REQUIRED());

	return std::make_unique<ADome>(envMapPath);
}

}// end namespace ph