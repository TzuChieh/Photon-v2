#include "DataIO/Stream/FormattedTextInputStream.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Common/os.h"
#include "DataIO/io_exceptions.h"
#include "Utility/string_utils.h"

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cctype>
#include <utility>
#include <format>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FormattedTextInputStream, DataIO);

FormattedTextInputStream::FormattedTextInputStream(const Path& textFilePath) : 
	FormattedTextInputStream(
		std::make_unique<std::ifstream>(
			textFilePath.toAbsoluteString().c_str(),
			std::ios_base::in),
		textFilePath.toAbsoluteString())
{}

FormattedTextInputStream::FormattedTextInputStream(const std::string& textString) : 
	FormattedTextInputStream(
		std::make_unique<std::istringstream>(
			textString, 
			std::ios_base::in),
		"")
{}

FormattedTextInputStream::FormattedTextInputStream(std::unique_ptr<std::istream> stream, std::string streamName) :
	StdInputStream(std::move(stream)),
	m_streamName(std::move(streamName))
{}

std::string FormattedTextInputStream::acquireName()
{
	return m_streamName;
}

void FormattedTextInputStream::readAllTightly(std::string* const out_allText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_allText);

	ensureStreamIsNotOnEOF();

	try
	{
		// Note that when reading characters, std::istream_iterator skips whitespace by default
		// (the std::istream_interator will iterate until EOF)

		out_allText->clear();
		out_allText->append(
			std::istream_iterator<char>(*(getStream())),
			std::istream_iterator<char>());
	}
	// exceptions() is set to NOT throw on EOF
	catch(const std::istream::failure& e)
	{
		throw IOException(std::format(
			"error reading bytes from std::istream; {}, reason: {}, ",
			e.what(), e.code().message()));
	}
}

void FormattedTextInputStream::readLine(std::string* const out_lineText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_lineText);

	ensureStreamIsNotOnEOF();

	try
	{
		// Note that std::getline() will stop on EOF (EOF is considered a delimiter, the final one)

		out_lineText->clear();
		std::getline(*(getStream()), *out_lineText);
	}
	// exceptions() is set to NOT throw on EOF
	catch(const std::istream::failure& e)
	{
		throw IOException(std::format(
			"error reading bytes from std::istream; {}, reason: {}, ",
			e.what(), e.code().message()));
	}
}

}// end namespace ph
