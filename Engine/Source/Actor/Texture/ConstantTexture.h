#pragma once

#include "Actor/Texture/Texture.h"
#include "Math/TVector3.h"

namespace ph
{

class ConstantTexture : public Texture
{
public:
	ConstantTexture(const Vector3R& value);
	ConstantTexture(const real r, const real g, const real b);
	virtual ~ConstantTexture() override;

	virtual void sample(const Vector3R& uvw, Vector3R* const out_value) const override;

private:
	Vector3R m_value;
};

}// end namespace ph