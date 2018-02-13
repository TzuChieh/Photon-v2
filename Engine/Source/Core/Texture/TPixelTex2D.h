#pragma once

#include "Core/Texture/TTexture.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"

namespace ph
{

template<typename T, std::size_t N>
class TPixelTex2D : public TTexture<typename TFrame<T, N>::Pixel>
{
public:
	typedef typename TFrame<T, N>::Pixel Pixel;

public:
	inline TPixelTex2D() :
		TPixelTex2D(TFrame<T, N>())
	{}

	inline TPixelTex2D(const TFrame<T, N>& frame) :
		TTexture<Pixel>(),
		m_frame(frame) 
	{}

	virtual ~TPixelTex2D() = default override;

	virtual void sample(const SampleLocation& sampleLocation, Pixel* out_value) const = 0;

	inline void getPixel(
		const uint32 x, const uint32 y, 
		Pixel* const out_pixel) const
	{
		PH_ASSERT(out_pixel != nullptr);

		m_frame.getPixel(x, y, out_pixel);
	}

	inline void setPixels(const TFrame<T, N>& frame)
	{
		m_frame = frame;
	}

private:
	TFrame<T, N> m_frame;
};

}// end namespace ph