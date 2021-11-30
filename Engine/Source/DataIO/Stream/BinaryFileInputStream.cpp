#include "DataIO/Stream/BinaryFileInputStream.h"
#include "Common/logging.h"
#include "DataIO/io_exceptions.h"

#include <fstream>
#include <utility>
#include <memory>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BinaryFileInputStream, DataIO);

BinaryFileInputStream::BinaryFileInputStream(const Path& filePath) :
	StdInputStream(std::make_unique<std::ifstream>(
		filePath.toAbsoluteString().c_str(),
		std::ios_base::in | std::ios_base::binary))
{
	if(getStream() && !getStream()->good())
	{
		throw FileIOError(
			std::format("error encountered while opening binary file",
			filePath.toAbsoluteString()));
	}
}

}// end namespace ph
