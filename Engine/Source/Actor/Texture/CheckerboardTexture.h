#pragma once

#include "Actor/Texture/Texture.h"
#include "Math/TVector3.h"

namespace ph
{

class CheckerboardTexture final : public Texture
{
public:
	CheckerboardTexture(const real nUtiles, const real nVtiles, const Vector3R& oddValue, const Vector3R& evenValue);
	virtual ~CheckerboardTexture() override;

	virtual void sample(const Vector3R& uvw, Vector3R* const out_value) const override;

private:
	real     m_nUtiles;
	real     m_nVtiles;
	Vector3R m_oddValue;
	Vector3R m_evenValue;

	real m_uTileSize;
	real m_vTileSize;
};

}// end namespace ph