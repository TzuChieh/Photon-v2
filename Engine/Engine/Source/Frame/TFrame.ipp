#pragma once

#include "Frame/TFrame.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <limits>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <concepts>

namespace ph
{

namespace frame_detail
{

template<typename Func, typename T, std::size_t N>
concept CIsGetPixelOp = requires (Func func, typename TFrame<T, N>::PixelType pixel)
{
	// Requires returning `void`--to disambiguate with set ops
	{ func(pixel) } -> std::same_as<void>;
};

template<typename Func, typename T, std::size_t N>
concept CIsGetPixelWithCoordsOp = requires (Func func, uint32 x, uint32 y, typename TFrame<T, N>::PixelType pixel)
{
	// Requires returning `void`--to disambiguate with set ops
	{ func(x, y, pixel) } -> std::same_as<void>;
};

template<typename Func, typename T, std::size_t N>
concept CIsSetPixelOp = requires (Func func)
{
	{ func() } -> std::convertible_to<typename TFrame<T, N>::PixelType>;
};

template<typename Func, typename T, std::size_t N>
concept CIsSetPixelWithCoordsOp = requires (Func func, uint32 x, uint32 y)
{
	{ func(x, y) } -> std::convertible_to<typename TFrame<T, N>::PixelType>;
};

template<typename Func, typename T, std::size_t N>
concept CIsGetAndSetPixelOp = requires (Func func, typename TFrame<T, N>::PixelType pixel)
{
	{ func(pixel) } -> std::convertible_to<typename TFrame<T, N>::PixelType>;
};

template<typename Func, typename T, std::size_t N>
concept CIsGetAndSetPixelWithCoordsOp = requires (Func func, uint32 x, uint32 y, typename TFrame<T, N>::PixelType pixel)
{
	{ func(x, y, pixel) } -> std::convertible_to<typename TFrame<T, N>::PixelType>;
};

}// end namespace frame_detail

template<typename T, std::size_t N>
template<typename U>
inline typename TFrame<T, N>::template TPixelType<U> TFrame<T, N>::makeMonochromaticPixel(const U value)
{
	return TPixelType<U>(value);
}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame() :
	TFrame(0, 0)
{}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame(const uint32 wPx, const uint32 hPx) :
	m_widthPx  (wPx), 
	m_heightPx (hPx),
	m_pixelData(wPx * hPx * N, 0)
{}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame(const TFrame& other) :
	m_widthPx  (other.m_widthPx), 
	m_heightPx (other.m_heightPx),
	m_pixelData(other.m_pixelData)
{}

template<typename T, std::size_t N>
inline TFrame<T, N>::TFrame(TFrame&& other) noexcept :
	m_widthPx  (other.m_widthPx), 
	m_heightPx (other.m_heightPx),
	m_pixelData(std::move(other.m_pixelData))
{}

template<typename T, std::size_t N>
inline void TFrame<T, N>::fill(const T value)
{
	std::fill(m_pixelData.begin(), m_pixelData.end(), value);
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::fill(const T value, const math::TAABB2D<uint32>& region)
{
	PH_ASSERT_MSG(!region.isEmpty(), region.toString());

	const uint32 regionDataWidth = static_cast<uint32>(N) * region.getWidth();
	for(uint32 y = region.getMinVertex().y(); y < region.getMaxVertex().y(); ++y)
	{
		const std::size_t offset = calcPixelDataBaseIndex(region.getMinVertex().x(), y);

		std::fill(
			m_pixelData.begin() + offset,
			m_pixelData.begin() + offset + regionDataWidth,
			value);
	}
}

// TODO: wrap mode
template<typename T, std::size_t N>
inline void TFrame<T, N>::sample(
	TFrame& sampled,
	const math::TMathFunction2D<float64>& kernel,
	const uint32 kernelRadiusPx) const
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
			const math::Vector2D samplePosPx(
				(x + 0.5) / sampled.widthPx() * widthPx(),
				(y + 0.5) / sampled.heightPx() * heightPx());

			// compute filter bounds
			math::Vector2D filterMin(samplePosPx.sub(kernelRadiusPx));
			math::Vector2D filterMax(samplePosPx.add(kernelRadiusPx));

			// make filter bounds not to exceed frame bounds
			filterMin = filterMin.max(math::Vector2D(0, 0));
			filterMax = filterMax.min(math::Vector2D(widthPx(), heightPx()));

			PH_ASSERT_LE(filterMin.x(), filterMax.x());
			PH_ASSERT_LE(filterMin.y(), filterMax.y());

			// compute pixel index bounds
			math::TVector2<int64> x0y0(filterMin.sub(0.5).ceil());
			math::TVector2<int64> x1y1(filterMax.sub(0.5).floor());

			PH_ASSERT(x0y0.x() >= 0 && x0y0.y() >= 0 &&
			          x1y1.x() < widthPx() && x1y1.y() < heightPx());

			TPixelType<float64> pixelSum  = makeMonochromaticPixel<float64>(0);
			float64 weightSum = 0.0;
			for(int64 ky = x0y0.y(); ky <= x1y1.y(); ++ky)
			{
				for(int64 kx = x0y0.x(); kx <= x1y1.x(); ++kx)
				{
					const float64 kernelX = (kx + 0.5) - samplePosPx.x();
					const float64 kernelY = (ky + 0.5) - samplePosPx.y();

					PixelType pixel;
					getPixel(static_cast<uint32>(kx), static_cast<uint32>(ky), &pixel);
					const float64 weight = kernel.evaluate(kernelX, kernelY);

					for(std::size_t i = 0; i < N; ++i)
					{
						pixelSum[i] += static_cast<float64>(pixel[i]);
					}
					weightSum += weight;
				}// 
			}    // end for each pixel in kernel support

			PixelType sampledPixel;
			if(weightSum > 0.0)
			{
				const float64 reciWeightSum = 1.0 / weightSum;
				for(std::size_t i = 0; i < N; ++i)
				{
					const float64 sampledValue = pixelSum[i] * reciWeightSum;
					sampledPixel[i] = static_cast<T>(sampledValue);
				}
			}
			else
			{
				sampledPixel = makeMonochromaticPixel(T(0));
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
inline void TFrame<T, N>::setSize(const uint32 wPx, const uint32 hPx)
{
	m_widthPx  = wPx;
	m_heightPx = hPx;
	m_pixelData.resize(wPx * hPx * N);
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::setSize(const math::TVector2<uint32>& sizePx)
{
	setSize(sizePx.x(), sizePx.y());
}

template<typename T, std::size_t N>
template<typename PerPixelOperation>
inline void TFrame<T, N>::forEachPixel(PerPixelOperation op)
{
	forEachPixel(math::TAABB2D<uint32>({0, 0}, {m_widthPx, m_heightPx}), std::move(op));
}

template<typename T, std::size_t N>
template<typename PerPixelOperation>
inline void TFrame<T, N>::forEachPixel(PerPixelOperation op) const
{
	forEachPixel(math::TAABB2D<uint32>({0, 0}, {m_widthPx, m_heightPx}), std::move(op));
}

template<typename T, std::size_t N>
template<typename PerPixelOperation>
inline void TFrame<T, N>::forEachPixel(const math::TAABB2D<uint32>& region, PerPixelOperation op)
{
	// OPT

	PixelType pixel;
	for(uint32 y = region.getMinVertex().y(); y < region.getMaxVertex().y(); ++y)
	{
		for(uint32 x = region.getMinVertex().x(); x < region.getMaxVertex().x(); ++x)
		{
			getPixel(x, y, &pixel);
			
			if constexpr(frame_detail::CIsGetPixelOp<PerPixelOperation, T, N>)
			{
				op(pixel);
			}
			else if constexpr(frame_detail::CIsGetPixelWithCoordsOp<PerPixelOperation, T, N>)
			{
				op(x, y, pixel);
			}
			else if constexpr(frame_detail::CIsSetPixelOp<PerPixelOperation, T, N>)
			{
				setPixel(x, y, op());
			}
			else if constexpr(frame_detail::CIsSetPixelWithCoordsOp<PerPixelOperation, T, N>)
			{
				setPixel(x, y, op(x, y));
			}
			else if constexpr(frame_detail::CIsGetAndSetPixelOp<PerPixelOperation, T, N>)
			{
				setPixel(x, y, op(pixel));
			}
			else if constexpr(frame_detail::CIsGetAndSetPixelWithCoordsOp<PerPixelOperation, T, N>)
			{
				setPixel(x, y, op(x, y, pixel));
			}
			else
			{
				// Unsupported per pixel operation
				PH_ASSERT_UNREACHABLE_SECTION();
				return;
			}
		}
	}
}

template<typename T, std::size_t N>
template<typename PerPixelOperation>
inline void TFrame<T, N>::forEachPixel(const math::TAABB2D<uint32>& region, PerPixelOperation op) const
{
	// OPT

	PixelType pixel;
	for(uint32 y = region.getMinVertex().y(); y < region.getMaxVertex().y(); ++y)
	{
		for(uint32 x = region.getMinVertex().x(); x < region.getMaxVertex().x(); ++x)
		{
			getPixel(x, y, &pixel);

			if constexpr(frame_detail::CIsGetPixelOp<PerPixelOperation, T, N>)
			{
				op(pixel);
			}
			else if constexpr(frame_detail::CIsGetPixelWithCoordsOp<PerPixelOperation, T, N>)
			{
				op(x, y, pixel);
			}
			else
			{
				// Unsupported per pixel operation
				PH_ASSERT_UNREACHABLE_SECTION();
				return;
			}
		}
	}
}

template<typename T, std::size_t N>
inline auto TFrame<T, N>::getPixel(const math::TVector2<uint32>& coordPx) const
	-> PixelType
{
	PixelType pixel;
	getPixel(coordPx.x(), coordPx.y(), &pixel);
	return pixel;
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::getPixel(
	const uint32 x, 
	const uint32 y, 
	PixelType* const out_pixel) const
{
	PH_ASSERT(out_pixel);

	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	for(std::size_t i = 0; i < N; ++i)
	{
		PH_ASSERT_LT(baseIndex + i, m_pixelData.size());

		(*out_pixel)[i] = m_pixelData[baseIndex + i];
	}
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::setPixel(const math::TVector2<uint32>& coordPx, const PixelType& pixel)
{
	setPixel(coordPx.x(), coordPx.y(), pixel);
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::setPixel(
	const uint32 x, 
	const uint32 y, 
	const PixelType& pixel)
{
	const std::size_t baseIndex = calcPixelDataBaseIndex(x, y);

	for(std::size_t i = 0; i < N; ++i)
	{
		PH_ASSERT_LT(baseIndex + i, m_pixelData.size());

		m_pixelData[baseIndex + i] = pixel[i];
	}
}

template<typename T, std::size_t N>
inline constexpr std::size_t TFrame<T, N>::numPixelComponents() const noexcept
{
	return N;
}

template<typename T, std::size_t N>
inline TSpan<T> TFrame<T, N>::getPixelData()
{
	return m_pixelData;
}

template<typename T, std::size_t N>
inline TSpanView<T> TFrame<T, N>::getPixelData() const
{
	return m_pixelData;
}

template<typename T, std::size_t N>
inline void TFrame<T, N>::copyPixelData(const math::TAABB2D<uint32>& region, TSpan<T> out_data) const
{
	PH_ASSERT_MSG(!region.isEmpty(), region.toString());

	const auto regionDataWidth = N * region.getWidth();
	for(uint32 y = region.getMinVertex().y(); y < region.getMaxVertex().y(); ++y)
	{
		const auto srcOffset = calcPixelDataBaseIndex(region.getMinVertex().x(), y);
		const auto dstOffset = (y - region.getMinVertex().y()) * regionDataWidth;

		std::copy_n(
			m_pixelData.begin() + srcOffset,
			regionDataWidth,
			out_data.begin() + dstOffset);
	}
}

template<typename T, std::size_t N>
inline math::TVector2<uint32> TFrame<T, N>::getSizePx() const
{
	return {m_widthPx, m_heightPx};
}

template<typename T, std::size_t N>
inline uint32 TFrame<T, N>::widthPx() const
{
	return m_widthPx;
}

template<typename T, std::size_t N>
inline uint32 TFrame<T, N>::heightPx() const
{
	return m_heightPx;
}

template<typename T, std::size_t N>
inline bool TFrame<T, N>::isEmpty() const
{
	return m_pixelData.empty();
}

template<typename T, std::size_t N>
inline std::size_t TFrame<T, N>::calcPixelDataBaseIndex(
	const uint32 x, 
	const uint32 y) const
{
	PH_ASSERT_LT(x, m_widthPx);
	PH_ASSERT_LT(y, m_heightPx);

	return (y * static_cast<std::size_t>(m_widthPx) + x) * N;
}

template<typename T, std::size_t N>
inline TFrame<T, N>& TFrame<T, N>::operator = (const TFrame& rhs)
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = rhs.m_pixelData;

	return *this;
}

template<typename T, std::size_t N>
inline TFrame<T, N>& TFrame<T, N>::operator = (TFrame&& rhs) noexcept
{
	m_widthPx   = rhs.m_widthPx;
	m_heightPx  = rhs.m_heightPx;
	m_pixelData = std::move(rhs.m_pixelData);

	return *this;
}

}// end namespace ph
