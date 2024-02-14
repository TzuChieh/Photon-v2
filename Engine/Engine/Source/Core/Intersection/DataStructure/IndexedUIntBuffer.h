#pragma once

#include "Math/math.h"
#include "Utility/utility.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <climits>
#include <memory>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <format>
#include <cstring>
#include <algorithm>
#include <type_traits>

namespace ph
{

/*! @brief A general unsigned integer buffer for integers with any number of bits.
*/
class IndexedUIntBuffer final
{
	static_assert(sizeof(std::byte)* CHAR_BIT == 8,
		"The buffer explicitly depends on the fact that std::byte contains 8 bits.");

public:
	IndexedUIntBuffer();

	// TODO: aligned memory allocation?

	void declareUIntFormat(std::size_t numBitsPerUInt);
	void declareUIntFormatByMaxValue(uint64 maxValue);

	template<std::integral IntegerType>
	void declareUIntFormat();

	void allocate(std::size_t numUInts);

	/*! @brief Set a single integer.
	*/
	template<std::integral IntegerType>
	void setUInt(std::size_t index, IntegerType value);

	/*! @brief Set integers to a range of indices.
	This method handles all types, including both integer and floating-point types. If `ValueType`
	matches the buffer's integer format (unsigned and with same bit count), it will be a direct
	memory copy.
	*/
	template<typename ValueType>
	void setUInts(const ValueType* values, std::size_t numValues, std::size_t dstBeginValueIndex = 0);

	void setUInts(const std::byte* srcBytes, std::size_t numBytes, std::size_t dstOffset = 0);

	// TODO: templatize
	uint64 getUInt(std::size_t index) const;

	std::size_t numUInts() const;
	std::size_t memoryUsage() const;
	bool isAllocated() const;

	/*!
	@return The largest possible integer that can be stored in this buffer.
	*/
	uint64 getMaxAllowedValue() const;

	/*! @brief Access to the underlying raw byte buffer.
	*/
	///@{
	std::byte* getData();
	const std::byte* getData() const;
	///@}

private:
	static uint64 maxAllowedValue(uint8 numBitsPerUInt);

	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t                  m_byteBufferSize;
	uint8                        m_numBitsPerUInt;
};

// In-header Implementations:

template<std::integral IntegerType>
inline void IndexedUIntBuffer::declareUIntFormat()
{
	declareUIntFormat(sizeof_in_bits<IntegerType>());
}

inline std::size_t IndexedUIntBuffer::memoryUsage() const
{
	return sizeof(*this) + m_byteBufferSize;
}

inline bool IndexedUIntBuffer::isAllocated() const
{
	return m_byteBuffer != nullptr;
}

inline uint64 IndexedUIntBuffer::getMaxAllowedValue() const
{
	return maxAllowedValue(m_numBitsPerUInt);
}

inline std::size_t IndexedUIntBuffer::numUInts() const
{
	return m_numBitsPerUInt > 0 ? m_byteBufferSize * CHAR_BIT / m_numBitsPerUInt : 0;
}

inline uint64 IndexedUIntBuffer::maxAllowedValue(const uint8 numBitsPerUInt)
{
	return numBitsPerUInt < 64
		? (static_cast<uint64>(1) << numBitsPerUInt) - 1
		: std::numeric_limits<uint64>::max();
}

template<std::integral IntegerType>
inline void IndexedUIntBuffer::setUInt(const std::size_t index, const IntegerType value)
{
	PH_ASSERT(isAllocated());

	const auto uint64Value = lossless_integer_cast<uint64>(value);
	if(uint64Value > getMaxAllowedValue())
	{
		throw std::invalid_argument(std::format(
			"Integer value {} cannot be stored using {} bits.", value, m_numBitsPerUInt));
	}

	const std::size_t firstByteIndex     = index * m_numBitsPerUInt / CHAR_BIT;
	const std::size_t firstByteBitOffset = index * m_numBitsPerUInt - firstByteIndex * CHAR_BIT;
	const std::size_t numStraddledBytes  = (firstByteBitOffset + m_numBitsPerUInt + (CHAR_BIT - 1)) / CHAR_BIT;

	PH_ASSERT_LT(firstByteBitOffset, CHAR_BIT);
	PH_ASSERT_LE(numStraddledBytes, 8 + 1);
	PH_ASSERT_LE(firstByteIndex + numStraddledBytes, m_byteBufferSize);

	// Potentially read straddled previous & next values' bits
	uint64 rawBits = 0;
	std::memcpy(&rawBits, &m_byteBuffer[firstByteIndex], std::min<std::size_t>(numStraddledBytes, 8));

	// Store value between the back of previous value and the head of next value
	rawBits = math::clear_bits_in_range(rawBits, firstByteBitOffset, firstByteBitOffset + m_numBitsPerUInt);
	rawBits |= (uint64Value << firstByteBitOffset);
	std::memcpy(&m_byteBuffer[firstByteIndex], &rawBits, std::min<std::size_t>(numStraddledBytes, 8));

	// Handle situations where the value needs the 9-th byte (straddles next byte), this can happen
	// since we support any number of bits per index
	if(numStraddledBytes > 8)
	{
		uint8 remainingRawBits;
		std::memcpy(&remainingRawBits, &m_byteBuffer[firstByteIndex + 8], 1);

		// Store the remaining value before next value
		remainingRawBits = math::clear_bits_in_range(remainingRawBits, static_cast<std::size_t>(0), firstByteBitOffset + m_numBitsPerUInt - 64);
		remainingRawBits |= static_cast<uint8>(uint64Value >> (64 - firstByteBitOffset));
		std::memcpy(&m_byteBuffer[firstByteIndex + 8], &remainingRawBits, 1);
	}
}

template<typename ValueType>
inline void IndexedUIntBuffer::setUInts(
	const ValueType* const values,
	const std::size_t      numValues,
	const std::size_t      dstBeginValueIndex)
{
	PH_ASSERT(values);

	// Directly copy the value buffer if the formats matched
	if(std::is_unsigned_v<ValueType> && sizeof_in_bits<ValueType>() == m_numBitsPerUInt)
	{
		setUInts(
			reinterpret_cast<const std::byte*>(values), 
			numValues * sizeof(ValueType), 
			dstBeginValueIndex * sizeof(ValueType));
	}
	// Resort to copy one by one
	else
	{
		for(std::size_t uintIdx = dstBeginValueIndex, valueIdx = 0; valueIdx < numValues; ++uintIdx, ++valueIdx)
		{
			if constexpr(std::is_integral_v<ValueType>)
			{
				setUInt(uintIdx, values[valueIdx]);
			}
			else
			{
				setUInt(uintIdx, static_cast<uint64>(values[valueIdx]));
			}
		}
	}
}

inline uint64 IndexedUIntBuffer::getUInt(const std::size_t index) const
{
	PH_ASSERT(isAllocated());

	const std::size_t firstByteIndex     = index * m_numBitsPerUInt / CHAR_BIT;
	const std::size_t firstByteBitOffset = index * m_numBitsPerUInt - firstByteIndex * CHAR_BIT;
	const std::size_t numStraddledBytes  = (firstByteBitOffset + m_numBitsPerUInt + (CHAR_BIT - 1)) / CHAR_BIT;

	PH_ASSERT_LT(firstByteBitOffset, CHAR_BIT);
	PH_ASSERT_LE(numStraddledBytes, 8 + 1);
	PH_ASSERT_LE(firstByteIndex + numStraddledBytes, m_byteBufferSize);

	// Read current value's bits (first 8 bytes, at most)
	uint64 rawBits = 0;
	std::memcpy(&rawBits, &m_byteBuffer[firstByteIndex], std::min<std::size_t>(numStraddledBytes, 8));

	// Clear previous and next values' bits if any, then get the value
	const auto bitMask = math::set_bits_in_range<uint64>(0, firstByteBitOffset, firstByteBitOffset + m_numBitsPerUInt);
	uint64 value = (rawBits & bitMask) >> firstByteBitOffset;

	// Handle situations where the value needs the 9-th byte (straddles next byte), this can happen
	// since we support any number of bits per index
	if(numStraddledBytes > 8)
	{
		uint8 remainingRawBits;
		std::memcpy(&remainingRawBits, &m_byteBuffer[firstByteIndex + 8], 1);

		// Extract the remaining value and clear next value's bits if any
		const auto remainingBitsBitMask = math::set_bits_in_range<uint8>(0, static_cast<std::size_t>(0), firstByteBitOffset + m_numBitsPerUInt - 64);
		remainingRawBits &= remainingBitsBitMask;

		// Add the remaining bits to the value
		value |= (static_cast<uint64>(remainingRawBits) << (64 - firstByteBitOffset));
	}

	return value;
}

inline std::byte* IndexedUIntBuffer::getData()
{
	PH_ASSERT(isAllocated());
	return m_byteBuffer.get();
}

inline const std::byte* IndexedUIntBuffer::getData() const
{
	PH_ASSERT(isAllocated());
	return m_byteBuffer.get();
}

}// end namespace ph
