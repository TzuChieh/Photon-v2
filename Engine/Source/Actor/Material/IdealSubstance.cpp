#include "Actor/Material/IdealSubstance.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorDielectricFresnel.h"
#include "FileIO/SDL/InputPrototype.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealDielectric.h"
#include "Actor/Image/ConstantImage.h"

#include <string>
#include <iostream>

namespace ph
{

IdealSubstance::IdealSubstance() : 
	SurfaceMaterial(),
	m_opticsGenerator()
{
	asAbsorber();
}

IdealSubstance::~IdealSubstance() = default;

void IdealSubstance::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_opticsGenerator);

	behavior.setOptics(m_opticsGenerator(context));
}

void IdealSubstance::asDielectricReflector(const real iorInner, const real iorOuter)
{
	m_opticsGenerator = [=](CookingContext& context)
	{
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
		auto optics  = std::make_unique<IdealReflector>(fresnel);
		return optics;
	};
}

void IdealSubstance::asMetallicReflector(const Vector3R& linearSrgbF0, const real iorOuter)
{
	SpectralStrength f0Spectral;
	f0Spectral.setLinearSrgb(linearSrgbF0);// FIXME: check color space

	m_opticsGenerator = [=](CookingContext& context)
	{
		auto fresnel = std::make_shared<SchlickApproxConductorDielectricFresnel>(f0Spectral);
		auto optics  = std::make_unique<IdealReflector>(fresnel);
		return optics;
	};
}

void IdealSubstance::asTransmitter(const real iorInner, const real iorOuter)
{
	m_opticsGenerator = [=](CookingContext& context)
	{
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
		auto optics  = std::make_unique<IdealTransmitter>(fresnel);
		return optics;
	};
}

void IdealSubstance::asAbsorber()
{
	m_opticsGenerator = [=](CookingContext& context)
	{
		return std::make_unique<IdealAbsorber>();
	};
}

void IdealSubstance::asDielectric(
	const real iorInner,
	const real iorOuter,
	const Vector3R& linearSrgbReflectionScale,
	const Vector3R& linearSrgbTransmissionScale)
{
	m_opticsGenerator = [=](CookingContext& context)
	{
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);

		if(linearSrgbReflectionScale == Vector3R(1.0_r) && linearSrgbTransmissionScale == Vector3R(1.0_r))
		{
			return std::make_unique<IdealDielectric>(fresnel);
		}
		else
		{
			auto reflectionScale = ConstantImage(linearSrgbReflectionScale, ConstantImage::EType::RAW_LINEAR_SRGB);
			auto transmissionScale = ConstantImage(linearSrgbTransmissionScale, ConstantImage::EType::RAW_LINEAR_SRGB);

			return std::make_unique<IdealDielectric>(
				fresnel, 
				reflectionScale.genTextureSpectral(context), 
				transmissionScale.genTextureSpectral(context));
		}
	};
}

// command interface

IdealSubstance::IdealSubstance(const InputPacket& packet) : 
	SurfaceMaterial(packet),
	m_opticsGenerator()
{
	const std::string type = packet.getString("type", 
		"absorber", DataTreatment::REQUIRED());
	if(type == "dielectric-reflector")
	{
		const real iorInner = packet.getReal("ior-inner", 1.0_r, DataTreatment::REQUIRED());
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());

		asDielectricReflector(iorInner, iorOuter);
	}
	else if(type == "metallic-reflector")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());

		// FIXME: check color space
		const Vector3R f0Rgb = packet.getVector3r("f0-rgb", Vector3R(1), DataTreatment::REQUIRED());

		asMetallicReflector(f0Rgb, iorOuter);
	}
	else if(type == "transmitter")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const real iorInner = packet.getReal("ior-inner", 1.0_r, DataTreatment::REQUIRED());

		asTransmitter(iorInner, iorOuter);
	}
	else if(type == "absorber")
	{
		asAbsorber();
	}
	else if(type == "dielectric")
	{
		const real iorOuter = packet.getReal("ior-outer", 1.0_r, DataTreatment::OPTIONAL());
		const real iorInner = packet.getReal("ior-inner", 1.5_r, DataTreatment::REQUIRED());
		const Vector3R reflectionScale = packet.getVector3r("reflection-scale", Vector3R(1.0_r), DataTreatment::OPTIONAL());
		const Vector3R transmissionScale = packet.getVector3r("transmission-scale", Vector3R(1.0_r), DataTreatment::OPTIONAL());

		asDielectric(iorInner, iorOuter, reflectionScale, transmissionScale);
	}
}

SdlTypeInfo IdealSubstance::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "ideal-substance");
}

void IdealSubstance::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<IdealSubstance>([](const InputPacket& packet)
	{
		return std::make_unique<IdealSubstance>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph