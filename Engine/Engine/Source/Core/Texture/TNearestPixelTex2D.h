#pragma once

#include "Core/Texture/TPixelTex2D.h"
#include "Core/Texture/SampleLocation.h"
#include "Math/math.h"

namespace ph
{

template<typename T, std::size_t N>
class TNearestPixelTex2D : public TPixelTex2D<T, N>
{
public:
	using TPixelTex2D<T, N>::TPixelTex2D;

	inline void sample(
		const SampleLocation&  sampleLocation, 
		TTexPixel<T, N>* const out_value) const override
	{
		const uint32 w = this->getWidthPx();
		const uint32 h = this->getHeightPx();

		PH_ASSERT(w != 0 && h != 0);

		// normalize uv first to avoid overflowing xy after multiplying
		// width and height
		float64 normU, normV;
		this->normalizeUV(sampleLocation.uvw().x(), sampleLocation.uvw().y(),
		                  &normU, &normV);
		
		// calculate pixel index and handle potential overflow
		uint32 x = static_cast<uint32>(normU * w);
		uint32 y = static_cast<uint32>(normV * h);
		x = x < w ? x : w - 1;
		y = y < h ? y : h - 1;

		this->getPixel(x, y, out_value);
	}
};

}// end namespace ph
