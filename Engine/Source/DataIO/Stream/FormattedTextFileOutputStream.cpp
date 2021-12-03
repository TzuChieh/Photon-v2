#include "DataIO/Stream/FormattedTextFileOutputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <fstream>

namespace ph
{

FormattedTextFileOutputStream::FormattedTextFileOutputStream(const Path& filePath) :

	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out)),

	m_filePath(filePath)
{}

void FormattedTextFileOutputStream::writeLine(const std::string_view line)
{
	writeString(line);
	writeNewLine();
}

void FormattedTextFileOutputStream::writeNewLine()
{
	writeChar('\n');
}

void FormattedTextFileOutputStream::writeString(const std::string_view str)
{
	PH_ASSERT(getStream());

	ensureStreamIsGoodForWrite();

	*(getStream()) << str;

	if(!getStream()->good())
	{
		throw FileIOError(
			std::format("Error writing string to std::ostream (string length: {}); reason: {}",
				str.size(), getReasonForError()),
			m_filePath.toAbsoluteString());
	}
}

void FormattedTextFileOutputStream::writeChar(const char ch)
{
	PH_ASSERT(getStream());

	ensureStreamIsGoodForWrite();

	*(getStream()) << ch;

	if(!getStream()->good())
	{
		throw FileIOError(
			std::format("Error writing char \'{}\' to std::ostream; reason: {}",
				ch, getReasonForError()),
			m_filePath.toAbsoluteString());
	}
}

std::string FormattedTextFileOutputStream::acquireName()
{
	return m_filePath.toAbsoluteString();
}

}// end namespace ph
