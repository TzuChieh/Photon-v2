#include "DataIO/Stream/BinaryFileOutputStream.h"
#include "Common/Logger.h"
#include "Common/assertion.h"

#include <fstream>
#include <utility>

namespace ph
{

namespace
{

Logger logger(LogSender("Binary O-Stream"));

}

BinaryFileOutputStream::BinaryFileOutputStream(const Path& filePath) :
	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out | std::ios_base::binary))
{
	if(getStream() && !getStream()->good())
	{
		logger.log(ELogLevel::WARNING_MED,
			"error encountered while opening file <" + 
			filePath.toAbsoluteString() +
			">, output operations may be unavailable");
	}
}

}// end namespace ph
