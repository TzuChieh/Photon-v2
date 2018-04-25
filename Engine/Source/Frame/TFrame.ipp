#pragma once

#include "Frame/TFrame.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/Math.h"

#include <limits>

namespace ph
{

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame() :
	TFrame(0, 0)
{}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame(const uint32 wPx, const uint32 hPx) :
	m_widthPx(wPx), m_heightPx(hPx),
	m_pixelData(wPx * hPx * N, 0)
{}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame(const TFrame& other) :
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_pixelData(other.m_pixelData)
{}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame(TFrame&& other) :
	m_widthPx(other.m_widthPx), m_heightPx(other.m_heightPx),
	m_pixelData(std::move(other.m_pixelData))
{}

template<typename T, std::size_t N>
inline void TFrame<T, N>::fill(const T value)
{
	for(auto& component : m_pixelData)
	{
		component = value;
	}
}

// TODO: wrap mode
template<typename T, std::size_t N>
inline void TFrame<T, N>::sample(
	TFrame& sampled, const TMathFunction2D<float64>& kernel, const uint32 kernelRadiusPx) const
{
	if(isEmpty() || sampled.isEmpty() ||
	   kernelRadiusPx == 0)
	{
		sampled.fill(0);
		return;
	}

	for(uint32 y = 0; y < sampled.heightPx(); ++y)
	{
		for(uint32 x = 0; x < sampled.widthPx(); ++x)
		{
			const Vector2D samplePosPx((x + 0.5) / sampled.widthPx() * widthPx(), 
			                           (y + 0.5) / sampled.heightPx() * heightPx());

			// compute filter bounds
			Vector2D filterMin(samplePosPx.sub(kernelRadiusPx));
			Vector2D filterMax(samplePosPx.add(kernelRadiusPx));

			// make filter bounds not to exceed frame bounds
			filterMin = filterMin.max(Vector2D(0, 0));
			filterMax = filterMax.min(Vector2D(widthPx(), heightPx()));

			PH_ASSERT(filterMin.x <= filterMax.x && filterMin.y <= filterMax.y);

			// compute pixel index bounds
			TVector2<int64> x0y0(filterMin.sub(0.5).ceil());
			TVector2<int64> x1y1(filterMax.sub(0.5).floor());

			PH_ASSERT(x0y0.x >= 0 && x0y0.y >= 0 &&
			          x1y1.x < widthPx() && x1y1.y < heightPx());

			TPixel<float64> pixelSum  = getMonochromaticPixel<float64>(0);
			float64         weightSum = 0.0;
			for(int64 ky = x0y0.y; ky <= x1y1.y; ++ky)
			{
				for(int64 kx = x0y0.x; kx <= x1y1.x; ++kx)
				{
					const float64 kernelX = (kx + 0.5) - samplePosPx.x;
					const float64 kernelY = (ky + 0.5) - samplePosPx.y;

					Pixel pixel;
					getPixel(static_cast<uint32>(kx), static_cast<uint32>(ky), &pixel);
					const float64 weight = kernel.evaluate(kernelX, kernelY);

					for(std::size_t i = 0; i < N; ++i)
					{
						pixelSum[i] += static_cast<float64>(pixel[i]);
					}
					weightSum += weight;
				}// 
			}    // end for each pixel in kernel support

			Pixel sampledPixel;
			if(weightSum > 0.0)
			{
				const float64 reciWeightSum = 1.0 / weightSum;
				for(std::size_t i = 0; i < N; ++i)
				{
					float64 sampledValue = pixelSum[i] * reciWeightSum;
					sampledValue = Math::clamp(sampledValue, 
					                           static_cast<float64>(std::numeric_limits<T>::min()),
					                           static_cast<float64>(std::numeric_limits<T>::max()));
					sampledPixel[i] = static_cast<T>(sampledValue);
				}
			}
			else
			{
				sampledPixel = getMonochromaticPixel(T(0));
			}
			sampled.setPixel(x, y, sampledPixel);
		}// 
	}    // end for each pixel in sampled frame
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::flipHorizontally()
{
	const uint32 halfWidthPx = m_widthPx / 2;

	for(uint32 y = 0; y < m_heightPx; ++y)
	{
		for(uint32 x = 0; x < halfWidthPx; ++x)
		{
			const std::size_t leftPixelBegin  = calcPixelDataBaseIndex(x, y);
			const std::size_t rightPixelBegin = calcPixelDataBaseIndex(m_widthPx - 1 - x, y);

			for(std::size_t i = 0; i < N; ++i)
			{
				std::swap(m_pixelData[leftPixelBegin + i], m_pixelData[rightPixelBegin + i]);
			}
		}
	}
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::flipVertically()
{
	const uint32      halfHeightPx   = m_heightPx / 2;
	const std::size_t numRowElements = m_widthPx * N;

	for(uint32 y = 0; y < halfHeightPx; ++y)
	{
		const std::size_t bottomRowBegin = calcPixelDataBaseIndex(0, y);
		const std::size_t topRowBegin    = calcPixelDataBaseIndex(0, m_heightPx - 1 - y);

		for(std::size_t i = 0; i < numRowElements; ++i)
		{
			std::swap(m_pixelData[bottomRowBegin + i], m_pixelData[topRowBegin + i]);
		}
	}
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::getPixel(
	const uint32 x, const uint32 y, 
	Pixel* const out_pixel) const
	-> void
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	for(std::size_t i = 0; i < N; ++i)
	{
		(*out_pixel)[i] = m_pixelData[baseIndex + i];
	}
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::setPixel(
	const uint32 x, const uint32 y, const Pixel& pixel)
	-> void
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	for(std::size_t i = 0; i < N; ++i)
	{
		m_pixelData[baseIndex + i] = pixel[i];
	}
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::getPixelData() const
	-> const T*
{
	return m_pixelData.data();
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::calcPixelDataBaseIndex(
	const uint32 x, const uint32 y) const
	-> std::size_t
{
	PH_ASSERT(x < m_widthPx && y < m_heightPx);

	return (y * static_cast<std::size_t>(m_widthPx) + x) * N;
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::operator = (const TFrame& rhs)
	-> TFrame<T, N>&
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = rhs.m_pixelData;

	return *this;
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::operator = (TFrame&& rhs)
	-> TFrame<T, N>&
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = std::move(rhs.m_pixelData);

	return *this;
}

}// end namespace ph