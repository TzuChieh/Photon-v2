#include "Core/Texture/TCheckerboardTexture.h"

#include <iostream>

namespace ph
{

template<typename OutputType>
inline TCheckerboardTexture<OutputType>::TCheckerboardTexture(
	const real nUtiles, const real nVtiles,
	const OutputType& oddValue, const OutputType& evenValue) :
	m_nUtiles(nUtiles), m_nVtiles(nVtiles), 
	m_oddValue(oddValue), m_evenValue(evenValue)
{
	if(nUtiles <= 0.0f || nVtiles <= 0.0f)
	{
		std::cerr << "warning: at CheckerboardTexture::CheckerboardTexture(), " 
		          << "number of tiles <= 0" << std::endl;
	}

	m_uTileSize = 1.0f / nUtiles;
	m_vTileSize = 1.0f / nVtiles;
}

template<typename OutputType>
inline TCheckerboardTexture<OutputType>::~TCheckerboardTexture() = default;

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::sample(
	const Vector3R& uvw, OutputType* const out_value) const
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