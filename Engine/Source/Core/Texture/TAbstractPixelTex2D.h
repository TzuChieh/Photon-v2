#pragma once

#include "Core/Texture/TTexture.h"
#include "Math/TArithmeticArray.h"
#include "Common/assertion.h"

namespace ph
{

template<typename T, std::size_t N>
using TTexPixel = TArithmeticArray<T, N>;

template<typename T, std::size_t N>
class TAbstractPixelTex2D : public TTexture<TTexPixel<T, N>>
{
public:
	inline TAbstractPixelTex2D() :
		TAbstractPixelTex2D(1, 1)
	{}

	inline TAbstractPixelTex2D(const uint32 widthPx, const uint32 heightPx) :
		TTexture<TTexPixel<T, N>>(),
		m_widthPx(widthPx), m_heightPx(heightPx)
	{
		PH_ASSERT(widthPx > 0 && heightPx > 0);
	}

	virtual ~TAbstractPixelTex2D() override = default;

	virtual void sample(
		const SampleLocation& sampleLocation, 
		TTexPixel<T, N>*      out_value) const override = 0;

	inline uint32 getWidthPx() const  { return m_widthPx;  }
	inline uint32 getHeightPx() const { return m_heightPx; }

	inline void setWidthPx(const uint32 widthPx)
	{
		PH_ASSERT(widthPx > 0);

		m_widthPx = widthPx;
	}

	inline void setHeightPx(const uint32 heightPx)
	{
		PH_ASSERT(heightPx > 0);

		m_heightPx = heightPx;
	}

protected:
	uint32 m_widthPx;
	uint32 m_heightPx;
};

}// end namespace ph