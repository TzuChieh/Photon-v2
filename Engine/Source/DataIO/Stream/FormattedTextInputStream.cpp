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
		throw IOException(std::format(
			"Error reading all text from std::istream ({}).",
			getReasonForError()));
	}
}

}// end namespace ph
