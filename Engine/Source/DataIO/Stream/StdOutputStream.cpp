#include "DataIO/Stream/StdOutputStream.h"
#include "Common/Logger.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

namespace
{
	
Logger logger(LogSender("std O-Stream"));

}

StdOutputStream::StdOutputStream(std::unique_ptr<std::ostream> stream) :
	m_ostream(std::move(stream))
{}

bool StdOutputStream::write(const std::size_t numBytes, const std::byte* const bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));
	PH_ASSERT(m_ostream);
	PH_ASSERT(bytes);

	m_ostream->write(reinterpret_cast<const char*>(bytes), numBytes);

	// TODO: proper exception handling
	return true;
}

void StdOutputStream::seekPut(const std::size_t pos)
{
	PH_ASSERT(m_ostream);

	m_ostream->seekp(pos);
}

std::size_t StdOutputStream::tellPut()
{
	PH_ASSERT(m_ostream);

	return m_ostream->tellp();
}

}// end namespace ph
