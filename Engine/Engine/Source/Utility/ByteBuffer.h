#pragma once

#include "Utility/TSpan.h"

#include <Common/assertion.h>

#include <vector>
#include <cstddef>
#include <algorithm>
#include <iterator>

namespace ph
{

/*! @brief An auto-resizable byte storage.
*/
class ByteBuffer final
{
public:
	ByteBuffer();
	explicit ByteBuffer(std::size_t numExpectedWrittenBytes);

	template<typename T>
	T read();

	template<typename T>
	void read(T* out_data, std::size_t numElements);

	/*! @brief Writes data to the buffer. Grows the buffer if no space left.
	*/
	template<typename T>
	void write(const T& data);

	/*! @brief Writes data to the buffer. Grows the buffer if no space left.
	*/
	template<typename T>
	void write(const T* data, std::size_t numElements);

	void rewindRead();
	void rewindWrite();

	/*! @brief Clear all data and reset reader and writer positions.
	*/
	void clear();

	void fill(std::byte filledByte);
	void truncate();
	void fit();
	void setReadPosition(std::size_t indexOfByte);
	void setWritePosition(std::size_t indexOfByte);
	void setNumBytes(std::size_t numBytes);

	/*!
	@return The index that is going to be read next.
	*/
	std::size_t getReadPosition() const;

	/*!
	@return The index that is going to be written next.
	*/
	std::size_t getWritePosition() const;

	/*! @brief Direct access to all the bytes in the buffer.
	*/
	///@{
	TSpan<std::byte> getBytes();
	TSpanView<std::byte> getBytes() const;
	///@}

	std::size_t numBytes() const;
	bool isEmpty() const;
	bool hasMoreToRead() const;

private:
	std::vector<std::byte> m_buffer;
	std::size_t            m_readHead;
	std::size_t            m_writeHead;
};

// In-header Implementations:

inline ByteBuffer::ByteBuffer() :
	m_buffer(),
	m_readHead(0),
	m_writeHead(0)
{}

inline ByteBuffer::ByteBuffer(std::size_t numExpectedWrittenBytes) :
	m_buffer(),
	m_readHead(0),
	m_writeHead(0)
{
	m_buffer.reserve(numExpectedWrittenBytes);
}

template<typename T>
inline T ByteBuffer::read()
{
	T data;
	read(&data, 1);
	return data;
}

template<typename T>
inline void ByteBuffer::read(T* const out_data, const std::size_t numElements)
{
	static_assert(std::is_trivially_copyable_v<T>,
		"reading the type from ByteBuffer is not allowed");

	PH_ASSERT(out_data);
	PH_ASSERT_LE(m_readHead, m_buffer.size());

	const std::size_t numRemainingBytes = m_buffer.size() - m_readHead;
	const std::size_t numDataBytes      = std::min(sizeof(T) * numElements, numRemainingBytes);

	std::copy_n(
		m_buffer.data() + m_readHead, 
		numDataBytes, 
		reinterpret_cast<std::byte*>(out_data));

	m_readHead += numDataBytes;

	PH_ASSERT_LE(m_readHead, m_buffer.size());
}

template<typename T>
inline void ByteBuffer::write(const T& data)
{
	write(&data, 1);
}

template<typename T>
inline void ByteBuffer::write(const T* const data, const std::size_t numElements)
{
	static_assert(std::is_trivially_copyable_v<T>,
		"writing the type into ByteBuffer is not allowed");

	PH_ASSERT(data);
	PH_ASSERT_LE(m_writeHead, m_buffer.size());

	const std::size_t numDataBytes      = sizeof(T) * numElements;
	const std::size_t numAvailableBytes = m_buffer.size() - m_writeHead;

	// Existing space is enough for writing
	if(numAvailableBytes >= numDataBytes)
	{
		std::copy_n(
			reinterpret_cast<const std::byte*>(data), 
			numDataBytes, 
			m_buffer.data() + m_writeHead);
	}
	else
	{
		// So existing space is not enough

		const std::size_t numShortedBytes = numDataBytes - numAvailableBytes;

		// First copy into what we have left
		std::copy_n(
			reinterpret_cast<const std::byte*>(data),
			numAvailableBytes, 
			m_buffer.data() + m_writeHead);

		// Then copy remaining bytes into the buffer (and let the buffer grow by itself)
		m_buffer.insert(
			m_buffer.end(), 
			reinterpret_cast<const std::byte*>(data) + numAvailableBytes, 
			reinterpret_cast<const std::byte*>(data) + numDataBytes);
	}

	m_writeHead += numDataBytes;

	PH_ASSERT_LE(m_writeHead, m_buffer.size());
}

inline void ByteBuffer::rewindRead()
{
	m_readHead = 0;
}

inline void ByteBuffer::rewindWrite()
{
	m_writeHead = 0;
}

inline void ByteBuffer::clear()
{
	m_buffer.clear();

	rewindRead();
	rewindWrite();
}

inline void ByteBuffer::fill(const std::byte filledByte)
{
	std::fill(m_buffer.begin(), m_buffer.end(), filledByte);

	m_writeHead = m_buffer.size();
}

inline void ByteBuffer::truncate()
{
	PH_ASSERT_LE(m_writeHead, m_buffer.size());

	m_buffer.resize(m_writeHead);

	if(m_readHead > m_writeHead)
	{
		setReadPosition(m_writeHead);
	}
}

inline void ByteBuffer::fit()
{
	m_buffer.shrink_to_fit();
}

inline void ByteBuffer::setReadPosition(const std::size_t indexOfByte)
{
	PH_ASSERT_LE(indexOfByte, m_buffer.size());

	m_readHead = indexOfByte;
}

inline void ByteBuffer::setWritePosition(const std::size_t indexOfByte)
{
	PH_ASSERT_LE(indexOfByte, m_buffer.size());

	m_writeHead = indexOfByte;
}

inline void ByteBuffer::setNumBytes(const std::size_t numBytes)
{
	m_buffer.resize(numBytes);

	if(m_writeHead > numBytes)
	{
		setWritePosition(numBytes);
	}

	if(m_readHead > numBytes)
	{
		setReadPosition(numBytes);
	}
}

inline std::size_t ByteBuffer::getReadPosition() const
{
	return m_readHead;
}

inline std::size_t ByteBuffer::getWritePosition() const
{
	return m_writeHead;
}

inline TSpan<std::byte> ByteBuffer::getBytes()
{
	return m_buffer;
}

inline TSpanView<std::byte> ByteBuffer::getBytes() const
{
	return m_buffer;
}

inline std::size_t ByteBuffer::numBytes() const
{
	return m_buffer.size();
}

inline bool ByteBuffer::isEmpty() const
{
	return m_buffer.empty();
}

inline bool ByteBuffer::hasMoreToRead() const
{
	return getReadPosition() < numBytes();
}

}// end namespace ph
