#include "DataIO/Stream/FormattedTextInputStream.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Common/os.h"
#include "DataIO/io_exceptions.h"

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cctype>
#include <algorithm>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FormattedTextInputStream, DataIO);

FormattedTextInputStream::FormattedTextInputStream(const Path& textFilePath) : 
	StdInputStream(std::make_unique<std::ifstream>(
		textFilePath.toAbsoluteString().c_str(),
		std::ios_base::in))
{
	if(getStream() && !getStream()->good())
	{
		throw FileIOError(std::format(
			"error encountered while opening text file",
			textFilePath.toAbsoluteString()));
	}
}

FormattedTextInputStream::FormattedTextInputStream(const std::string& textString) : 
	StdInputStream(std::make_unique<std::istringstream>(
		textString,
		std::ios_base::in))
{
	if(getStream() && !getStream()->good())
	{
		throw FileIOError(std::format(
			"error encountered while wrapping string as stream"));
	}
}

void FormattedTextInputStream::readAllTightly(std::string* const out_allText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_allText);

	out_allText->clear();
	out_allText->append(
		std::istream_iterator<char>(*(getStream())),
		std::istream_iterator<char>());
}

void FormattedTextInputStream::readLine(std::string* const out_lineText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_lineText);

	out_lineText->clear();
	std::getline(*(getStream()), *out_lineText);
}

void FormattedTextInputStream::readTrimmedLine(std::string* const out_lineText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_lineText);

	out_lineText->clear();

	// Skip any leading whitespaces
	*(getStream()) >> std::ws;

	std::getline(*(getStream()), *out_lineText);

	// Remove trailing whitespaces
	const auto trailingWhiteSpaceBegin = std::find_if(
		out_lineText->rbegin(),
		out_lineText->rend(),
		[](const char ch)
		{
			return !std::isspace(ch);
		}).base();
	out_lineText->erase(trailingWhiteSpaceBegin, out_lineText->end());
}

}// end namespace ph
