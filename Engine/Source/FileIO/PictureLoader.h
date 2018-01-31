#pragma once

#include "Common/primitive_type.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"
#include "Frame/TFrame.h"

#include <memory>

namespace ph
{

class PictureLoader final
{
public:
	static LdrRgbFrame loadLdr(const Path& picturePath);

private:
	static LdrRgbFrame loadLdrViaStb(const std::string& fullFilename);

	static const Logger& LOGGER();
};

}// end namespace ph