#pragma once

#include "Core/Texture/TPixelTex2D.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/Math.h"

namespace ph
{

template<typename T, std::size_t N>
class TNearestPixelTex2D final : public TPixelTex2D<T, N>
{
public:
	using TPixelTex2D<T, N>::TPixelTex2D;
	virtual ~TNearestPixelTex2D() override = default;

	virtual inline void sample(
		const SampleLocation&  sampleLocation, 
		TTexPixel<T, N>* const out_value) const override
	{
		const real   u = sampleLocation.uvw().x;
		const real   v = sampleLocation.uvw().y;
		const uint32 w = this->getWidthPx();
		const uint32 h = this->getHeightPx();

		PH_ASSERT(w != 0 && h != 0);

		// normalizing uv to [0, 1] (wrapping mode: repeat)
		real normalizedU = Math::fractionalPart(u);
		real normalizedV = Math::fractionalPart(v);
		normalizedU = normalizedU >= 0.0_r ? normalizedU : normalizedU + 1.0_r;
		normalizedV = normalizedV >= 0.0_r ? normalizedV : normalizedV + 1.0_r;

		uint32 x = static_cast<uint32>(normalizedU * w);
		uint32 y = static_cast<uint32>(normalizedV * h);
		x = x < w ? x : w - 1;
		y = y < h ? y : h - 1;

		return this->getPixel(x, y, out_value);
	}
};

}// end namespace ph