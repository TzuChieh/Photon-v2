#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Frame/TFrame.h"

namespace ph
{

class ExrFileReader
{
public:
	explicit ExrFileReader(const Path& filePath);

	bool load(HdrRgbFrame* out_frame);

private:
	Path m_filePath;

	bool loadStandaloneRgb(HdrRgbFrame* const out_frame);
	std::string listAllLayersAndChannels();
};

}// end namespace ph
