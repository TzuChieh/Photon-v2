#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Texture/ConstantTexture.h"
#include "FileIO/InputPacket.h"

#include <memory>
#include <algorithm>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	m_bsdfCos()
{
	
}

AbradedOpaque::AbradedOpaque(const InputPacket& packet) :
	Material(packet), 
	m_bsdfCos()
{
	Vector3R albedo(0.5f, 0.5f, 0.5f);
	Vector3R f0(0.04f, 0.04f, 0.04f);
	real roughness = 0.5f;
	albedo    = packet.getVector3R("albedo", albedo);
	f0        = packet.getVector3R("f0", f0);
	roughness = packet.getReal("roughness", roughness);

	setAlbedo(albedo);
	setF0(f0);
	setRoughness(roughness);
}

AbradedOpaque::~AbradedOpaque() = default;

void AbradedOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<OpaqueMicrofacet>(m_bsdfCos));
}

void AbradedOpaque::setAlbedo(const Vector3R& albedo)
{
	m_bsdfCos.setF0(std::make_shared<ConstantTexture>(albedo));
}

void AbradedOpaque::setRoughness(const real roughness)
{
	const real alpha = roughnessToAlpha(roughness);
	m_bsdfCos.setAlpha(std::make_shared<ConstantTexture>(alpha, alpha, alpha));
}

void AbradedOpaque::setF0(const Vector3R& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedOpaque::setF0(const real r, const real g, const real b)
{
	m_bsdfCos.setF0(std::make_shared<ConstantTexture>(r, g, b));
}

// This mapping is what used in PBRT-v3. 
// (Strangely the original paper: Microfacet Models for Refraction through Rough Surfaces by Walter et al. does 
// not include such mapping for GGX distribution, only the ones for other kinds of distribution.)
float32 AbradedOpaque::roughnessToAlpha(const real roughness)
{
	const real clampedRoughness = std::max(roughness, 0.001f);
	const real x = std::log(clampedRoughness);
	return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}

}// end namespace ph