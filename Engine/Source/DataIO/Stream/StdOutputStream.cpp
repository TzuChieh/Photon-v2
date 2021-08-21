#include "DataIO/Stream/StdOutputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <utility>
#include <format>

namespace ph
{

StdOutputStream::StdOutputStream(std::unique_ptr<std::ostream> stream) :
	m_ostream(std::move(stream))
{
	useExceptionForOStreamError();
}

void StdOutputStream::write(const std::size_t numBytes, const std::byte* const bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_ostream);
	PH_ASSERT(bytes);

	try
	{
		m_ostream->write(reinterpret_cast<const char*>(bytes), numBytes);
	}
	catch(const std::ostream::failure& e)
	{
		throw IOException(std::format("error writing bytes to std::ostream; {}, reason: {}, ", 
			e.what(), e.code().message()));
	}
}

void StdOutputStream::seekPut(const std::size_t pos)
{
	PH_ASSERT(m_ostream);

	try
	{
		m_ostream->seekp(pos);
	}
	catch(const std::ostream::failure& e)
	{
		throw IOException(std::format("error seeking put on std::ostream; {}, reason: {}, ",
			e.what(), e.code().message()));
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

void StdOutputStream::useExceptionForOStreamError()
{
	if(!m_ostream)
	{
		return;
	}

	try
	{
		// Does not set the "eofbit" mask as it is for input streams
		m_ostream->exceptions(std::ostream::failbit | std::ostream::badbit);
	}
	catch(const std::ostream::failure& e)
	{
		throw IOException(std::format("error on enabling std::ostream exceptions; {}, reason: {}, ",
			e.what(), e.code().message()));
	}
}

}// end namespace ph
