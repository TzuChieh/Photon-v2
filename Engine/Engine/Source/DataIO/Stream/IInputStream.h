#pragma once

#include "DataIO/Stream/IDataStream.h"

#include <cstddef>
#include <optional>
#include <string>

namespace ph
{

class IInputStream : public IDataStream
{
public:
	/*! @brief Read specific number of raw bytes in one go.
	The method does not return before finishing the reading process.
	@exception IOException If the read operation failed.
	*/
	virtual void read(std::size_t numBytes, std::byte* out_bytes) = 0;

	/*! @brief Read a string in one go.
	Note the EOF is also considered a delimiter (the final one).
	@param out_string The read string. Does not include the @p delimiter.
	@param delimiter The character that denotes the ending of a line.
	@exception IOException If the read operation failed.
	*/
	virtual void readString(std::string* out_string, char delimiter) = 0;

	/*! @brief Set the input position of the stream.
	The unit of the position is defined by the implementation.
	@exception IOException If the seeking process failed.
	*/
	virtual void seekGet(std::size_t pos) = 0;

	/*! @brief Get the current input position of the stream.
	The unit of the position is defined by the implementation.
	@return Current input position. Empty if the position is unavailable.
	*/
	virtual std::optional<std::size_t> tellGet() = 0;

	/*!
	@return Whether the stream is good for read.
	*/
	operator bool () const override = 0;

	/*! @brief Read some data in the form of raw bytes.
	The method may return before finish reading all bytes. In such case, the method
	returns how many bytes were actually read.
	@return How many bytes were actually read.
	@exception IOException If the read operation failed.
	*/
	virtual std::size_t readSome(std::size_t numBytes, std::byte* out_bytes);

	/*! @brief Read a line. Equivalent to calling readString(std::string*, char) with '\n' as 
	the delimiter.
	@param out_string The read string. Does not include the new-line character.
	@exception IOException If the read operation failed.
	*/
	void readLine(std::string* out_string);
};

// In-header Implementations:

inline std::size_t IInputStream::readSome(const std::size_t numBytes, std::byte* const out_bytes)
{
	read(numBytes, out_bytes);
	return numBytes;
}

inline void IInputStream::readLine(std::string* const out_string)
{
	readString(out_string, '\n');
}

}// end namespace ph
