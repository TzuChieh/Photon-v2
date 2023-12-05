#include "DataIO/Stream/FormattedTextInputStream.h"
#include "DataIO/io_exceptions.h"

#include <Common/assertion.h>
#include <Common/os.h>
#include <Common/logging.h>
#include <Utility/string_utils.h>

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cctype>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FormattedTextInputStream, DataIO);

FormattedTextInputStream::FormattedTextInputStream(const Path& textFilePath)
	: FormattedTextInputStream(
		std::make_unique<std::ifstream>(
			textFilePath.toNativeString().c_str(),
			std::ios_base::in),
		textFilePath.toAbsoluteString())
{}

FormattedTextInputStream::FormattedTextInputStream(const std::string& textString)
	: FormattedTextInputStream(
		std::make_unique<std::istringstream>(
			textString, 
			std::ios_base::in),
		"")
{}

FormattedTextInputStream::FormattedTextInputStream(std::unique_ptr<std::istream> stream, std::string streamName)
	: StdInputStream(std::move(stream))
	, m_streamName(std::move(streamName))
{}

std::string FormattedTextInputStream::acquireName() const
{
	return m_streamName;
}

void FormattedTextInputStream::readAll(std::string* const out_allText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_allText);

	ensureStreamIsGoodForRead();

	std::ostringstream buffer;
	buffer << getStream()->rdbuf();

	// `move()` required as we want to call the rvalue-qualified overload of `str()`
	*out_allText = std::move(buffer).str();

	// EOF-bit of the stream is not set as we are reading indirectly through the stream buffer,
	// attempting to read the next character through `peek()` seems to fix this (set the EOF-bit). 
	// We need the EOF-bit set so `operator bool ()` properly returns false after `readAll()`.
	// See https://stackoverflow.com/questions/6114231/c-io-file-streams-writing-from-one-file-to-another-using-operator-and-rdbuf
	getStream()->peek();

	// Report non-EOF error
	if(!getStream()->good() && !getStream()->eof())
	{
		throw_formatted<IOException>(
			"Error reading all text from std::istream ({}).",
			getReasonForError());
	}
}

void FormattedTextInputStream::readAllTightly(std::string* const out_allText)
{
	PH_ASSERT(getStream());
	PH_ASSERT(out_allText);

	ensureStreamIsGoodForRead();

	// Note that when reading characters, std::istream_iterator skips whitespace by default
	// (the std::istream_interator will iterate until EOF)

	out_allText->clear();
	out_allText->append(
		std::istream_iterator<char>(*(getStream())),
		std::istream_iterator<char>());

	// Report non-EOF error
	if(!getStream()->good() && !getStream()->eof())
	{
		throw_formatted<IOException>(
			"Error reading all remaining text tightly from std::istream ({}).",
			getReasonForError());
	}
}

}// end namespace ph
