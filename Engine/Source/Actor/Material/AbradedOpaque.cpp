#include "Actor/Material/AbradedOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/SDL/InputPacket.h"
#include "FileIO/SDL/InputPrototype.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Property/IsoBeckmann.h"
#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"
#include "Core/SurfaceBehavior/Property/ExactConductorFresnel.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"

#include <memory>
#include <algorithm>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	SurfaceMaterial(),

	m_opticsGenerator(),
	m_interfaceInfo  ()
{}

void AbradedOpaque::genSurface(CookingContext& context, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_opticsGenerator);

	behavior.setOptics(m_opticsGenerator());
}

// command interface

AbradedOpaque::AbradedOpaque(const InputPacket& packet) : 
	SurfaceMaterial(packet),

	m_opticsGenerator(),
	m_interfaceInfo  (packet)
{
	const std::string surfaceType = packet.getString("type", 
		"iso-metallic-ggx", DataTreatment::REQUIRED());
	if(surfaceType == "iso-metallic-ggx")
	{
		m_opticsGenerator = loadITR(packet);
	}
	else if(surfaceType == "aniso-metallic-ggx")
	{
		m_opticsGenerator = loadATR(packet);
	}
}

SdlTypeInfo AbradedOpaque::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-opaque");
}

void AbradedOpaque::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedOpaque>([](const InputPacket& packet)
	{
		return std::make_unique<AbradedOpaque>(packet);
	});
	cmdRegister.setLoader(loader);
}

std::function<std::unique_ptr<SurfaceOptics>()> AbradedOpaque::loadITR(const InputPacket& packet)
{
	Vector3R albedo      = Vector3R(0.5f, 0.5f, 0.5f);
	real     roughness   = 0.5f;
	bool     useBeckmann = false;

	albedo      = packet.getVector3("albedo", albedo);
	roughness   = packet.getReal("roughness", roughness);
	useBeckmann = packet.getString("use-beckmann", "false") == "true" ? true : false;

	//const real alpha = RoughnessToAlphaMapping::pbrtV3(roughness);
	const real alpha = RoughnessToAlphaMapping::squared(roughness);
	SpectralStrength albedoSpectrum;
	albedoSpectrum.setLinearSrgb(albedo, EQuantity::ECF);// FIXME: check color space

	//std::shared_ptr<FresnelEffect> fresnelEffect = loadFresnelEffect(packet);
	std::shared_ptr<FresnelEffect> fresnelEffect = ConductiveInterfaceInfo(packet).genFresnelEffect();

	return [=]()
	{
		std::unique_ptr<Microfacet> microfacet;
		if(!useBeckmann)
		{
			microfacet = std::make_unique<IsoTrowbridgeReitz>(alpha);
		}
		else
		{
			microfacet = std::make_unique<IsoBeckmann>(alpha);
		}

		auto optics = std::make_unique<OpaqueMicrofacet>(
			fresnelEffect, 
			std::move(microfacet));
		//optics->setAlbedo(std::make_shared<TConstantTexture<SpectralStrength>>(albedoSpectrum));
		return optics;
	};
}

std::function<std::unique_ptr<SurfaceOptics>()> AbradedOpaque::loadATR(const InputPacket& packet)
{
	Vector3R albedo     = Vector3R(0.5f, 0.5f, 0.5f);
	real     roughnessU = 0.5f;
	real     roughnessV = 0.5f;

	albedo     = packet.getVector3("albedo", albedo);
	roughnessU = packet.getReal("roughness-u", roughnessU);
	roughnessV = packet.getReal("roughness-v", roughnessV);

	const real alphaU = RoughnessToAlphaMapping::pbrtV3(roughnessU);
	const real alphaV = RoughnessToAlphaMapping::pbrtV3(roughnessV);
	SpectralStrength albedoSpectrum;
	albedoSpectrum.setLinearSrgb(albedo, EQuantity::ECF);// FIXME: check color space

	//std::shared_ptr<FresnelEffect> fresnelEffect = loadFresnelEffect(packet);
	std::shared_ptr<FresnelEffect> fresnelEffect = ConductiveInterfaceInfo(packet).genFresnelEffect();

	return [=]()
	{
		auto optics = std::make_unique<OpaqueMicrofacet>(
			fresnelEffect, 
			std::make_shared<AnisoTrowbridgeReitz>(alphaU, alphaV));
		//optics->setAlbedo(std::make_shared<TConstantTexture<SpectralStrength>>(albedoSpectrum));
		return optics;
	};
}

//std::unique_ptr<FresnelEffect> AbradedOpaque::loadFresnelEffect(const InputPacket& packet)
//{
//	std::unique_ptr<FresnelEffect> fresnelEffect;
//
//	// FIXME: f0's color space
//	InputPrototype approxInput;
//	approxInput.addVector3("f0");
//
//	InputPrototype exactInput;
//	exactInput.addReal("ior-outer");
//	exactInput.addRealArray("ior-inner-wavelength-nm");
//	exactInput.addRealArray("ior-inner-n");
//	exactInput.addRealArray("ior-inner-k");
//
//	if(packet.isPrototypeMatched(exactInput))
//	{
//		const auto& iorOuter             = packet.getReal("ior-outer");
//		const auto& iorInnerWavelengthNm = packet.getRealArray("ior-inner-wavelength-nm");
//		const auto& iorInnerN            = packet.getRealArray("ior-inner-n");
//		const auto& iorInnerK            = packet.getRealArray("ior-inner-k");
//
//		fresnelEffect = std::make_unique<ExactConductorFresnel>(
//			iorOuter,
//			iorInnerWavelengthNm, 
//			iorInnerN, 
//			iorInnerK);
//	}
//	else if(packet.isPrototypeMatched(approxInput))
//	{
//		const auto& f0 = packet.getVector3("f0");
//
//		SpectralStrength spectralF0;
//		spectralF0.setLinearSrgb(f0);// FIXME: check color space
//		fresnelEffect = std::make_unique<SchlickApproxConductorFresnel>(spectralF0);
//	}
//	else
//	{
//		std::cout << "NOTE: AbradedOpaque requires Fresnel effect parameter set "
//		          << "which are not found, using default" << std::endl;
//
//		const Vector3R defaultF0(0.04_r, 0.04_r, 0.04_r);
//
//		SpectralStrength spectralF0;// FIXME: check color space
//		spectralF0.setLinearSrgb(defaultF0);
//		fresnelEffect = std::make_unique<SchlickApproxConductorFresnel>(spectralF0);
//	}
//	
//	return fresnelEffect;
//}

}// end namespace ph
