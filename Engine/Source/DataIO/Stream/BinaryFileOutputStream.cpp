#include "DataIO/Stream/BinaryFileOutputStream.h"

#include <fstream>
#include <memory>

namespace ph
{

BinaryFileOutputStream::BinaryFileOutputStream(const Path& filePath) :
	StdOutputStream(
		std::make_unique<std::ofstream>(
			filePath.toAbsoluteString().c_str(),
			std::ios_base::out | std::ios_base::binary))
{}

}// end namespace ph
