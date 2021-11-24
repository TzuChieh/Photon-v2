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
public:
	IndexedUIntBuffer();

	void setUIntFormat(uint8 numBitsPerUInt);
	void setUIntFormatByMaxValue(uint64 maxValue);
	void allocate(std::size_t numUInts);

	template<std::integral IntegerType>
	void setUInt(IntegerType value, std::size_t index);

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
inline void IndexedUIntBuffer::setUInt(const IntegerType value, const std::size_t index)
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

}// end namespace ph
