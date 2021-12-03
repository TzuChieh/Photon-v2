#include "DataIO/Stream/StdOutputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <utility>
#include <format>
#include <cerrno>
#include <cstring>

/*
Note on the implementation:

We do not rely on setting bits to std::ostream::exceptions() for error detection. Instead, we explicitly
test whether the stream is in an erroneous state. 

See <DataIO/Stream/StdInputStream.cpp> for more information.
*/

namespace ph
{

StdOutputStream::StdOutputStream(std::unique_ptr<std::ostream> stream) :
	m_ostream(std::move(stream))
{}

void StdOutputStream::write(const std::size_t numBytes, const std::byte* const bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_ostream);
	PH_ASSERT(bytes);

	ensureStreamIsGoodForWrite();

	m_ostream->write(reinterpret_cast<const char*>(bytes), numBytes);

	if(!m_ostream->good())
	{
		throw IOException(std::format(
			"Error on trying to write {} bytes to std::ostream ({}).",
			numBytes, getReasonForError()));
	}
}

void StdOutputStream::seekPut(const std::size_t pos)
{
	PH_ASSERT(m_ostream);

	ensureStreamIsGoodForWrite();

	m_ostream->seekp(pos);
	
	if(!m_ostream->good())
	{
		throw IOException(std::format(
			"Error seeking to position {} on std::ostream ({}).",
			pos, getReasonForError()));
	}
}

std::optional<std::size_t> StdOutputStream::tellPut()
{
	PH_ASSERT(m_ostream);

	// tellp() will not throw; instead, it returns pos_type(-1) if a failure occurs
	const std::ostream::pos_type pos = m_ostream->tellp();
	if(pos == std::ostream::pos_type(-1))
	{
		return std::nullopt;
	}

	return static_cast<std::size_t>(pos);
}

void StdOutputStream::ensureStreamIsGoodForWrite() const
{
	if(!isStreamGoodForWrite())
	{
		throw IOException(getReasonForError());
	}
}

std::string StdOutputStream::getReasonForError() const
{
	if(isStreamGoodForWrite())
	{
		return "No error.";
	}

	if(!m_ostream)
	{
		return "Stream is uninitialized.";
	}

	return std::strerror(errno);
}

}// end namespace ph
