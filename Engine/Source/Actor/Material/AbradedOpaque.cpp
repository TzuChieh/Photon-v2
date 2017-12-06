#include "Actor/Material/AbradedOpaque.h"
#include "Core/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/Utility/IsoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Utility/AnisoTrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxConductorDielectricFresnel.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"

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

	const real alpha = RoughnessToAlphaMapping::pbrtV3(roughness);
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

	const real alphaU = RoughnessToAlphaMapping::pbrtV3(roughnessU);
	const real alphaV = RoughnessToAlphaMapping::pbrtV3(roughnessV);
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