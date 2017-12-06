#pragma once

#include "Core/Texture/Texture.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class CheckerboardTexture final : public Texture
{
public:
	CheckerboardTexture(real nUtiles, real nVtiles, 
	                    const Vector3R& oddRgbValue, const Vector3R& evenRgbValue);
	virtual ~CheckerboardTexture() override;

	virtual void sample(const Vector3R& uvw, real* out_value) const override;
	virtual void sample(const Vector3R& uvw, Vector3R* out_value) const override;
	virtual void sample(const Vector3R& uvw, SpectralStrength* out_value) const override;

private:
	real     m_nUtiles;
	real     m_nVtiles;
	Vector3R m_oddRgbValue;
	Vector3R m_evenRgbValue;

	real m_uTileSize;
	real m_vTileSize;
};

}// end namespace ph