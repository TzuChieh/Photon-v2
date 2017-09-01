#include "Actor/Material/IdealSubstance.h"
#include "Core/SurfaceBehavior/BSDF/IdealReflector.h"
#include "Core/SurfaceBehavior/BSDF/IdealTransmitter.h"
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
	Material(),
	m_bsdfGenerator(nullptr)
{
	m_bsdfGenerator = []()
	{
		std::cerr << "warning: at IdealSubstance::populateSurfaceBehavior(), "
		          << "no BSDF specified, using default one" << std::endl;

		auto bsdf    = std::make_unique<IdealReflector>();
		auto fresnel = std::make_shared<SchlickApproxConductorDielectricFresnel>(SpectralStrength(0.0_r));
		bsdf->setFresnelEffect(fresnel);

		return bsdf;
	};
}

IdealSubstance::~IdealSubstance() = default;

void IdealSubstance::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdf(m_bsdfGenerator());
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

		material->m_bsdfGenerator = [=]()
		{
			auto bsdf    = std::make_unique<IdealReflector>();
			auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
			bsdf->setFresnelEffect(fresnel);

			return bsdf;
		};
	}
	else if(type == "metallic-reflector")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const Vector3R f0Rgb = packet.getVector3r("f0-rgb", Vector3R(1), DataTreatment::REQUIRED());

		SpectralStrength f0Spectral;
		f0Spectral.setRgb(f0Rgb);

		material->m_bsdfGenerator = [=]()
		{
			auto bsdf    = std::make_unique<IdealReflector>();
			auto fresnel = std::make_shared<SchlickApproxConductorDielectricFresnel>(f0Spectral);
			bsdf->setFresnelEffect(fresnel);

			return bsdf;
		};
	}
	else if(type == "transmitter")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const real iorInner = packet.getReal("ior-inner", 1.0_r, DataTreatment::REQUIRED());

		material->m_bsdfGenerator = [=]()
		{
			auto bsdf    = std::make_unique<IdealTransmitter>();
			auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
			bsdf->setFresnelEffect(fresnel);

			return bsdf;
		};
	}

	return material;
}

}// end namespace ph