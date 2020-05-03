#include "DataIO/Stream/BinaryFileInputStream.h"
#include "Common/Logger.h"

#include <fstream>

namespace ph
{

namespace
{
	Logger logger(LogSender("Binary I-Stream"));
}

BinaryFileInputStream::BinaryFileInputStream(const Path& filePath) :
	m_istream(std::make_unique<std::ifstream>(
		filePath.toAbsoluteString().c_str(),
		std::ios_base::in | std::ios_base::binary))
{
	if(!m_istream->good())
	{
		logger.log(ELogLevel::WARNING_MED,
			"error encountered while opening file <" + 
			filePath.toAbsoluteString() +
			">, input operations may be unavailable");
	}
}

bool BinaryFileInputStream::read(const std::size_t numBytes, std::byte* const out_bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));

	m_istream->read(reinterpret_cast<char*>(out_bytes), numBytes);

	const auto numReadBytes = m_istream->gcount();
	return numBytes == numReadBytes;
}

void BinaryFileInputStream::seekGet(const std::size_t pos)
{
	m_istream->seekg(pos);
}

std::size_t BinaryFileInputStream::tellGet() const
{
	return m_istream->tellg();
}

}// end namespace ph