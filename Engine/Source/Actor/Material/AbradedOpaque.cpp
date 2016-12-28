#include "Actor/Material/AbradedOpaque.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	m_bsdfCos()
{
	
}

AbradedOpaque::~AbradedOpaque() = default;

void AbradedOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<OpaqueMicrofacet>(m_bsdfCos));
}

void AbradedOpaque::setRoughness(const float32 roughness)
{
	m_bsdfCos.setRoughness(std::make_shared<ConstantTexture>(roughness, roughness, roughness));
}

void AbradedOpaque::setF0(const Vector3f& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedOpaque::setF0(const float32 r, const float32 g, const float32 b)
{
	m_bsdfCos.setF0(std::make_shared<ConstantTexture>(r, g, b));
}

}// end namespace ph