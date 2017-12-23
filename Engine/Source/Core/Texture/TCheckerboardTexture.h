#pragma once

#include "Core/Texture/TTexture.h"

namespace ph
{

template<typename OutputType>
class TCheckerboardTexture final : public TTexture<OutputType>
{
public:
	TCheckerboardTexture(real nUtiles, real nVtiles,
	                     const OutputType& oddValue, const OutputType& evenValue);
	virtual ~TCheckerboardTexture() override;

	virtual void sample(const Vector3R& uvw, OutputType* out_value) const override;

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