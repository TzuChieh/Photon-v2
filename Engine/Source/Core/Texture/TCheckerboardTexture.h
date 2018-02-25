#pragma once

#include "Core/Texture/TTexture.h"

#include <memory>

namespace ph
{

/*
	A texture that is able to represent two inner textures alternately, 
	producing pattern similar to a chessboard. We name the cells in the
	checkerboard in following manners:

	v
	+-----+-----+-----+
	| even|  odd| even|
	+-----+-----+-----+
	|  odd| even|  odd|
	+-----+-----+-----+
	| even|  odd| even|
	+-----+-----+-----+ u

*/
template<typename OutputType>
class TCheckerboardTexture final : public TTexture<OutputType>
{
public:
	inline TCheckerboardTexture(
		real numUtiles, real numVtiles,
		const OutputType& oddValue, 
		const OutputType& evenValue);

	inline TCheckerboardTexture(
		real numUtiles, real numVtiles,
		const std::shared_ptr<TTexture<OutputType>>& oddTexture, 
		const std::shared_ptr<TTexture<OutputType>>& evenTexture);

	inline virtual ~TCheckerboardTexture() override;

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* out_value) const override;

	// Sets the texture that is going to be used in odd cells.
	//
	inline void setOddTexture(const std::shared_ptr<TTexture<OutputType>>& oddTexture);

	// Sets the texture that is going to be used in even cells.
	//
	inline void setEvenTexture(const std::shared_ptr<TTexture<OutputType>>& evenTexture);

	// Sets the scale factors of cell texture. Larger u-, v- and w-scale 
	// makes texture appears to be larger in the corresponding direction, 
	// and vice versa.
	//
	inline void setOddTextureScale(const Vector3R& sale);
	inline void setEvenTextureScale(const Vector3R& scale);

private:
	std::shared_ptr<TTexture<OutputType>> m_oddTexture;
	std::shared_ptr<TTexture<OutputType>> m_evenTexture;

	real m_uTileSize;
	real m_vTileSize;
	Vector3R m_oddUvwScale;
	Vector3R m_evenUvwScale;
};

}// end namespace ph

#include "Core/Texture/TCheckerboardTexture.ipp"