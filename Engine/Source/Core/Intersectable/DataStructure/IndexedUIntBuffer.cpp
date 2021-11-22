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
	m_byteBufferSize = numUInts * m_numBitsPerUInt;
	m_byteBuffer = std::make_unique<std::byte[]>(m_byteBufferSize);

	if(m_byteBufferSize == 0)
	{
		PH_LOG_WARNING(IndexedUIntBuffer, "Allocated buffer with 0 size.");
	}
}

std::size_t IndexedUIntBuffer::getUInt(const std::size_t index) const
{

}

}// end namespace ph
