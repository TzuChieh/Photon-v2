#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/Utility/IsoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Utility/AnisoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxConductorDielectricFresnel.h"

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

void AbradedOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setSurfaceOptics(m_opticsGenerator());
}

// This mapping is what used in PBRT-v3. 
// (Strangely the original paper: Microfacet Models for Refraction through Rough Surfaces by Walter et al. does 
// not include such mapping for GGX distribution, only the ones for other kinds of distribution.)
real AbradedOpaque::roughnessToAlpha(const real roughness)
{
	const real clampedRoughness = std::max(roughness, 0.001_r);
	const real x = std::log(clampedRoughness);
	return 1.62142_r + 0.819955_r * x + 0.1734_r * x * x + 0.0171201_r * x * x * x + 0.000640711_r * x * x * x * x;
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
		material = ciLoadITR(packet);
	}
	else if(type == "aniso-metallic-ggx")
	{
		material = ciLoadATR(packet);
	}

	return material;
}

std::unique_ptr<AbradedOpaque> AbradedOpaque::ciLoadITR(const InputPacket& packet)
{
	Vector3R albedo    = Vector3R(0.5f, 0.5f, 0.5f);
	Vector3R f0        = Vector3R(0.04f, 0.04f, 0.04f);
	real     roughness = 0.5f;

	albedo    = packet.getVector3r("albedo", albedo);
	f0        = packet.getVector3r("f0", f0);
	roughness = packet.getReal("roughness", roughness);

	const real alpha = roughnessToAlpha(roughness);
	SpectralStrength f0Spectrum;
	f0Spectrum.setRgb(f0);

	std::unique_ptr<AbradedOpaque> material = std::make_unique<AbradedOpaque>();
	material->m_opticsGenerator = [=]()
	{
		auto optics = std::make_unique<OpaqueMicrofacet>();
		optics->setMicrofacet(std::make_shared<IsoTrowbridgeReitz>(alpha));
		optics->setFresnelEffect(std::make_shared<SchlickApproxConductorDielectricFresnel>(f0Spectrum));
		optics->setAlbedo(std::make_shared<ConstantTexture>(albedo));
		return optics;
	};
	return material;
}

std::unique_ptr<AbradedOpaque> AbradedOpaque::ciLoadATR(const InputPacket& packet)
{
	Vector3R albedo     = Vector3R(0.5f, 0.5f, 0.5f);
	Vector3R f0         = Vector3R(0.04f, 0.04f, 0.04f);
	real     roughnessU = 0.5f;
	real     roughnessV = 0.5f;

	albedo     = packet.getVector3r("albedo", albedo);
	f0         = packet.getVector3r("f0", f0);
	roughnessU = packet.getReal("roughness-u", roughnessU);
	roughnessV = packet.getReal("roughness-v", roughnessV);

	const real alphaU = roughnessToAlpha(roughnessU);
	const real alphaV = roughnessToAlpha(roughnessV);
	SpectralStrength f0Spectrum;
	f0Spectrum.setRgb(f0);

	std::unique_ptr<AbradedOpaque> material = std::make_unique<AbradedOpaque>();
	material->m_opticsGenerator = [=]()
	{
		auto optics = std::make_unique<OpaqueMicrofacet>();
		optics->setMicrofacet(std::make_shared<AnisoTrowbridgeReitz>(alphaU, alphaV));
		optics->setFresnelEffect(std::make_shared<SchlickApproxConductorDielectricFresnel>(f0Spectrum));
		optics->setAlbedo(std::make_shared<ConstantTexture>(albedo));
		return optics;
	};
	return material;
}

}// end namespace ph