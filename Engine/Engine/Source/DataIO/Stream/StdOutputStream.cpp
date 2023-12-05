#include "DataIO/Stream/StdOutputStream.h"
#include "DataIO/io_exceptions.h"

#include <Common/assertion.h>

#include <utility>
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
		throw_formatted<IOException>(
			"Error on trying to write {} bytes to std::ostream ({}).",
			numBytes, getReasonForError());
	}
}

void StdOutputStream::writeString(std::string_view str)
{
	PH_ASSERT(getStream());

	ensureStreamIsGoodForWrite();

	*m_ostream << str;

	if(!m_ostream->good())
	{
		throw_formatted<IOException>(
			"Error writing string to std::ostream (string length: {}); reason: {}",
			str.size(), getReasonForError());
	}
}

void StdOutputStream::seekPut(const std::size_t pos)
{
	PH_ASSERT(m_ostream);

	ensureStreamIsGoodForWrite();

	m_ostream->seekp(pos);
	
	if(!m_ostream->good())
	{
		throw_formatted<IOException>(
			"Error seeking to position {} on std::ostream ({}).",
			pos, getReasonForError());
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
	std::string errorMsg;
	if(isStreamGoodForWrite())
	{
		errorMsg += "No error.";
	}
	else if(!m_ostream)
	{
		errorMsg += "Stream is uninitialized.";
	}
	else
	{
		return std::strerror(errno);
	}

	std::string name = acquireName();
	if(name.empty())
	{
		name = "(unavailable)";
	}

	return errorMsg + "; Stream name: " + name;
}

}// end namespace ph
