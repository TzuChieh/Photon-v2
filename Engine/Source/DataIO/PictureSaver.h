#pragma once

#include "Frame/frame_fwd.h"
#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

// A utility class for saving pictures.
class PictureSaver final
{
public:
	static bool init();

	// Saves a LDR frame in the spacified file. Format is deduced from 
	// filename extension.
	static bool save(const LdrRgbFrame& frame, const Path& filePath);

	// Saves a HDR frame in the specified file. Notice that if the specified 
	// format is LDR, values outside [0, 1] will be clamped. Format is deduced
	// from filename extension.
	static bool save(const HdrRgbFrame& frame, const Path& filePath);

	static bool savePng(const LdrRgbFrame& frame, const Path& filePath);
	static bool saveJpg(const LdrRgbFrame& frame, const Path& filePath);
	static bool saveBmp(const LdrRgbFrame& frame, const Path& filePath);
	static bool saveTga(const LdrRgbFrame& frame, const Path& filePath);
	static bool saveHdr(const HdrRgbFrame& frame, const Path& filePath);
	static bool saveExr(const HdrRgbFrame& frame, const Path& filePath);
	static bool saveExrHighPrecision(const HdrRgbFrame& frame, const Path& filePath);

	// HACK
	static bool saveExr(const HdrRgbFrame& frame, std::string& byteBuffer);
};

}// end namespace ph
