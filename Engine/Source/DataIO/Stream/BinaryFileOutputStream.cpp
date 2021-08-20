#include "DataIO/Stream/BinaryFileOutputStream.h"
#include "DataIO/io_exceptions.h"

#include <fstream>
#include <memory>
#include <format>

namespace ph
{

BinaryFileOutputStream::BinaryFileOutputStream(const Path& filePath) :
	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out | std::ios_base::binary))
{
	if(getStream() && !getStream()->good())
	{
		throw FileIOError(
			std::format("error encountered while opening binary file",
			filePath.toAbsoluteString()));
	}
}

}// end namespace ph
