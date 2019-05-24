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
	static HdrRgbFrame load(const Path& picturePath);
	static LdrRgbFrame loadLdr(const Path& picturePath);
	static HdrRgbFrame loadHdr(const Path& picturePath);

private:
	static LdrRgbFrame loadLdrViaStb(const std::string& fullFilename);
	static HdrRgbFrame loadHdrViaStb(const std::string& fullFilename);

	static const Logger logger;
};

}// end namespace ph