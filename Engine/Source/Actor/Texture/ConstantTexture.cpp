#include "Actor/Texture/ConstantTexture.h"

namespace ph
{

ConstantTexture::ConstantTexture(const Vector3R& value) :
	m_value(value)
{

}

ConstantTexture::ConstantTexture(const real r, const real g, const real b) :
	ConstantTexture(Vector3R(r, g, b))
{

}

ConstantTexture::~ConstantTexture() = default;

void ConstantTexture::sample(const Vector3R& uvw, SpectralStrength* const out_value) const
{
	out_value->setRgb(Vector3R(m_value));
}

}// end namespace ph