#pragma once

#include "Core/Texture/TTexture.h"

namespace ph
{

template<typename OutputType>
class TCheckerboardTexture final : public TTexture<OutputType>
{
public:
	inline TCheckerboardTexture(real nUtiles, real nVtiles,
	                     const OutputType& oddValue, const OutputType& evenValue);
	inline virtual ~TCheckerboardTexture() override;

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* out_value) const override;

private:
	real       m_nUtiles;
	real       m_nVtiles;
	OutputType m_oddValue;
	OutputType m_evenValue;

	real m_uTileSize;
	real m_vTileSize;
};

}// end namespace ph

#include "Core/Texture/TCheckerboardTexture.h"