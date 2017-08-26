#include "Actor/Material/IdealSubstance.h"
#include "Core/SurfaceBehavior/BSDF/IdealReflector.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "FileIO/InputPrototype.h"
#include "FileIO/InputPacket.h"

#include <string>

namespace ph
{

IdealSubstance::IdealSubstance() : 
	Material()
{
	setDielectricReflector(1.0_r, 1.5_r);
}

IdealSubstance::~IdealSubstance() = default;

void IdealSubstance::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	switch(m_type)
	{
	case Type::DIELECTRIC_REFLECTOR:
		auto bsdf    = std::make_unique<IdealReflector>();
		auto fresnel = std::make_shared<ExactDielectricFresnel>(m_monoIorOuter, m_monoIorInner);
		bsdf->setFresnelEffect(fresnel);
		out_surfaceBehavior->setBsdf(std::move(bsdf));
		break;
	}
}

void IdealSubstance::setDielectricReflector(const real iorOuter, const real iorInner)
{
	m_type = Type::DIELECTRIC_REFLECTOR;

	m_monoIorOuter = iorOuter;
	m_monoIorInner = iorInner;
}

// command interface

SdlTypeInfo IdealSubstance::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "ideal-substance");
}

std::unique_ptr<IdealSubstance> IdealSubstance::ciLoad(const InputPacket& packet)
{
	auto material = std::make_unique<IdealSubstance>();

	const std::string type = packet.getString("type", 
	                                          "dielectric-reflector", 
	                                          DataTreatment::REQUIRED());
	if(type == "dielectric-reflector")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const real iorInner = packet.getReal("ior-inner", 1.0_r, DataTreatment::REQUIRED());
		material->setDielectricReflector(iorOuter, iorInner);
	}

	return material;
}

ExitStatus IdealSubstance::ciExecute(const std::shared_ptr<IdealSubstance>& targetResource, 
                                     const std::string& functionName, 
                                     const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph