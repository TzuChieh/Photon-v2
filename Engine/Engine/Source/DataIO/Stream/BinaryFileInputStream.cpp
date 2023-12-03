#include "DataIO/Stream/BinaryFileInputStream.h"

#include <fstream>
#include <memory>
#include <filesystem>

namespace ph
{

BinaryFileInputStream::BinaryFileInputStream(const Path& filePath) :

	StdInputStream(std::make_unique<std::ifstream>(
		filePath.toNativeString().c_str(),
		std::ios_base::in | std::ios_base::binary)),

	m_filePath(filePath)
{}

std::optional<std::size_t> BinaryFileInputStream::getFileSizeInBytes() const
{
	try
	{
		return static_cast<std::size_t>(std::filesystem::file_size(m_filePath.toNativeString()));
	}
	catch(const std::filesystem::filesystem_error& /* e */)
	{
		return std::nullopt;
	}
}

}// end namespace ph
