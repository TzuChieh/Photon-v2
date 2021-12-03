#include "DataIO/Stream/ByteBufferInputStream.h"

namespace ph
{

ByteBufferInputStream::ByteBufferInputStream() :
	m_byteBuffer(),
	m_numBytes  (0),
	m_readHead  (0)
{}

ByteBufferInputStream::ByteBufferInputStream(const std::byte* const srcByteBuffer, const std::size_t numBytes) :
	ByteBufferInputStream(numBytes)
{
	PH_ASSERT(srcByteBuffer);

	std::copy(srcByteBuffer, srcByteBuffer + numBytes, byteBuffer());
}

ByteBufferInputStream::ByteBufferInputStream(const std::size_t numBytes) :
	m_byteBuffer(std::make_unique<std::byte[]>(numBytes)),
	m_numBytes  (numBytes),
	m_readHead  (0)
{}

}// end namespace ph
