#include "Actor/Material/AbradedTranslucent.h"
#include "Image/ConstantTexture.h"

#include <memory>

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
	m_bsdfCos.setRoughness(std::make_shared<ConstantTexture>(roughness, roughness, roughness));
}

}// end namespace ph