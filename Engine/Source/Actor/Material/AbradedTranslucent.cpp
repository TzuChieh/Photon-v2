#include "Actor/Material/AbradedTranslucent.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"

#include <memory>
#include <cmath>
#include <algorithm>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	m_bsdfCos()
{
	
}

AbradedTranslucent::AbradedTranslucent(const InputPacket& packet) : 
	Material(packet), 
	m_bsdfCos()
{
	Vector3R albedo(0.5f, 0.5f, 0.5f);
	Vector3R f0(0.04f, 0.04f, 0.04f);
	real roughness = 0.5f;
	real ior       = 1.0f;
	albedo    = packet.getVector3R("albedo", albedo);
	f0        = packet.getVector3R("f0", f0);
	roughness = packet.getReal("roughness", roughness);
	ior       = packet.getReal("ior", ior);

	setAlbedo(albedo);
	setF0(f0);
	setRoughness(roughness);
	setIOR(ior);
}

AbradedTranslucent::~AbradedTranslucent() = default;

void AbradedTranslucent::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<TranslucentMicrofacet>(m_bsdfCos));
}

void AbradedTranslucent::setAlbedo(const Vector3R& albedo)
{
	m_bsdfCos.setF0(std::make_shared<ConstantTexture>(albedo));
}

void AbradedTranslucent::setF0(const Vector3R& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedTranslucent::setF0(const real r, const real g, const real b)
{
	m_bsdfCos.setF0(std::make_shared<ConstantTexture>(r, g, b));
}

void AbradedTranslucent::setIOR(const real ior)
{
	m_bsdfCos.setIOR(std::make_shared<ConstantTexture>(ior, ior, ior));
}

void AbradedTranslucent::setRoughness(const real roughness)
{
	const real alpha = roughnessToAlpha(roughness);
	m_bsdfCos.setAlpha(std::make_shared<ConstantTexture>(alpha, alpha, alpha));
}

// This mapping is what used in PBRT-v3. 
// (Strangely the original paper: Microfacet Models for Refraction through Rough Surfaces by Walter et al. does 
// not include such mapping for GGX distribution, only the ones for other kinds of distribution.)
real AbradedTranslucent::roughnessToAlpha(const real roughness)
{
	const real clampedRoughness = std::max(roughness, 0.001_r);
	const real x = std::log(clampedRoughness);
	return 1.62142_r + 0.819955_r * x + 0.1734_r * x * x + 0.0171201_r * x * x * x + 0.000640711_r * x * x * x * x;
}

}// end namespace ph