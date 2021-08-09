#include "DataIO/Stream/FormattedTextFileOutputStream.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <fstream>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FormattedTextFileOutputStream, DataIO);

FormattedTextFileOutputStream::FormattedTextFileOutputStream(const Path& filePath) :
	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out))
{
	if(getStream() && !getStream()->good())
	{
		PH_LOG_WARNING(FormattedTextFileOutputStream,
			"error encountered while opening file <{}>, output operations may be unavailable", 
			filePath.toAbsoluteString());
	}
}

bool FormattedTextFileOutputStream::writeStr(const std::string_view str)
{
	PH_ASSERT(getStream());

	*(getStream()) << str;

	// TODO: properly handle this
	return true;
}

}// end namespace ph
