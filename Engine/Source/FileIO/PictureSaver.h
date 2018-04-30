#pragma once

#include "Common/Logger.h"
#include "Frame/frame_fwd.h"
#include "FileIO/FileSystem/Path.h"

namespace ph
{

class PictureSaver final
{
public:
	// Saves a LDR frame in the spacified file.
	//
	static bool save(const LdrRgbFrame& frame, const Path& filePath);

	// Saves a HDR frame in the specified file. Notice that if the file is 
	// a LDR format, values outside [0, 1] will be clamped.
	//
	static bool save(const HdrRgbFrame& frame, const Path& filePath);

private:
	static bool saveFrameViaStb(const LdrRgbFrame& frame, const Path& path);

	static const Logger logger;
};

}// end namespace ph