#include "DataIO/Stream/BinaryFileOutputStream.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <fstream>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BinaryFileOutputStream, DataIO);

BinaryFileOutputStream::BinaryFileOutputStream(const Path& filePath) :
	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out | std::ios_base::binary))
{
	if(getStream() && !getStream()->good())
	{
		PH_LOG_WARNING(BinaryFileOutputStream, 
			"error encountered while opening file <{}>, output operations may be unavailable",
			filePath.toAbsoluteString());
	}
}

}// end namespace ph
