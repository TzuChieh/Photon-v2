#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Frame/frame_fwd.h"
#include "Math/Function/TMathFunction2D.h"

#include <vector>
#include <cstddef>
#include <array>

namespace ph
{

template<typename T, std::size_t N>
class TFrame final
{
public:
	template<typename U>
	using TPixel = std::array<U, N>;

	typedef TPixel<T> Pixel;

	template<typename U = T>
	static inline TPixel<U> getMonochromaticPixel(const U value)
	{
		TPixel<U> result;
		for(std::size_t i = 0; i < result.size(); ++i)
		{
			result[i] = U(value);
		}
		return result;
	}

public:
	inline TFrame();
	inline TFrame(uint32 wPx, uint32 hPx);
	inline TFrame(const TFrame& other);
	inline TFrame(TFrame&& other);
	inline ~TFrame() = default;

	inline void fill(T value);

	// TODO: user specified clamping range?
	inline void sample(
		TFrame& sampled, 
		const TMathFunction2D<float64>& kernel, uint32 kernelRadiusPx) const;

	inline void getPixel(uint32 x, uint32 y, Pixel* out_pixel) const;
	inline void setPixel(uint32 x, uint32 y, const Pixel& pixel);
	inline const T* getPixelData() const;

	inline TFrame& operator = (const TFrame& rhs);
	inline TFrame& operator = (TFrame&& rhs);

	inline uint32 widthPx() const
	{
		return m_widthPx;
	}

	inline uint32 heightPx() const
	{
		return m_heightPx;
	}

	inline std::size_t numRgbDataElements() const
	{
		return m_pixelData.size();
	}

	inline bool isEmpty() const
	{
		return m_pixelData.empty();
	}

	friend inline void swap(TFrame& first, TFrame& second);

	// HACK
	inline std::vector<real> getRealData() const
	{
		std::vector<real> realData;
		for(std::size_t i = 0; i < m_pixelData.size(); i++)
		{
			realData.push_back(m_pixelData[i] / 255.0_r);
		}

		return realData;
	}

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<T> m_pixelData;

	inline std::size_t calcPixelDataBaseIndex(uint32 x, uint32 y) const;
};

}// end namespace ph

#include "Frame/TFrame.ipp"