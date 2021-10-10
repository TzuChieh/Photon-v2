#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"

#include <cstddef>
#include <array>
#include <utility>
#include <stdexcept>
#include <format>
#include <limits>

namespace ph
{

namespace pixel_buffer
{

enum class EPixelType
{
	PT_Unspecified = 0,

	PT_float32,
	PT_float64,
	PT_int32,
	PT_int64,
	PT_uint8
};

inline constexpr uint8 MAX_PIXEL_ELEMENTS = 4;

template<typename T>
class TPixel final
{
public:
	TPixel();

	template<std::size_t N>
	TPixel(const std::array<T, N>& values);

	template<typename U>
	explicit TPixel(const TPixel<U>& other);

	template<std::size_t N>
	std::array<T, N> getValues() const;

	std::size_t numValues() const;
	T operator [] (std::size_t index) const;

private:
	using ValueArrayType = std::array<T, MAX_PIXEL_ELEMENTS>;

	ValueArrayType m_valueBuffer;
	std::size_t    m_numValues;
};

// In-header Implementations:

template<typename T>
inline TPixel<T>::TPixel() :
	m_valueBuffer(),
	m_numValues  (0)
{}

template<typename T>
template<std::size_t N>
inline TPixel<T>::TPixel(const std::array<T, N>& values) :
	TPixel()
{
	static_assert(N <= std::tuple_size_v<ValueArrayType>,
		"Number of input values overflow the internal buffer.");

	for(std::size_t i = 0; i < N; ++i)
	{
		m_valueBuffer[i] = values[i];
	}

	m_numValues = N;
}

template<typename T>
template<typename U>
inline TPixel<T>::TPixel(const TPixel<U>& other) :
	TPixel()
{
	for(std::size_t i = 0; i < other.numValues(); ++i)
	{
		m_valueBuffer[i] = static_cast<T>(other[i]);
	}

	m_numValues = other.numValues();
}

template<typename T>
template<std::size_t N>
inline auto TPixel<T>::getValues() const
-> std::array<T, N>
{
	if(N > m_numValues)
	{
		throw std::out_of_range(std::format(
			"Attepmting to get {} input values from an internal buffer of size {}", N, m_numValues));
	}

	std::array<T, N> values;
	for(std::size_t i = 0; i < N; ++i)
	{
		values[i] = m_valueBuffer[i];
	}
	return values;
}

template<typename T>
inline std::size_t TPixel<T>::numValues() const
{
	return m_numValues;
}

template<typename T>
inline T TPixel<T>::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_numValues);

	return m_valueBuffer[index];
}

}// end namespace pixel_buffer

class PixelBuffer2D
{
public:
	PixelBuffer2D(
		math::TVector2<uint32>   size,
		std::size_t              numPixelElements,
		pixel_buffer::EPixelType pixelType);

	PixelBuffer2D(
		math::TVector2<uint32>   size, 
		std::size_t              numPixelElements,
		pixel_buffer::EPixelType pixelType,
		std::size_t              numMipLevels);

	virtual pixel_buffer::TPixel<float32> fetchPixel(math::TVector2<uint32> xy, std::size_t mipLevel) const = 0;
	virtual pixel_buffer::TPixel<float64> fetchF64Pixel(math::TVector2<uint32> xy, std::size_t mipLevel) const;

	math::TVector2<uint32> getSize() const;
	std::size_t numPixelElements() const;
	pixel_buffer::EPixelType getPixelType() const;
	bool hasMipmap() const;
	std::size_t numMipLevels() const;

private:
	math::TVector2<uint32>   m_size;
	uint8                    m_numPixelElements;
	pixel_buffer::EPixelType m_pixelType;
	uint8                    m_numMipLevels;
};

// In-header Implementations:

inline PixelBuffer2D::PixelBuffer2D(
	const math::TVector2<uint32>   size,
	const std::size_t              numPixelElements,
	const pixel_buffer::EPixelType pixelType) :

	PixelBuffer2D(
		size,
		numPixelElements,
		pixelType,
		1)
{}

inline PixelBuffer2D::PixelBuffer2D(
	const math::TVector2<uint32>   size,
	const std::size_t              numPixelElements,
	const pixel_buffer::EPixelType pixelType,
	const std::size_t              numMipLevels) :

	m_size            (size),
	m_numPixelElements(static_cast<uint8>(numPixelElements)),
	m_pixelType       (pixelType),
	m_numMipLevels    (static_cast<uint8>(numMipLevels))
{
	// No 0-sized buffer and unspecified pixel type
	PH_ASSERT_GT(size.product(), 0);
	PH_ASSERT_GT(numPixelElements, 0);
	PH_ASSERT(pixelType != pixel_buffer::EPixelType::PT_Unspecified);
	PH_ASSERT_GE(m_numMipLevels, 1);

	// Check for possible value overflow
	PH_ASSERT_LE(numPixelElements, pixel_buffer::MAX_PIXEL_ELEMENTS);
	PH_ASSERT_LE(numMipLevels, std::numeric_limits<decltype(m_numMipLevels)>::max());
}

inline math::TVector2<uint32> PixelBuffer2D::getSize() const
{
	return m_size;
}

inline bool PixelBuffer2D::hasMipmap() const
{
	return m_numMipLevels > 1;
}

inline std::size_t PixelBuffer2D::numPixelElements() const
{
	return m_numPixelElements;
}

inline pixel_buffer::EPixelType PixelBuffer2D::getPixelType() const
{
	return m_pixelType;
}

inline std::size_t PixelBuffer2D::numMipLevels() const
{
	return m_numMipLevels;
}

inline pixel_buffer::TPixel<float64> PixelBuffer2D::fetchF64Pixel(const math::TVector2<uint32> xy, const std::size_t mipLevel) const
{
	return pixel_buffer::TPixel<float64>(fetchPixel(xy, mipLevel));
}

}// end namespace ph
