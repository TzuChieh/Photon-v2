#include "DataIO/Stream/StdInputStream.h"
#include "Common/assertion.h"
#include "Common/os.h"
#include "DataIO/io_exceptions.h"

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cctype>
#include <algorithm>

namespace ph
{

StdInputStream::StdInputStream(std::unique_ptr<std::istream> stream) :
	m_istream(std::move(stream))
{
	useExceptionForIStreamError();
}

bool StdInputStream::read(const std::size_t numBytes, std::byte* const out_bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_istream);
	PH_ASSERT(out_bytes);

	try
	{
		m_istream->read(reinterpret_cast<char*>(out_bytes), numBytes);
	}
	catch(const std::istream::failure& e)
	{
		throw IOException(std::format("error reading bytes from std::istream; {}, reason: {}, ", 
			e.what(), e.code().message()));
	}

	const auto numReadBytes = m_istream->gcount();
	return numBytes == numReadBytes;
}

void StdInputStream::seekGet(const std::size_t pos)
{
	PH_ASSERT(m_istream);

	try
	{
		m_istream->seekg(pos);
	}
	catch(const std::istream::failure& e)
	{
		throw IOException(std::format("error seeking get on std::istream; {}, reason: {}, ",
			e.what(), e.code().message()));
	}
}

std::optional<std::size_t> StdInputStream::tellGet()
{
	PH_ASSERT(m_istream);

	try
	{
		const std::istream::pos_type pos = m_istream->tellg();
		if(pos == std::ostream::pos_type(-1))
		{
			return std::nullopt;
		}

		return static_cast<std::size_t>(pos);
	}
	// According to https://en.cppreference.com/w/cpp/io/basic_istream/tellg
	// tellg() may fail/throw if the error state flag is not goodbit. In this case, return empty position
	// to indicate error.
	catch(const std::istream::failure& e)
	{
		return std::nullopt;
	}
}

void StdInputStream::useExceptionForIStreamError()
{
	if(!m_istream)
	{
		return;
	}

	try
	{
		m_istream->exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);
	}
	catch(const std::istream::failure& e)
	{
		throw IOException(std::format(
			"existing error detected on enabling std::istream exceptions; {}, reason: {}, ",
			e.what(), e.code().message()));
	}
}

}// end namespace ph
