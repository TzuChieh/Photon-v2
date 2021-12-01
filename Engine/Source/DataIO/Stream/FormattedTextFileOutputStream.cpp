#include "DataIO/Stream/FormattedTextFileOutputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <fstream>

namespace ph
{

FormattedTextFileOutputStream::FormattedTextFileOutputStream(const Path& filePath) try :

	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out)),

	m_filePath(filePath)
{}
catch(const IOException& e)
{
	throw FileIOError(std::format(
		"error encountered while opening text file: {}", e.what(), 
		filePath.toAbsoluteString()));
}

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

	try
	{
		*(getStream()) << str;
	}
	catch(const std::ostream::failure& e)
	{
		throw FileIOError(
			std::format("error writing string to std::ostream (string length: {}); {}, reason: {}, ",
				str.size(), e.what(), e.code().message()),
			m_filePath.toAbsoluteString());
	}
}

void FormattedTextFileOutputStream::writeChar(const char ch)
{
	PH_ASSERT(getStream());

	try
	{
		*(getStream()) << ch;
	}
	catch(const std::ostream::failure& e)
	{
		throw FileIOError(
			std::format("error writing char \'{}\' to std::ostream; {}, reason: {}, ", 
				ch, e.what(), e.code().message()),
			m_filePath.toAbsoluteString());
	}
}

std::string FormattedTextFileOutputStream::acquireName()
{
	return m_filePath.toAbsoluteString();
}

}// end namespace ph
