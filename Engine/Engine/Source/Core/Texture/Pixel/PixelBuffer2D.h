#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"

#include <cstddef>
#include <array>
#include <limits>
#include <stdexcept>
#include <format>
#include <algorithm>

namespace ph
{

namespace pixel_buffer
{

inline constexpr uint8 MAX_PIXEL_ELEMENTS = 4;

/*! @brief Represent a pixel from pixel buffer.
*/
template<typename T>
class TPixel final
{
public:
	TPixel();

	/*! @brief Make a pixel from an array of values.
	Number of pixel values is inferred by the array dimension.
	*/
	template<std::size_t N>
	TPixel(const std::array<T, N>& values);

	/*! @brief Make a pixel from values in a buffer.
	@param numUsedValues Number of pixel values to extract from @p valueBuffer.
	*/
	template<std::size_t N>
	TPixel(const std::array<T, N>& valueBuffer, std::size_t numUsedValues);

	template<typename U>
	explicit TPixel(const TPixel<U>& other);

	template<std::size_t N>
	std::array<T, N> getValues() const;

	/*!
	Values other than the stored values (i.e., `index >= numValues`) will be 0.
	*/
	std::array<T, MAX_PIXEL_ELEMENTS> getAllValues() const;

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
	m_valueBuffer(),// init all values to 0
	m_numValues  (0)
{}

template<typename T>
template<std::size_t N>
inline TPixel<T>::TPixel(const std::array<T, N>& values) :
	TPixel(values, N)
{}

template<typename T>
template<std::size_t N>
inline TPixel<T>::TPixel(const std::array<T, N>& valueBuffer, const std::size_t numUsedValues) :
	TPixel()
{
	constexpr auto MIN_BUFFER_SIZE = std::min(N, std::tuple_size_v<ValueArrayType>);

	if(numUsedValues > MIN_BUFFER_SIZE)
	{
		throw std::out_of_range(std::format(
			"Buffer of size {} cannot hold {} input values.", MIN_BUFFER_SIZE, numUsedValues));
	}

	for(std::size_t i = 0; i < numUsedValues; ++i)
	{
		m_valueBuffer[i] = valueBuffer[i];
	}

	m_numValues = numUsedValues;
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
inline std::array<T, N> TPixel<T>::getValues() const
{
	if(N > m_numValues)
	{
		throw std::out_of_range(std::format(
			"Attepmting to get {} input values from an internal buffer of size {}.", N, m_numValues));
	}

	std::array<T, N> values;
	for(std::size_t i = 0; i < N; ++i)
	{
		values[i] = m_valueBuffer[i];
	}
	return values;
}

template<typename T>
inline std::array<T, MAX_PIXEL_ELEMENTS> TPixel<T>::getAllValues() const
{
	return m_valueBuffer;
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
		math::TVector2<uint32> size,
		std::size_t            numPixelElements);

	PixelBuffer2D(
		math::TVector2<uint32> size, 
		std::size_t            numPixelElements,
		std::size_t            numMipLevels);

	inline virtual ~PixelBuffer2D() = default;

	virtual pixel_buffer::TPixel<float64> fetchPixel(math::TVector2<uint32> xy, std::size_t mipLevel) const = 0;
	
	virtual std::size_t estimateMemoryUsageBytes() const;

	math::TVector2<uint32> getSize() const;
	std::size_t numPixelElements() const;
	bool hasMipmap() const;
	std::size_t numMipLevels() const;

private:
	math::TVector2<uint32> m_size;
	uint8                  m_numPixelElements;
	uint8                  m_numMipLevels;
};

// In-header Implementations:

inline PixelBuffer2D::PixelBuffer2D(
	const math::TVector2<uint32> size,
	const std::size_t            numPixelElements) :

	PixelBuffer2D(
		size,
		numPixelElements,
		1)
{}

inline PixelBuffer2D::PixelBuffer2D(
	const math::TVector2<uint32> size,
	const std::size_t            numPixelElements,
	const std::size_t            numMipLevels) :

	m_size            (size),
	m_numPixelElements(static_cast<uint8>(numPixelElements)),
	m_numMipLevels    (static_cast<uint8>(numMipLevels))
{
	// No 0-sized buffer and unspecified pixel type
	PH_ASSERT_GT(size.product(), 0);
	PH_ASSERT_GT(numPixelElements, 0);
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

inline std::size_t PixelBuffer2D::numMipLevels() const
{
	return m_numMipLevels;
}

inline std::size_t PixelBuffer2D::estimateMemoryUsageBytes() const
{
	return sizeof(PixelBuffer2D);
}

}// end namespace ph
