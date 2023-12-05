#include "DataIO/Stream/StdInputStream.h"
#include "DataIO/io_exceptions.h"

#include <Common/assertion.h>
#include <Common/os.h>

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cctype>
#include <algorithm>
#include <cerrno>
#include <cstring>

/*
Note on the implementation:

One might want to enable exceptions by setting std::istream::exceptions(). However, many operations 
depend on the detection of EOF, e.g., std::istream_iterator stops on EOF, if an exception is to 
thrown there, the iterating process may stop prematurely. Moreover, not setting std::istream::eofbit
to std::istream::exceptions() may not be enough to avoid EOF exception as std::istream::failbit may 
be set after std::istream::eofbit is set, triggering an exception nevertheless. 

We resort to default std::istream behavior and check for stream status manually after each operation
and throw exceptions as appropriate (otherwise we may need to check for error code and catch istream
exceptions at the same time since we disabled most of the error bits, but not all). 
*/

namespace ph
{

StdInputStream::StdInputStream(std::unique_ptr<std::istream> stream) :
	m_istream(std::move(stream))
{}

void StdInputStream::read(const std::size_t numBytes, std::byte* const out_bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_istream);
	PH_ASSERT(out_bytes);

	ensureStreamIsGoodForRead();

	m_istream->read(reinterpret_cast<char*>(out_bytes), numBytes);
	if(!m_istream->good())
	{
		throw_formatted<IOException>(
			"Error on trying to read {} bytes from std::istream ({}).",
			numBytes, getReasonForError());
	}

	PH_ASSERT_EQ(numBytes, m_istream->gcount());
}

void StdInputStream::readString(std::string* const out_string, const char delimiter)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_istream);
	PH_ASSERT(out_string);

	ensureStreamIsGoodForRead();

	// Note that std::getline() will stop on EOF (EOF is considered a delimiter, the final one)

	// std::getline() clears the string for us
	std::getline(*m_istream, *out_string, delimiter);

	if(!m_istream->good() && !m_istream->eof())
	{
		throw_formatted<IOException>(
			"Error on trying to read a string with delimiter {} from std::istream ({}).",
			delimiter, getReasonForError());
	}
}

std::size_t StdInputStream::readSome(const std::size_t numBytes, std::byte* const out_bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_istream);
	PH_ASSERT(out_bytes);

	ensureStreamIsGoodForRead();

	m_istream->read(reinterpret_cast<char*>(out_bytes), numBytes);

	if(!m_istream->good())
	{
		if(m_istream->eof())
		{
			const auto numBytesRead = static_cast<std::size_t>(m_istream->gcount());
			return numBytesRead;
		}

		throw_formatted<IOException>(
			"Error on trying to read {} bytes from std::istream ({}).",
			numBytes, getReasonForError());
	}

	PH_ASSERT_EQ(numBytes, m_istream->gcount());
	return numBytes;
}

void StdInputStream::seekGet(const std::size_t pos)
{
	PH_ASSERT(m_istream);

	ensureStreamIsGoodForRead();

	m_istream->seekg(pos);
	
	if(!m_istream->good())
	{
		throw_formatted<IOException>(
			"Error seeking to position {} on std::istream ({}).",
			pos, getReasonForError());
	}
}

std::optional<std::size_t> StdInputStream::tellGet()
{
	PH_ASSERT(m_istream);

	const std::istream::pos_type pos = m_istream->tellg();

	// According to https://en.cppreference.com/w/cpp/io/basic_istream/tellg
	// tellg() may fail by returning pos_type(-1) on failure. In such case, return empty position
	// to indicate error.
	return pos != std::istream::pos_type(-1)
		? std::make_optional<std::size_t>(pos)
		: std::nullopt;
}

void StdInputStream::ensureStreamIsGoodForRead() const
{
	if(!isStreamGoodForRead())
	{
		throw IOException(getReasonForError());
	}
}

std::string StdInputStream::getReasonForError() const
{
	std::string errorMsg;
	if(isStreamGoodForRead())
	{
		errorMsg += "No error.";
	}
	else if(!m_istream)
	{
		errorMsg += "Stream is uninitialized.";
	}
	else if(m_istream->eof())
	{
		errorMsg += "Stream is on EOF (expected std::istream to not being on EOF already).";
	}
	else
	{
		errorMsg += std::strerror(errno);
	}

	std::string name = acquireName();
	if(name.empty())
	{
		name = "(unavailable)";
	}

	return errorMsg + "; Stream name: " + name;
}

}// end namespace ph
