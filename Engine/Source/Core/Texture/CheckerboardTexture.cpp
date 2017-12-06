#include "Core/Texture/CheckerboardTexture.h"

#include <iostream>

namespace ph
{

CheckerboardTexture::CheckerboardTexture(const real nUtiles, const real nVtiles, 
                                         const Vector3R& oddRgbValue, const Vector3R& evenRgbValue) :
	m_nUtiles(nUtiles), m_nVtiles(nVtiles), 
	m_oddRgbValue(oddRgbValue), m_evenRgbValue(evenRgbValue)
{
	if(nUtiles <= 0.0f || nVtiles <= 0.0f)
	{
		std::cerr << "warning: at CheckerboardTexture::CheckerboardTexture(), " 
		          << "number of tiles <= 0" << std::endl;
	}

	m_uTileSize = 1.0f / nUtiles;
	m_vTileSize = 1.0f / nVtiles;
}

CheckerboardTexture::~CheckerboardTexture() = default;

void CheckerboardTexture::sample(const Vector3R& uvw, real* out_value) const
{
	Vector3R rgb;
	CheckerboardTexture::sample(uvw, &rgb);
	*out_value = rgb.x;
}

void CheckerboardTexture::sample(const Vector3R& uvw, Vector3R* out_value) const
{
	const int32 uNumber = static_cast<int32>(std::floor(uvw.x / m_uTileSize));
	const int32 vNumber = static_cast<int32>(std::floor(uvw.y / m_vTileSize));

	if(std::abs(uNumber % 2) != std::abs(vNumber % 2))
	{
		out_value->set(m_oddRgbValue);
	}
	else
	{
		out_value->set(m_evenRgbValue);
	}
}

void CheckerboardTexture::sample(const Vector3R& uvw, SpectralStrength* const out_value) const
{
	Vector3R rgb;
	CheckerboardTexture::sample(uvw, &rgb);
	out_value->setRgb(rgb);
}

}// end namespace ph