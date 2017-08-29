#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/Utility/TrowbridgeReitz.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxConductorDielectricFresnel.h"

#include <memory>
#include <algorithm>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	Material(),
	m_bsdf()
{
	
}

AbradedOpaque::~AbradedOpaque() = default;

void AbradedOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdf(std::make_unique<OpaqueMicrofacet>(m_bsdf));
}

void AbradedOpaque::setAlbedo(const Vector3R& albedo)
{
	// FIXME
	//m_bsdf.setF0(std::make_shared<ConstantTexture>(albedo));
}

void AbradedOpaque::setRoughness(const real roughness)
{
	const real alpha = roughnessToAlpha(roughness);
	m_bsdf.setMicrofacet(std::make_shared<TrowbridgeReitz>(alpha));
}

void AbradedOpaque::setF0(const Vector3R& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedOpaque::setF0(const real r, const real g, const real b)
{
	SpectralStrength f0;
	f0.setRgb(Vector3R(r, g, b));
	m_bsdf.setFresnelEffect(std::make_shared<SchlickApproxConductorDielectricFresnel>(f0));
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
	loader.setFunc(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<AbradedOpaque> AbradedOpaque::ciLoad(const InputPacket& packet)
{
	Vector3R albedo(0.5f, 0.5f, 0.5f);
	Vector3R f0(0.04f, 0.04f, 0.04f);
	real roughness = 0.5f;

	albedo    = packet.getVector3r("albedo", albedo);
	f0        = packet.getVector3r("f0", f0);
	roughness = packet.getReal("roughness", roughness);

	std::unique_ptr<AbradedOpaque> material = std::make_unique<AbradedOpaque>();
	material->setAlbedo(albedo);
	material->setF0(f0);
	material->setRoughness(roughness);
	return material;
}

}// end namespace ph