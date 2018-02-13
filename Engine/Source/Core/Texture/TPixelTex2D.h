#pragma once

#include "Core/Texture/TTexture.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

template<typename T, std::size_t N>
using TTexPixel = typename TFrame<T, N>::Pixel;

template<typename T, std::size_t N>
class TPixelTex2D : public TTexture<TTexPixel<T, N>>
{
public:
	inline TPixelTex2D() :
		TPixelTex2D(TFrame<T, N>())
	{}

	explicit inline TPixelTex2D(const TFrame<T, N>& frame) :
		TTexture<TTexPixel<T, N>>(),
		m_frame(frame) 
	{}

	virtual ~TPixelTex2D() = default override;

	virtual void sample(const SampleLocation& sampleLocation, TTexPixel<T, N>* out_value) const = 0;

	inline void getPixel(
		const uint32 x, const uint32 y, 
		TTexPixel<T, N>* const out_pixel) const
	{
		static_assert(std::is_same_v<TTexPixel<T, N>, typename TFrame<T, N>::Pixel>, 
		              "require same pixel type");

		PH_ASSERT(out_pixel != nullptr);

		m_frame.getPixel(x, y, out_pixel);
	}

	inline void setPixels(const TFrame<T, N>& frame)
	{
		m_frame = frame;
	}

	inline uint32 getWidthPx() const  { return m_frame.widthPx();  }
	inline uint32 getHeightPx() const { return m_frame.heightPx(); }

private:
	TFrame<T, N> m_frame;
};

}// end namespace ph