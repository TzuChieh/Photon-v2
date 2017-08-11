#pragma once

#include "Actor/Texture/Texture.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

// TODO: this class should be named ConstantRgbTexture

class ConstantTexture : public Texture
{
public:
	ConstantTexture(const Vector3R& value);
	ConstantTexture(const real r, const real g, const real b);
	virtual ~ConstantTexture() override;

	virtual void sample(const Vector3R& uvw, SpectralStrength* out_value) const override;

private:
	Vector3R m_value;
};

}// end namespace ph