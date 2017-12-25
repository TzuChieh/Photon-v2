#pragma once

#include "Common/primitive_type.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <memory>

namespace ph
{

template<typename ComponentType>
class TFrame;

class PictureLoader final
{
public:
	static TFrame<uint8> loadLdr(const Path& picturePath);

private:
	static TFrame<uint8> loadLdrViaStb(const std::string& fullFilename);

	static const Logger& LOGGER();
};

}// end namespace ph