#pragma once

#include "Core/Texture/TPixelTex2D.h"

#include <utility>
#include <type_traits>

namespace ph
{

template<typename T, std::size_t N>
class TBilinearPixelTex2D final : public TPixelTex2D<T, N>
{
public:
	using TPixelTex2D<T, N>::TPixelTex2D;
	virtual ~TBilinearPixelTex2D() override = default;

	virtual inline void sample(
		const SampleLocation&  sampleLocation, 
		TTexPixel<T, N>* const out_value) const override
	{
		const float64 u = sampleLocation.uvw().x;
		const float64 v = sampleLocation.uvw().y;
		const uint32  w = this->getWidthPx();
		const uint32  h = this->getHeightPx();

		PH_ASSERT(w != 0 && h != 0);

		float64 normU, normV;
		this->normalizeUV(u, v, &normU, &normV);

		const float64 x  = normU * w;
		const float64 y  = normV * h;
		const uint32  x0 = static_cast<uint32>(x);
		const uint32  y0 = static_cast<uint32>(y);
		const uint32  x1 = x0 + 1;
		const uint32  y1 = y0 + 1;

		PH_ASSERT_MSG(x1 > x0 && y1 > y0, 
		              "too large a dimension for uint32 to store causing overflow");

		const std::pair<uint32, uint32> xys[4]
		{
			{x0, y0}, {x0, y1}, {x1, y0}, {x1, y1}
		};
		const float64 weights[4]
		{
			(x1 - x) * (y1 - y),// weight 00
			(x1 - x) * (y - y0),// weight 01
			(x - x0) * (y1 - y),// weight 10
			(x - x0) * (y - y0) // weight 11
		};

		TTexPixel<float64, N> accuPixel(0);
		TTexPixel<T, N>       pixel;
		for(std::size_t i = 0; i < 4; ++i)
		{
			uint32 normX, normY;
			this->normalizeXY(xys[i].first, xys[i].second, &normX, &normY);

			this->getPixel(normX, normY, &pixel);
			accuPixel.addLocal(TTexPixel<float64, N>(pixel).mulLocal(weights[i]));
		}

		if constexpr(!std::is_integral_v<T>)
		{
			accuPixel.addLocal(0.5);
		}
		*out_value = TTexPixel<T, N>(accuPixel);
	}
};

}// end namespace ph