#pragma once

#include "Common/Logger.h"
#include "Frame/frame_fwd.h"
#include "FileIO/FileSystem/Path.h"

namespace ph
{

class PictureSaver final
{
public:
	static bool save(const LdrRgbFrame& frame, const Path& filePath);

private:
	static bool saveFrameViaStb(const LdrRgbFrame& frame, const Path& path);

	static const Logger logger;
};

}// end namespace ph