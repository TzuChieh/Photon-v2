#include "Actor/Material/AbradedTranslucent.h"
#include "Actor/Texture/ConstantTexture.h"

#include <memory>
#include <cmath>
#include <algorithm>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	m_bsdfCos()
{
	
}

AbradedTranslucent::~AbradedTranslucent() = default;

void AbradedTranslucent::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<TranslucentMicrofacet>(m_bsdfCos));
}

void AbradedTranslucent::setF0(const Vector3f& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedTranslucent::setF0(const float32 r, const float32 g, const float32 b)
{
	m_bsdfCos.setF0(std::make_shared<ConstantTexture>(r, g, b));
}

void AbradedTranslucent::setIOR(const float32 ior)
{
	m_bsdfCos.setIOR(std::make_shared<ConstantTexture>(ior, ior, ior));
}

void AbradedTranslucent::setRoughness(const float32 roughness)
{
	const float32 alpha = roughnessToAlpha(roughness);
	m_bsdfCos.setAlpha(std::make_shared<ConstantTexture>(alpha, alpha, alpha));
}

// This mapping is what used in PBRT-v3. 
// (Strangely the original paper: Microfacet Models for Refraction through Rough Surfaces by Walter et al. does 
// not include such mapping for GGX distribution, only the ones for other kinds of distribution.)
float32 AbradedTranslucent::roughnessToAlpha(const float32 roughness)
{
	const float32 clampedRoughness = std::max(roughness, 0.001f);
	const float32 x = std::log(clampedRoughness);
	return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}

}// end namespace ph