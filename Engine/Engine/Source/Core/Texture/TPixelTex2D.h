#pragma once

#include "Core/Texture/TAbstractPixelTex2D.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"

#include <type_traits>
#include <utility>

namespace ph
{

template<typename T, std::size_t N>
class TPixelTex2D : public TAbstractPixelTex2D<T, N>
{
public:
	inline TPixelTex2D() :
		TPixelTex2D(TFrame<T, N>(1, 1))
	{}

	explicit inline TPixelTex2D(const TFrame<T, N>& frame) :
		TAbstractPixelTex2D<T, N>(frame.widthPx(), frame.heightPx()),
		m_frame(frame) 
	{
		PH_ASSERT(!m_frame.isEmpty());
	}

	explicit inline TPixelTex2D(TFrame<T, N>&& frame) :
		TAbstractPixelTex2D<T, N>(frame.widthPx(), frame.heightPx()),
		m_frame(std::move(frame))
	{
		PH_ASSERT(!m_frame.isEmpty());
	}

	void sample(
		const SampleLocation& sampleLocation, 
		TTexPixel<T, N>*      out_value) const override = 0;

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
		setWidthPx(frame->widthPx());
		setHeightPx(frame->heightPx());

		PH_ASSERT(!m_frame.isEmpty());
	}

private:
	TFrame<T, N> m_frame;
};

}// end namespace ph
