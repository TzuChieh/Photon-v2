#pragma once

#include "Image/Texture.h"
#include "Math/Vector3f.h"

namespace ph
{

class CheckerboardTexture final : public Texture
{
public:
	CheckerboardTexture(const float32 nUtiles, const float32 nVtiles, const Vector3f& oddValue, const Vector3f& evenValue);
	virtual ~CheckerboardTexture() override;

	virtual void sample(const float32 u, const float32 v, Vector3f* const out_value) const override;

private:
	float32 m_nUtiles;
	float32 m_nVtiles;
	Vector3f m_oddValue;
	Vector3f m_evenValue;

	float32 m_uTileSize;
	float32 m_vTileSize;
};

}// end namespace ph