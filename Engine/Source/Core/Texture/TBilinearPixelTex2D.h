#pragma once

#include "Core/Texture/TPixelTex2D.h"
#include "Math/TVector2.h"

#include <utility>
#include <type_traits>
#include <cmath>

namespace ph
{

template<typename T, std::size_t N>
class TBilinearPixelTex2D : public TPixelTex2D<T, N>
{
public:
	using TPixelTex2D<T, N>::TPixelTex2D;

	inline void sample(
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
		const float64 x0 = std::floor(x - 0.5) + 0.5;
		const float64 y0 = std::floor(y - 0.5) + 0.5;
		const float64 x1 = x0 + 1.0;
		const float64 y1 = y0 + 1.0;

		const float64 weights[4]
		{
			(x1 - x) * (y1 - y),// weight 00
			(x1 - x) * (y - y0),// weight 01
			(x - x0) * (y1 - y),// weight 10
			(x - x0) * (y - y0) // weight 11
		};
		const math::Vector2D xys[4]
		{
			{x0, y0}, {x0, y1}, {x1, y0}, {x1, y1}
		};

		TTexPixel<float64, N> accuPixel(0);
		TTexPixel<T, N>       pixel;
		for(std::size_t i = 0; i < 4; ++i)
		{
			float64 normUi, normVi;
			this->normalizeUV(xys[i].x() * this->getTexelSizeU(),
			                  xys[i].y() * this->getTexelSizeV(),
			                  &normUi, &normVi);

			uint32 xi = static_cast<uint32>(normUi * w);
			uint32 yi = static_cast<uint32>(normVi * h);
			xi = xi < w ? xi : w - 1;
			yi = yi < h ? yi : h - 1;

			this->getPixel(xi, yi, &pixel);
			accuPixel.addLocal(TTexPixel<float64, N>(pixel).mulLocal(weights[i]));
		}

		// taking care of pixel value rounding
		if constexpr(std::is_integral_v<T>)
		{
			accuPixel.addLocal(0.5);
		}

		*out_value = TTexPixel<T, N>(accuPixel);
	}
};

}// end namespace ph
