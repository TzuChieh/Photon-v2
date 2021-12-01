#pragma once

#include "DataIO/Stream/IDataStream.h"

#include <cstddef>
#include <optional>

namespace ph
{

class IOutputStream : public IDataStream
{
public:
	/*! @brief Write data in the form of raw bytes in one go.
	The method does not return before finishing the writing process.
	@exception IOException If the write operation failed.
	*/
	virtual void write(std::size_t numBytes, const std::byte* bytes) = 0;

	/*! @brief Set the output position of the stream.
	The unit of the position is defined by the implementation.
	@exception IOException If the seeking process failed.
	*/
	virtual void seekPut(std::size_t pos) = 0;

	/*! @brief Get the current output position of the stream.
	The unit of the position is defined by the implementation.
	@return Current output position. Empty if the position is unavailable.
	*/
	virtual std::optional<std::size_t> tellPut() = 0;

	operator bool () const override = 0;

	/*! @brief Write some data in the form of raw bytes.
	The method may return before finish writing all bytes. In such case, the method
	returns how many bytes were actually written.
	@return How many bytes were actually written.
	@exception IOException If the write operation failed.
	*/
	virtual std::size_t writeSome(std::size_t numBytes, const std::byte* bytes);
};

// In-header Implementations:

inline std::size_t IOutputStream::writeSome(const std::size_t numBytes, const std::byte* out_bytes)
{
	write(numBytes, out_bytes);
	return numBytes;
}

}// end namespace ph
