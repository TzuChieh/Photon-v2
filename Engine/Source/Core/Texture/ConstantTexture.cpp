#include "Core/Texture/ConstantTexture.h"

namespace ph
{

ConstantTexture::ConstantTexture(const real value) :
	m_rgb(value)
{

}

ConstantTexture::ConstantTexture(const real r, const real g, const real b) : 
	m_rgb(r, g, b)
{

}

ConstantTexture::ConstantTexture(const Vector3R& rgb) : 
	m_rgb(rgb)
{

}

ConstantTexture::~ConstantTexture() = default;

void ConstantTexture::sample(const Vector3R& uvw, real* const out_value) const
{
	*out_value = m_rgb.x;
}

void ConstantTexture::sample(const Vector3R& uvw, Vector3R* const out_value) const
{
	out_value->set(m_rgb);
}

void ConstantTexture::sample(const Vector3R& uvw, SpectralStrength* const out_value) const
{
	out_value->setRgb(m_rgb);
}

}// end namespace ph