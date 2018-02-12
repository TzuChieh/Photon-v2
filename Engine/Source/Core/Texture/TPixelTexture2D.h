#pragma once

#include "Core/Texture/TTexture.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"

namespace ph
{

template<typename Output, typename T, std::size_t N>
class TPixelTexture2D : public TTexture<Output>
{
public:
	inline TPixelTexture2D() :
		TPixelTexture2D(TFrame<T, N>()) 
	{}

	inline TPixelTexture2D(const TFrame<T, N>& frame) :
		TTexture<Output>(),
		m_frame(frame) 
	{}

	virtual ~TPixelTexture2D() = default override;

	inline virtual void sample(const SampleLocation& sampleLocation, Output* out_value) const = 0;

	inline void getPixel(
		const uint32 x, const uint32 y, 
		typename TFrame<T, N>::Pixel* const out_pixel) const
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