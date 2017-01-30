#include "Actor/Texture/CheckerboardTexture.h"

#include <iostream>

namespace ph
{

CheckerboardTexture::CheckerboardTexture(const float32 nUtiles, const float32 nVtiles, const Vector3R& oddValue, const Vector3R& evenValue) :
	m_nUtiles(nUtiles), m_nVtiles(nVtiles), m_oddValue(oddValue), m_evenValue(evenValue)
{
	if(nUtiles <= 0.0f || nVtiles <= 0.0f)
	{
		std::cerr << "warning: at CheckerboardTexture::CheckerboardTexture(), number of tiles <= 0" << std::endl;
	}

	m_uTileSize = 1.0f / nUtiles;
	m_vTileSize = 1.0f / nVtiles;
}

CheckerboardTexture::~CheckerboardTexture() = default;

void CheckerboardTexture::sample(const Vector3R& uvw, Vector3R* const out_value) const
{
	const int32 uNumber = static_cast<int32>(std::floor(uvw.x / m_uTileSize));
	const int32 vNumber = static_cast<int32>(std::floor(uvw.y / m_vTileSize));

	if(std::abs(uNumber % 2) != std::abs(vNumber % 2))
	{
		*out_value = m_oddValue;
	}
	else
	{
		*out_value = m_evenValue;
	}
}

}// end namespace ph