#include "Actor/Model/Material/MatteOpaque.h"
#include "Image/ConstantTexture.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	m_bsdfCos()
{
	
}

MatteOpaque::~MatteOpaque() = default;

void MatteOpaque::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<LambertianDiffuse>(m_bsdfCos));
}

void MatteOpaque::setAlbedo(const Vector3f& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const float32 r, const float32 g, const float32 b)
{
	setAlbedo(std::make_shared<ConstantTexture>(r, g, b));
}

void MatteOpaque::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_bsdfCos.setAlbedo(albedo);
}

}// end namespace ph