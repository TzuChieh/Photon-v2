#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <cstddef>
#include <climits>
#include <memory>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <format>
#include <cstring>
#include <algorithm>

namespace ph
{

class IndexedUIntBuffer final
{
	static_assert(sizeof(std::byte)* CHAR_BIT == 8,
		"The buffer explicitly depends on the fact that std::byte contains 8 bits.");

public:
	IndexedUIntBuffer();

	void setUIntFormat(uint8 numBitsPerUInt);
	void setUIntFormatByMaxValue(uint64 maxValue);
	void allocate(std::size_t numUInts);

	template<std::integral IntegerType>
	void setUInt(std::size_t index, IntegerType value);

	template<typename IntegerType>
	void setUInts(std::size_t beginIndex, std::size_t endIndex, IntegerType* values);

	// TODO: templatize
	uint64 getUInt(std::size_t index) const;

	std::size_t numUInts() const;
	std::size_t estimateMemoryUsageBytes() const;
	bool isAllocated() const;
	uint64 getMaxAllowedValue() const;

private:
	static uint64 maxAllowedValue(uint8 numBitsPerUInt);

	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t                  m_byteBufferSize;
	uint8                        m_numBitsPerUInt;
};

// In-header Implementations:

inline std::size_t IndexedUIntBuffer::estimateMemoryUsageBytes() const
{
	return sizeof(IndexedUIntBuffer) + m_byteBufferSize;
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

	const auto uint64Value = static_cast<uint64>(value);
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

	// Handle situations where the value needs the 9-th byte (straddles next byte)
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

template<typename IntegerType>
inline void IndexedUIntBuffer::setUInts(
	const std::size_t  beginIndex, 
	const std::size_t  endIndex, 
	IntegerType* const values)
{
	if(endIndex <= beginIndex)
	{
		return;
	}

	PH_ASSERT(values);
	for(std::size_t uintIdx = beginIndex, valueIdx = 0; uintIdx < endIndex; ++uintIdx, ++valueIdx)
	{
		PH_ASSERT_LT(valueIdx, endIndex - beginIndex);
		setUInt<IntegerType>(uintIdx, values[valueIdx]);
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

	// Handle situations where the value needs the 9-th byte (straddles next byte)
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

}// end namespace ph
