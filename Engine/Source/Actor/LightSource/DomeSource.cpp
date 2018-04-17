#include "Actor/LightSource/DomeSource.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/IdealSubstance.h"
#include "Actor/CookingContext.h"
#include "World/VisualWorldInfo.h"

namespace ph
{

const Logger DomeSource::logger(LogSender("Dome Source"));

DomeSource::DomeSource() : 
	LightSource()
{}

DomeSource::~DomeSource() = default;

std::unique_ptr<Emitter> DomeSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	// TODO
}

std::shared_ptr<Geometry> DomeSource::genGeometry(CookingContext& context) const
{
	real rootActorBoundRadius = 1000.0_r;
	if(context.getVisualWorldInfo())
	{
		const AABB3D&  bound   = context.getVisualWorldInfo()->getRootActorsBound();
		const Vector3R extends = bound.calcExtents();
		rootActorBoundRadius = extends.max() * 4.0_r;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"No visual world information available, cannot access root actor bounds."
			"Using " + std::to_string(rootActorBoundRadius) + " as dome radius.");
	}

	return std::make_shared<GSphere>(rootActorBoundRadius);
}

std::shared_ptr<Material> DomeSource::genMaterial(CookingContext& context) const
{
	auto material = std::make_shared<IdealSubstance>();
	
	// A dome should not have any visible inter-reflection ideally.
	//
	material->asAbsorber();

	return material;
}

// command interface

DomeSource::DomeSource(const InputPacket& packet) : 
	LightSource(packet)
{
	// TODO
}

SdlTypeInfo DomeSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "dome");
}

void DomeSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<DomeSource>(packet);
	}));
}

}// end namespace ph