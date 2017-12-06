#pragma once

#include "Core/Texture/Texture.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class ConstantTexture : public Texture
{
public:
	ConstantTexture(real value);
	ConstantTexture(real r, real g, real b);
	ConstantTexture(const Vector3R& rgb);
	virtual ~ConstantTexture() override;

	virtual void sample(const Vector3R& uvw, real* out_value) const override;
	virtual void sample(const Vector3R& uvw, Vector3R* out_value) const override;
	virtual void sample(const Vector3R& uvw, SpectralStrength* out_value) const override;

private:
	Vector3R m_rgb;
};

}// end namespace ph