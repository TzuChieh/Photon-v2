#include "DataIO/Stream/FormattedTextFileOutputStream.h"
#include "Common/Logger.h"
#include "Common/assertion.h"

#include <fstream>
#include <utility>

namespace ph
{

namespace
{

Logger logger(LogSender("Text O-Stream"));

}

FormattedTextFileOutputStream::FormattedTextFileOutputStream(const Path& filePath) :
	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out))
{
	if(getStream() && !getStream()->good())
	{
		logger.log(ELogLevel::WARNING_MED,
			"error encountered while opening file <" + 
			filePath.toAbsoluteString() +
			">, output operations may be unavailable");
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
