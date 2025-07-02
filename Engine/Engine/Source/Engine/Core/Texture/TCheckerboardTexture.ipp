#include "Engine/Core/Texture/TCheckerboardTexture.h"
#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Core/Texture/constant_textures.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

template<typename OutputType>
inline TCheckerboardTexture<OutputType>::TCheckerboardTexture(
	const real numUTiles,
	const real numVTiles,
	const OutputType& oddValue, const OutputType& evenValue) : 

	TCheckerboardTexture(
		numUTiles, numVTiles, 
		std::make_shared<TConstantTexture<OutputType>>(oddValue),
		std::make_shared<TConstantTexture<OutputType>>(evenValue))
{}

template<typename OutputType>
inline TCheckerboardTexture<OutputType>::TCheckerboardTexture(
	const real numUTiles,
	const real numVTiles,
	const std::shared_ptr<TTexture<OutputType>>& oddTexture,
	const std::shared_ptr<TTexture<OutputType>>& evenTexture)
{
	if(numUTiles <= 0.0f || numVTiles <= 0.0f)
	{
		PH_DEFAULT_LOG(Warning,
			"at TCheckerboardTexture's ctor, number of tiles <= 0 (numUtiles = {}, numVtiles = {})",
			numUTiles, numVTiles);
	}

	setOddTexture(oddTexture);
	setEvenTexture(evenTexture);
	setNumTiles(numUTiles, numVTiles);
}

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::sample(
	const SampleLocation& sampleLocation, OutputType* const out_value) const
{
	PH_ASSERT(m_oddTexture  && m_oddTexture.get()  != this);
	PH_ASSERT(m_evenTexture && m_evenTexture.get() != this);

	const math::Vector3R uvw = sampleLocation.uvw();
	const int32 uNumber = static_cast<int32>(std::floor(uvw.x() / m_uTileSize));
	const int32 vNumber = static_cast<int32>(std::floor(uvw.y() / m_vTileSize));

	TTexture<OutputType>* tileTexture = nullptr;
	if(std::abs(uNumber % 2) != std::abs(vNumber % 2))
	{
		tileTexture = m_oddTexture.get();
	}
	else
	{
		tileTexture = m_evenTexture.get();
	}

	tileTexture->sample(sampleLocation.getUvwScaled(m_uvwScale), out_value);
}

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::setOddTexture(
	const std::shared_ptr<TTexture<OutputType>>& oddTexture)
{
	if(!oddTexture || oddTexture.get() == this)
	{
		PH_DEFAULT_LOG(Warning,
			"at TCheckerboardTexture::setOddTexture(), does not allow empty or self-referencing tile");
		return;
	}

	m_oddTexture = oddTexture;
}

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::setEvenTexture(
	const std::shared_ptr<TTexture<OutputType>>& evenTexture)
{
	if(!evenTexture || evenTexture.get() == this)
	{
		PH_DEFAULT_LOG(Warning,
			"at TCheckerboardTexture::setEvenTexture(), does not allow empty or self-referencing tile");
		return;
	}

	m_evenTexture = evenTexture;
}

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::setNumTiles(real numUTiles, real numVTiles)
{
	m_uTileSize = 1.0f / numUTiles;
	m_vTileSize = 1.0f / numVTiles;

	// Note that in order to scale a texture, we need to multiply texture coordinates by
	// the reciprocal of tile size, which is equivalent to multiplying by tile size.
	m_uvwScale = {numUTiles, numVTiles, 1};
}

}// end namespace ph
