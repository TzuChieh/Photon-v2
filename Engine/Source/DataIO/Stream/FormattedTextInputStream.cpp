#include "DataIO/Stream/FormattedTextInputStream.h"
#include "Common/assertion.h"
#include "Common/Logger.h"
#include "Common/os.h"

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cctype>
#include <algorithm>

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

std::size_t FormattedTextInputStream::tellGet()
{
	return m_istream->tellg();
}

void FormattedTextInputStream::readAllTightly(std::string* const out_allText)
{
	PH_ASSERT(out_allText);

	out_allText->clear();
	out_allText->append(
		std::istream_iterator<char>(*m_istream),
		std::istream_iterator<char>());
}

bool FormattedTextInputStream::readLineTightly(std::string* const out_lineText)
{
	PH_ASSERT(out_lineText);

	out_lineText->clear();

	// skip any leading whitespaces
	*m_istream >> std::ws;

	std::getline(*m_istream, *out_lineText);

	// remove trailing whitespaces
	const auto trailingWhiteSpaceBegin = std::find_if(
		out_lineText->rbegin(),
		out_lineText->rend(),
		[](const char ch)
		{
			return !std::isspace(ch);
		}).base();
	out_lineText->erase(trailingWhiteSpaceBegin, out_lineText->end());
	
	return !out_lineText->empty();
}

}// end namespace ph
