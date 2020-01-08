#include "DataIO/Stream/FormattedTextInputStream.h"
#include "Common/assertion.h"
#include "Common/Logger.h"
#include "Common/os.h"

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>

namespace ph
{

namespace
{
	Logger logger(LogSender("Text I-Stream"));
}

FormattedTextInputStream::FormattedTextInputStream(const Path& textFilePath) : 
	m_istream(std::make_unique<std::ifstream>(
		textFilePath.toAbsoluteString().c_str(),
		std::ios_base::in))
{
	if(!m_istream->good())
	{
		logger.log(ELogLevel::WARNING_MED,
			"error encountered while opening file <" +
			textFilePath.toAbsoluteString() +
			">, input operations may be unavailable");
	}
}

FormattedTextInputStream::FormattedTextInputStream(const std::string& textString) : 
	m_istream(std::make_unique<std::istringstream>(
		textString,
		std::ios_base::in))
{
	if(!m_istream->good())
	{
		logger.log(ELogLevel::WARNING_MED,
			"error encountered while constructing stream from string, "
			"input operations may be unavailable");
	}
}

bool FormattedTextInputStream::read(const std::size_t numBytes, std::byte* const out_bytes)
{
	static_assert(sizeof(char) == sizeof(std::byte));

	m_istream->read(reinterpret_cast<char*>(out_bytes), numBytes);

	const auto numReadBytes = m_istream->gcount();
	return numBytes == numReadBytes;
}

void FormattedTextInputStream::seekGet(const std::size_t pos)
{
	m_istream->seekg(pos);
}

std::size_t FormattedTextInputStream::tellGet() const
{
	return m_istream->tellg();
}

void FormattedTextInputStream::readAll(std::string* const out_allText)
{
	PH_ASSERT(out_allText);

	out_allText->clear();
	out_allText->append(
		std::istream_iterator<char>(*m_istream),
		std::istream_iterator<char>());
}

bool FormattedTextInputStream::readLine(std::string* const out_lineText)
{
	PH_ASSERT(out_lineText);

	const bool hasLine = m_istream->good();
	if(hasLine)
	{
		std::getline(*m_istream, *out_lineText);
	}

	return hasLine;
}

}// end namespace ph
