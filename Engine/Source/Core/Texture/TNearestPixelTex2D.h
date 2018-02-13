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
	using TPixelTex2D<T, N>::TPixelTex2D<T, N>;
	virtual ~TPixelTex2D() = default override;

	virtual void sample(const SampleLocation& sampleLocation, TTexPixel<T, N>* const out_value) const override
	{
		const real   u = sampleLocation.uvw().x;
		const real   v = sampleLocation.uvw().y;
		const uint32 w = getWidthPx();
		const uint32 h = getHeightPx();

		PH_ASSERT(w != 0 && h != 0);

		uint32 x = static_cast<uint32>(Math::fractionalPart(u) * w);
		uint32 y = static_cast<uint32>(Math::fractionalPart(v) * h);
		x = x == w ? w - 1 : w;
		y = y == h ? h - 1 : h;

		return getPixel(x, y, out_value);
	}
};

}// end namespace ph