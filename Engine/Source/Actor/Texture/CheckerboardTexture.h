#pragma once

#include "Actor/Texture/Texture.h"
#include "Math/TVector3.h"

namespace ph
{

class CheckerboardTexture final : public Texture
{
public:
	CheckerboardTexture(const float32 nUtiles, const float32 nVtiles, const Vector3R& oddValue, const Vector3R& evenValue);
	virtual ~CheckerboardTexture() override;

	virtual void sample(const Vector3R& uvw, Vector3R* const out_value) const override;

private:
	float32 m_nUtiles;
	float32 m_nVtiles;
	Vector3R m_oddValue;
	Vector3R m_evenValue;

	float32 m_uTileSize;
	float32 m_vTileSize;
};

}// end namespace ph