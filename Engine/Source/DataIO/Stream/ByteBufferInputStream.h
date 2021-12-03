#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <format>

namespace ph
{

/*! @brief An in-memory byte stream designed for I/O performance.
*/
class ByteBufferInputStream : public IInputStream
{
public:
	ByteBufferInputStream();
	ByteBufferInputStream(const std::byte* srcByteBuffer, std::size_t numBytes);
	explicit ByteBufferInputStream(std::size_t numBytes);
	inline ByteBufferInputStream(ByteBufferInputStream&& other) = default;

	void read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::optional<std::size_t> tellGet() override;
	operator bool() const override;
	std::size_t readSome(std::size_t numBytes, std::byte* out_bytes) override;

	template<typename T>
	void readData(T* out_data);

	/*! @brief Get the size of the underlying byte buffer.
	*/
	std::size_t numBufferBytes() const;

	/*! @brief Direct access to the underlying byte buffer.
	*/
	std::byte* byteBuffer();

	inline ByteBufferInputStream& operator = (ByteBufferInputStream&& rhs) = default;

protected:
	bool canRead(std::size_t numBytes) const;

private:
	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t                  m_numBytes;
	std::size_t                  m_readHead;
};

// In-header Implementations:

inline void ByteBufferInputStream::read(const std::size_t numBytes, std::byte* const out_bytes)
{
	PH_ASSERT(out_bytes);

	if(canRead(numBytes))
	{
		PH_ASSERT_LT(m_readHead, numBufferBytes());
		PH_ASSERT_LE(m_readHead + numBytes, numBufferBytes());

		std::copy(
			byteBuffer() + m_readHead, 
			byteBuffer() + m_readHead + numBytes,
			out_bytes);

		m_readHead += numBytes;
	}
	else
	{
		throw IOException(std::format(
			"Attempt to read bytes in [{}, {}) which overflows [{}, {}).",
			m_readHead, m_readHead + numBytes, m_readHead, numBufferBytes()));
	}
}

inline void ByteBufferInputStream::seekGet(const std::size_t pos)
{
	if(pos < numBufferBytes())
	{
		m_readHead = pos;
	}
	else
	{
		throw IOException(std::format(
			"Attempt to seek to read position {} which overflows [0, {}).",
			pos, numBufferBytes()));
	}
}

inline std::optional<std::size_t> ByteBufferInputStream::tellGet()
{
	return m_readHead;
}

inline std::size_t ByteBufferInputStream::readSome(const std::size_t numBytes, std::byte* const out_bytes)
{
	PH_ASSERT(out_bytes);

	if(canRead(numBytes))
	{
		read(numBytes, out_bytes);
		return numBytes;
	}
	else
	{
		PH_ASSERT_LE(m_readHead, numBufferBytes());
		const auto numRemainingBytes = numBufferBytes() - m_readHead;

		PH_ASSERT_LT(numRemainingBytes, numBytes);
		read(numRemainingBytes, out_bytes);
		return numRemainingBytes;
	}
}

inline bool ByteBufferInputStream::canRead(const std::size_t numBytes) const
{
	PH_ASSERT(
		m_byteBuffer ||
		(!m_byteBuffer && m_readHead == 0 && numBufferBytes() == 0));

	// For empty buffer, the only allowed <numBytes> to read is 0
	return m_readHead + numBytes <= numBufferBytes();
}

inline ByteBufferInputStream::operator bool() const
{
	PH_ASSERT(
		m_byteBuffer || 
		(!m_byteBuffer && m_readHead == 0 && numBufferBytes() == 0));

	// For empty buffer, both <m_readHead> and <numBufferBytes()> should be 0 which results in `false`
	return m_readHead < numBufferBytes();
}

template<typename T>
inline void ByteBufferInputStream::readData(T* const out_data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data);

	read(sizeof(T), reinterpret_cast<std::byte*>(out_data));
}

inline std::size_t ByteBufferInputStream::numBufferBytes() const
{
	return m_numBytes;
}

inline std::byte* ByteBufferInputStream::byteBuffer()
{
	PH_ASSERT(m_byteBuffer);
	return m_byteBuffer.get();
}

}// end namespace ph
