#include "Actor/Material/IdealSubstance.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorDielectricFresnel.h"
#include "FileIO/InputPrototype.h"
#include "FileIO/InputPacket.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"

#include <string>
#include <iostream>

namespace ph
{

IdealSubstance::IdealSubstance() : 
	Material(),
	m_opticsGenerator(nullptr)
{
	asAbsorber();
}

IdealSubstance::~IdealSubstance() = default;

void IdealSubstance::populateSurfaceBehavior(CookingContext& context, SurfaceBehavior* const out_surfaceBehavior) const
{
	PH_ASSERT(m_opticsGenerator);

	out_surfaceBehavior->setOptics(m_opticsGenerator());
}

void IdealSubstance::asDielectricReflector(const real iorInner, const real iorOuter)
{
	m_opticsGenerator = [=]()
	{
		auto optics  = std::make_unique<IdealReflector>();
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
		optics->setFresnelEffect(fresnel);

		return optics;
	};
}

void IdealSubstance::asMetallicReflector(const Vector3R& linearSrgbF0, const real iorOuter)
{
	SpectralStrength f0Spectral;
	f0Spectral.setLinearSrgb(linearSrgbF0);// FIXME: check color space

	m_opticsGenerator = [=]()
	{
		auto optics  = std::make_unique<IdealReflector>();
		auto fresnel = std::make_shared<SchlickApproxConductorDielectricFresnel>(f0Spectral);
		optics->setFresnelEffect(fresnel);

		return optics;
	};
}

void IdealSubstance::asTransmitter(const real iorInner, const real iorOuter)
{
	m_opticsGenerator = [=]()
	{
		auto optics  = std::make_unique<IdealTransmitter>();
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
		optics->setFresnelEffect(fresnel);

		return optics;
	};
}

void IdealSubstance::asAbsorber()
{
	m_opticsGenerator = [=]()
	{
		return std::make_unique<IdealAbsorber>();
	};
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
	                                          "absorber", 
	                                          DataTreatment::REQUIRED());
	if(type == "dielectric-reflector")
	{
		const real iorInner = packet.getReal("ior-inner", 1.0_r, DataTreatment::REQUIRED());
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());

		material->asDielectricReflector(iorInner, iorOuter);
	}
	else if(type == "metallic-reflector")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());

		// FIXME: check color space
		const Vector3R f0Rgb = packet.getVector3r("f0-rgb", Vector3R(1), DataTreatment::REQUIRED());

		material->asMetallicReflector(f0Rgb, iorOuter);
	}
	else if(type == "transmitter")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const real iorInner = packet.getReal("ior-inner", 1.0_r, DataTreatment::REQUIRED());

		material->asTransmitter(iorInner, iorOuter);
	}
	else if(type == "absorber")
	{
		material->asAbsorber();
	}

	return material;
}

}// end namespace ph