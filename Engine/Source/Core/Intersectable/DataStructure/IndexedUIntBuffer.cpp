#include "Core/Intersectable/DataStructure/IndexedUIntBuffer.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IndexedUIntBuffer, Core);

IndexedUIntBuffer::IndexedUIntBuffer() :
	m_byteBuffer    (nullptr),
	m_byteBufferSize(0),
	m_numBitsPerUInt(0)
{}

void IndexedUIntBuffer::setUIntFormat(const uint8 numBitsPerUInt)
{
	m_numBitsPerUInt = numBitsPerUInt;
}

void IndexedUIntBuffer::setUIntFormatByMaxValue(const uint64 maxValue)
{
	for(uint8 numBitsPerUInt = 1; numBitsPerUInt <= 64; ++numBitsPerUInt)
	{
		if(maxAllowedValue(numBitsPerUInt) >= maxValue)
		{
			setUIntFormat(numBitsPerUInt);
			return;
		}
	}

	PH_LOG_WARNING(IndexedUIntBuffer, 
		"Unable to store integer value {}, max allowed value is {}", maxValue, maxAllowedValue(64));
	setUIntFormat(64);
}

void IndexedUIntBuffer::allocate(const std::size_t numUInts)
{
	// Allocate storage for the uints

	// Possibly clear existing buffer first to reduce memory usage
	m_byteBuffer = nullptr;

	m_numBitsPerUInt = m_numBitsPerUInt > 0 ? m_numBitsPerUInt : 32;
	m_byteBufferSize = (numUInts * m_numBitsPerUInt + (CHAR_BIT - 1)) / CHAR_BIT;
	m_byteBuffer = std::make_unique<std::byte[]>(m_byteBufferSize);

	if(m_byteBufferSize == 0)
	{
		PH_LOG_WARNING(IndexedUIntBuffer, "Allocated buffer with 0 size.");
	}
}

uint64 IndexedUIntBuffer::getUInt(const std::size_t index) const
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
