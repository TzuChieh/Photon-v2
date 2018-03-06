#include "Actor/Material/AbradedOpaque.h"
#include "Core/Texture/TConstantTexture.h"
#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorDielectricFresnel.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"
#include "Core/SurfaceBehavior/Property/ExactConductorDielectricFresnel.h"

#include <memory>
#include <algorithm>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	Material(),
	m_opticsGenerator(nullptr)
{
	m_opticsGenerator = []()
	{
		std::cerr << "warning: at AbradedOpaque::populateSurfaceBehavior(), "
		          << "no BSDF specified, using default one" << std::endl;
		return nullptr;
	};
}

AbradedOpaque::~AbradedOpaque() = default;

void AbradedOpaque::populateSurfaceBehavior(CookingContext& context, SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setSurfaceOptics(m_opticsGenerator());
}

// command interface

SdlTypeInfo AbradedOpaque::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-opaque");
}

void AbradedOpaque::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedOpaque>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<AbradedOpaque> AbradedOpaque::ciLoad(const InputPacket& packet)
{
	std::unique_ptr<AbradedOpaque> material;
	const std::string type = packet.getString("type",
	                                          "iso-metallic-ggx",
	                                          DataTreatment::REQUIRED());
	if(type == "iso-metallic-ggx")
	{
		material = loadITR(packet);
	}
	else if(type == "aniso-metallic-ggx")
	{
		material = loadATR(packet);
	}

	return material;
}

std::unique_ptr<AbradedOpaque> AbradedOpaque::loadITR(const InputPacket& packet)
{
	Vector3R albedo    = Vector3R(0.5f, 0.5f, 0.5f);
	real     roughness = 0.5f;

	albedo    = packet.getVector3r("albedo", albedo);
	roughness = packet.getReal("roughness", roughness);

	const real alpha = RoughnessToAlphaMapping::pbrtV3(roughness);
	SpectralStrength albedoSpectrum;
	albedoSpectrum.setLinearSrgb(albedo, EQuantity::ECF);// FIXME: check color space

	std::shared_ptr<FresnelEffect> fresnelEffect = loadFresnelEffect(packet);

	std::unique_ptr<AbradedOpaque> material = std::make_unique<AbradedOpaque>();
	material->m_opticsGenerator = [=]()
	{
		auto optics = std::make_unique<OpaqueMicrofacet>();
		optics->setMicrofacet(std::make_shared<IsoTrowbridgeReitz>(alpha));
		optics->setFresnelEffect(fresnelEffect);
		optics->setAlbedo(std::make_shared<TConstantTexture<SpectralStrength>>(albedoSpectrum));
		return optics;
	};
	return material;
}

std::unique_ptr<AbradedOpaque> AbradedOpaque::loadATR(const InputPacket& packet)
{
	Vector3R albedo     = Vector3R(0.5f, 0.5f, 0.5f);
	real     roughnessU = 0.5f;
	real     roughnessV = 0.5f;

	albedo     = packet.getVector3r("albedo", albedo);
	roughnessU = packet.getReal("roughness-u", roughnessU);
	roughnessV = packet.getReal("roughness-v", roughnessV);

	const real alphaU = RoughnessToAlphaMapping::pbrtV3(roughnessU);
	const real alphaV = RoughnessToAlphaMapping::pbrtV3(roughnessV);
	SpectralStrength albedoSpectrum;
	albedoSpectrum.setLinearSrgb(albedo, EQuantity::ECF);// FIXME: check color space

	std::shared_ptr<FresnelEffect> fresnelEffect = loadFresnelEffect(packet);

	std::unique_ptr<AbradedOpaque> material = std::make_unique<AbradedOpaque>();
	material->m_opticsGenerator = [=]()
	{
		auto optics = std::make_unique<OpaqueMicrofacet>();
		optics->setMicrofacet(std::make_shared<AnisoTrowbridgeReitz>(alphaU, alphaV));
		optics->setFresnelEffect(fresnelEffect);
		optics->setAlbedo(std::make_shared<TConstantTexture<SpectralStrength>>(albedoSpectrum));
		return optics;
	};
	return material;
}

std::unique_ptr<FresnelEffect> AbradedOpaque::loadFresnelEffect(const InputPacket& packet)
{
	std::unique_ptr<FresnelEffect> fresnelEffect;

	// FIXME: f0's color space
	InputPrototype approxInput;
	approxInput.addVector3r("f0");

	InputPrototype exactInput;
	exactInput.addReal("ior-outer");
	exactInput.addRealArray("ior-inner-wavelength-nm");
	exactInput.addRealArray("ior-inner-n");
	exactInput.addRealArray("ior-inner-k");

	if(packet.isPrototypeMatched(exactInput))
	{
		const auto& iorOuter             = packet.getReal("ior-outer");
		const auto& iorInnerWavelengthNm = packet.getRealArray("ior-inner-wavelength-nm");
		const auto& iorInnerN            = packet.getRealArray("ior-inner-n");
		const auto& iorInnerK            = packet.getRealArray("ior-inner-k");

		fresnelEffect = std::make_unique<ExactConductorDielectricFresnel>(
			iorOuter,
			iorInnerWavelengthNm, 
			iorInnerN, 
			iorInnerK);
	}
	else if(packet.isPrototypeMatched(approxInput))
	{
		const auto& f0 = packet.getVector3r("f0");

		SpectralStrength spectralF0;
		spectralF0.setLinearSrgb(f0);// FIXME: check color space
		fresnelEffect = std::make_unique<SchlickApproxConductorDielectricFresnel>(spectralF0);
	}
	else
	{
		std::cout << "NOTE: AbradedOpaque requires Fresnel effect parameter set "
		          << "which are not found, using default" << std::endl;

		const Vector3R defaultF0(0.04_r, 0.04_r, 0.04_r);

		SpectralStrength spectralF0;// FIXME: check color space
		spectralF0.setLinearSrgb(defaultF0);
		fresnelEffect = std::make_unique<SchlickApproxConductorDielectricFresnel>(spectralF0);
	}
	
	return fresnelEffect;
}

}// end namespace ph