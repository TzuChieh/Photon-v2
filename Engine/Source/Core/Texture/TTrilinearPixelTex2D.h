#pragma once

#include "Core/Texture/TMipmap.h"

namespace ph
{
template<typename T, std::size_t N>
class TTrilinearPixelTex2D final : public TMipmap<T, N>
{
public:
	using TMipmap<T, N>::TMipmap;
	virtual ~TTrilinearPixelTex2D() override = default;

	virtual inline void sample(
		const SampleLocation&  sampleLocation,
		TTexPixel<T, N>* const out_value) const override
	{
		// TODO
	}

	inline void setMipLevel(const std::size_t level,
	                     std::unique_ptr<TAbstractPixelTex2D<T, N>> texture)
	{
		setLevel(level, std::move(texture));
	}
};	

}// end namespace ph