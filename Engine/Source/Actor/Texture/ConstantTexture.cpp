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

void ConstantTexture::sample(const Vector3R& uvw, Vector3R* const out_value) const
{
	out_value->set(m_value);
}

}// end namespace ph