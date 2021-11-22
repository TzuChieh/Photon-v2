#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>
#include <climits>
#include <memory>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <format>

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

	std::size_t getUInt(std::size_t index) const;
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

	const auto unsignedValue = static_cast<uint64>(value);
	if(unsignedValue > getMaxAllowedValue())
	{
		throw std::invalid_argument(std::format(
			"Integer value {} cannot be stored using {} bits.", value, m_numBitsPerUInt));
	}

	const auto firstByteIndex = index * m_numBitsPerUInt / CHAR_BIT;
	const auto firstBitOffset = index * m_numBitsPerUInt - firstByteIndex * CHAR_BIT;

	// TODO:
}

}// end namespace ph
