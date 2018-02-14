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
		const uint32 w = this->getWidthPx();
		const uint32 h = this->getHeightPx();

		PH_ASSERT(w != 0 && h != 0);

		// normalize uv first to avoid overflowing xy after multiplying
		// width and height
		float64 normU, normV;
		this->normalizeUV(sampleLocation.uvw().x, sampleLocation.uvw().y,
		                  &normU, &normV);

		uint32 normX, normY;
		this->normalizeXY(static_cast<uint32>(normU * w), static_cast<uint32>(normV * h),
		                  &normX, &normY);

		this->getPixel(normX, normY, out_value);
	}
};

}// end namespace ph