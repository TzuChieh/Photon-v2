#include "DataIO/Stream/BinaryFileOutputStream.h"
#include "Common/Logger.h"

#include <fstream>

namespace ph
{

namespace
{
	Logger logger(LogSender("Binary O-Stream"));
}

BinaryFileOutputStream::BinaryFileOutputStream(const Path& filePath) :
	m_ostream(std::make_unique<std::ofstream>(
		filePath.toAbsoluteString().c_str(),
		std::ios_base::out | std::ios_base::binary))
{
	if(!m_ostream->good())
	{
		logger.log(ELogLevel::WARNING_MED,
			"error encountered while opening file <" + 
			filePath.toAbsoluteString() +
			">, output operations may be unavailable");
	}
}


bool BinaryFileOutputStream::write(const std::size_t numBytes, const std::byte* const bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));

	m_ostream->write(reinterpret_cast<const char*>(bytes), numBytes);

	// TODO: proper exception handling
	return true;
}

void BinaryFileOutputStream::seekPut(const std::size_t pos)
{
	m_ostream->seekp(pos);
}

std::size_t BinaryFileOutputStream::tellPut()
{
	return m_ostream->tellp();
}

}// end namespace ph
