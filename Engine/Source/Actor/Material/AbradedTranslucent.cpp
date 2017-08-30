#include "Actor/Material/AbradedTranslucent.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"
#include "Core/SurfaceBehavior/Utility/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Utility/TrowbridgeReitz.h"

#include <memory>
#include <cmath>
#include <algorithm>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	Material(),
	m_bsdf()
{
	
}

AbradedTranslucent::~AbradedTranslucent() = default;

void AbradedTranslucent::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdf(std::make_unique<TranslucentMicrofacet>(m_bsdf));
}

//void AbradedTranslucent::setAlbedo(const Vector3R& albedo)
//{
//	m_bsdf.setF0(std::make_shared<ConstantTexture>(albedo));
//}

//void AbradedTranslucent::setF0(const real iorOuter, const real iorInner)
//{
//	setF0(f0.x, f0.y, f0.z);
//}

//void AbradedTranslucent::setF0(const real r, const real g, const real b)
//{
//	m_bsdf.setF0(std::make_shared<ConstantTexture>(r, g, b));
//}

void AbradedTranslucent::setIor(const real iorOuter, const real iorInner)
{
	//m_bsdf.setFrenelEffect(std::make_shared<SchlickApproxDielectricFresnel>(iorOuter, iorInner));
	m_bsdf.setFrenelEffect(std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner));
}

void AbradedTranslucent::setRoughness(const real roughness)
{
	const real alpha = roughnessToAlpha(roughness);
	m_bsdf.setMicrofacet(std::make_shared<TrowbridgeReitz>(alpha));
}

// This mapping is what used in PBRT-v3. 
// (Strangely the original paper: Microfacet Models for Refraction through Rough Surfaces by Walter et al. does 
// not include such mapping for GGX distribution, only the ones for other kinds of distribution.)
real AbradedTranslucent::roughnessToAlpha(const real roughness)
{
	// roughness = 0, mapped result = 0.31116918

	const real clampedRoughness = std::max(roughness, 0.001_r);
	const real x = std::log(clampedRoughness);
	return 1.62142_r + 0.819955_r * x + 0.1734_r * x * x + 0.0171201_r * x * x * x + 0.000640711_r * x * x * x * x;
}

// command interface

SdlTypeInfo AbradedTranslucent::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "abraded-translucent");
}

void AbradedTranslucent::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AbradedTranslucent>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<AbradedTranslucent> AbradedTranslucent::ciLoad(const InputPacket& packet)
{
	Vector3R albedo(0.5f, 0.5f, 0.5f);
	Vector3R f0(0.04f, 0.04f, 0.04f);
	real roughness = 0.5f;
	real ior       = 1.0f;

	albedo    = packet.getVector3r("albedo", albedo);
	f0        = packet.getVector3r("f0", f0);
	roughness = packet.getReal("roughness", roughness);
	ior       = packet.getReal("ior", ior);

	std::unique_ptr<AbradedTranslucent> material = std::make_unique<AbradedTranslucent>();
	//material->setAlbedo(albedo);
	//material->setF0(f0);
	material->setRoughness(roughness);
	material->setIor(1, ior);
	return material;
}

}// end namespace ph