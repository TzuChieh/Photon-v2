#include "DataIO/Stream/FormattedTextOutputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <fstream>
#include <sstream>

namespace ph
{

FormattedTextOutputStream::FormattedTextOutputStream(const Path& filePath)
	: StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toNativeString().c_str(),
			std::ios_base::out))

	, m_streamName(filePath.toAbsoluteString())
	, m_isStringStream(false)
{}

FormattedTextOutputStream::FormattedTextOutputStream(std::string initialStrContent)
	: StdOutputStream(
		std::make_unique<std::ostringstream>(
			std::move(initialStrContent),
			std::ios_base::out))

	, m_streamName()
	, m_isStringStream(true)
{}

void FormattedTextOutputStream::writeNewLine()
{
	writeChar('\n');
}

void FormattedTextOutputStream::writeChar(const char ch)
{
	PH_ASSERT(getStream());

	ensureStreamIsGoodForWrite();

	*(getStream()) << ch;

	if(!getStream()->good())
	{
		throw_formatted<FileIOError>(
			"Error writing char \'{}\' to std::ostream; reason: {}",
			ch, getReasonForError());
	}
}

std::string FormattedTextOutputStream::getString() const
{
	if(!m_isStringStream)
	{
		throw_formatted<IOException>(
			"The stream (name: {}) is not a string stream. Cannot get the written string from it.",
			acquireName());
	}

	try
	{
		auto* strStream = static_cast<std::ostringstream*>(getStream());
		return strStream->str();
	}
	catch(const Exception& e)
	{
		throw_formatted<IOException>(
			"Error on getting the written string from string stream (error: {}).",
			e.what());
	}
}

std::string FormattedTextOutputStream::acquireName() const
{
	return m_streamName;
}

}// end namespace ph
