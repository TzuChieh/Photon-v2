#include "Core/Texture/TCheckerboardTexture.h"
#include "Core/Texture/SampleLocation.h"
#include "Common/assertion.h"
#include "Core/Texture/constant_textures.h"

#include <iostream>
#include <memory>

namespace ph
{

template<typename OutputType>
inline TCheckerboardTexture<OutputType>::TCheckerboardTexture(
	const real numUtiles, const real numVtiles,
	const OutputType& oddValue, const OutputType& evenValue) : 

	TCheckerboardTexture(
		numUtiles, numVtiles, 
		std::make_shared<TConstantTexture<OutputType>>(oddValue),
		std::make_shared<TConstantTexture<OutputType>>(evenValue))
{}

template<typename OutputType>
inline TCheckerboardTexture<OutputType>::TCheckerboardTexture(
	const real numUtiles, const real numVtiles,
	const std::shared_ptr<TTexture<OutputType>>& oddTexture,
	const std::shared_ptr<TTexture<OutputType>>& evenTexture)
{
	if(numUtiles <= 0.0f || numVtiles <= 0.0f)
	{
		std::cerr << "warning: at TCheckerboardTexture's ctor, " 
		          << "number of tiles <= 0" << std::endl;
	}

	setOddTexture(oddTexture);
	setEvenTexture(evenTexture);

	setOddTextureScale(math::Vector3R(1.0_r / numUtiles));
	setEvenTextureScale(math::Vector3R(1.0_r / numVtiles));

	m_uTileSize = 1.0f / numUtiles;
	m_vTileSize = 1.0f / numVtiles;
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

	if(std::abs(uNumber % 2) != std::abs(vNumber % 2))
	{
		m_oddTexture->sample(sampleLocation.getUvwScaled(m_oddUvwScale), 
		                     out_value);
	}
	else
	{
		m_evenTexture->sample(sampleLocation.getUvwScaled(m_evenUvwScale), 
		                      out_value);
	}
}

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::setOddTexture(
	const std::shared_ptr<TTexture<OutputType>>& oddTexture)
{
	if(!oddTexture || oddTexture.get() == this)
	{
		std::cerr << "warning: at TCheckerboardTexture::setOddTexture(), " 
		          << "does not allow empty or self-referencing tile" << std::endl;
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
		std::cerr << "warning: at TCheckerboardTexture::setEvenTexture(), " 
		          << "does not allow empty or self-referencing tile" << std::endl;
		return;
	}

	m_evenTexture = evenTexture;
}

/*
	Note that in order to scale a texture, we need to multiply texture
	coordinates by a reciprocal factor.
*/

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::setOddTextureScale(const math::Vector3R& scale)
{
	m_oddUvwScale = scale.rcp();
}

template<typename OutputType>
inline void TCheckerboardTexture<OutputType>::setEvenTextureScale(const math::Vector3R& scale)
{
	m_evenUvwScale = scale.rcp();
}

}// end namespace ph
