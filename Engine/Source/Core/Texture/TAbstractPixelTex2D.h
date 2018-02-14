#pragma once

#include "Core/Texture/TTexture.h"
#include "Math/TArithmeticArray.h"
#include "Common/assertion.h"
#include "Math/Math.h"

namespace ph
{

enum class ETexWrapMode
{
	REPEAT,
	CLAMP_TO_EDGE
};

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
		m_wrapMode(ETexWrapMode::REPEAT)
	{
		setWidthPx(widthPx);
		setHeightPx(heightPx);
	}

	virtual ~TAbstractPixelTex2D() override = default;

	virtual void sample(
		const SampleLocation& sampleLocation, 
		TTexPixel<T, N>*      out_value) const override = 0;

	inline uint32       getWidthPx() const    { return m_widthPx;    }
	inline uint32       getHeightPx() const   { return m_heightPx;   }
	inline float64      getTexelSizeU() const { return m_texelSizeU; }
	inline float64      getTexelSizeV() const { return m_texelSizeV; }
	inline ETexWrapMode getWrapMode() const   { return m_wrapMode;   }

	inline void setWrapMode(const ETexWrapMode mode)
	{
		m_wrapMode = mode;
	}

protected:
	// Normalizing (u, v) coordinates to [0, 1] according to wrapping mode.
	inline void normalizeUV(const float64 u, const float64 v, 
	                        float64* const out_u, float64* const out_v) const
	{
		switch(m_wrapMode)
		{
		case ETexWrapMode::REPEAT:
		{
			const float64 fu = Math::fractionalPart(u);
			const float64 fv = Math::fractionalPart(v);
			*out_u = fu >= 0.0 ? fu : fu + 1.0;
			*out_v = fv >= 0.0 ? fv : fv + 1.0;
			break;
		}
		case ETexWrapMode::CLAMP_TO_EDGE:
		{
			*out_u = Math::clamp(u, 0.0, 1.0);
			*out_v = Math::clamp(v, 0.0, 1.0);
			break;
		}
		default:
			PH_ASSERT_UNREACHABLE_SECTION();
		}

		PH_ASSERT(*out_u >= 0.0 && *out_u <= 1.0 && 
		          *out_v >= 0.0 && *out_v <= 1.0);
	}

	inline void setWidthPx(const uint32 widthPx)
	{
		PH_ASSERT(widthPx > 0);

		m_widthPx    = widthPx;
		m_texelSizeU = 1.0 / widthPx;
	}

	inline void setHeightPx(const uint32 heightPx)
	{
		PH_ASSERT(heightPx > 0);

		m_heightPx   = heightPx;
		m_texelSizeV = 1.0 / heightPx;
	}

protected:
	uint32       m_widthPx;
	uint32       m_heightPx;
	float64      m_texelSizeU;
	float64      m_texelSizeV;
	ETexWrapMode m_wrapMode;
};

}// end namespace ph