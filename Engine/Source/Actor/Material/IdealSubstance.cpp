#include "Actor/Material/IdealSubstance.h"
#include "Core/SurfaceBehavior/BSDF/IdealReflector.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxConductorDielectricFresnel.h"
#include "FileIO/InputPrototype.h"
#include "FileIO/InputPacket.h"
#include "Math/TVector3.h"

#include <string>
#include <iostream>

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
	{
		auto bsdf = std::make_unique<IdealReflector>();
		auto fresnel = std::make_shared<ExactDielectricFresnel>(m_monoIorOuter, m_monoIorInner);
		bsdf->setFresnelEffect(fresnel);
		out_surfaceBehavior->setBsdf(std::move(bsdf));
		break;
	}

	case Type::METALLIC_REFLECTOR:
	{
		auto bsdf = std::make_unique<IdealReflector>();
		auto fresnel = std::make_shared<SchlickApproxConductorDielectricFresnel>(m_f0);
		bsdf->setFresnelEffect(fresnel);
		out_surfaceBehavior->setBsdf(std::move(bsdf));
		break;
	}

	default:
		std::cerr << "warning: at IdealSubstance::populateSurfaceBehavior(), " 
		          << "unsupported type received" << std::endl;
		break;
	}
}

void IdealSubstance::setDielectricReflector(const real iorOuter, const real iorInner)
{
	m_type = Type::DIELECTRIC_REFLECTOR;

	m_monoIorOuter = iorOuter;
	m_monoIorInner = iorInner;
}

void IdealSubstance::setMetallicReflector(const real iorOuter, const SpectralStrength& f0)
{
	m_type = Type::METALLIC_REFLECTOR;

	m_monoIorOuter = iorOuter;
	m_f0           = f0;
}

// command interface

SdlTypeInfo IdealSubstance::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "ideal-substance");
}

void IdealSubstance::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<IdealSubstance>(ciLoad);
	cmdRegister.setLoader(loader);
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
	else if(type == "metallic-reflector")
	{
		const real     iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const Vector3R f0Rgb    = packet.getVector3r("f0-rgb", Vector3R(1), DataTreatment::REQUIRED());

		SpectralStrength f0Spectral;
		f0Spectral.setRgb(f0Rgb);

		material->setMetallicReflector(iorOuter, f0Spectral);
	}

	return material;
}

}// end namespace ph