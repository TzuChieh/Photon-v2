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

void IndexedUIntBuffer::declareUIntFormat(const uint8 numBitsPerUInt)
{
	m_numBitsPerUInt = numBitsPerUInt;
}

void IndexedUIntBuffer::declareUIntFormatByMaxValue(const uint64 maxValue)
{
	for(uint8 numBitsPerUInt = 1; numBitsPerUInt <= 64; ++numBitsPerUInt)
	{
		if(maxAllowedValue(numBitsPerUInt) >= maxValue)
		{
			declareUIntFormat(numBitsPerUInt);
			return;
		}
	}

	PH_LOG_WARNING(IndexedUIntBuffer, 
		"Unable to store integer value {}, max allowed value is {}", maxValue, maxAllowedValue(64));
	declareUIntFormat(64);
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

void IndexedUIntBuffer::setUInts(const std::byte* const srcBytes, const std::size_t numBytes, const std::size_t dstOffset)
{
	PH_ASSERT(srcBytes);
	PH_ASSERT(isAllocated());

	if(dstOffset + numBytes > m_byteBufferSize)
	{
		throw std::invalid_argument(std::format(
			"Copying {} bytes will overflow the index buffer (buffer-size: {} bytes, buffer-offset: {}).",
			numBytes, numBytes, dstOffset));
	}

	std::memcpy(&(m_byteBuffer[dstOffset]), srcBytes, numBytes);
}

}// end namespace ph
