#include "DataIO/Stream/FormattedTextFileOutputStream.h"
#include "Common/assertion.h"
#include "DataIO/io_exceptions.h"

#include <fstream>
#include <utility>
#include <format>

namespace ph
{

FormattedTextFileOutputStream::FormattedTextFileOutputStream(const Path& filePath) :

	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out)),

	m_filePath(filePath)
{
	if(getStream() && !getStream()->good())
	{
		throw FileIOError(std::format("error encountered while opening file", 
			filePath.toAbsoluteString()));
	}
}

bool FormattedTextFileOutputStream::writeStr(const std::string_view str)
{
	PH_ASSERT(getStream());

	*(getStream()) << str;

	// TODO: properly handle this
	return true;
}

std::string FormattedTextFileOutputStream::acquireName()
{
	return m_filePath.toAbsoluteString();
}

}// end namespace ph
