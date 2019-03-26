#pragma once

#include "FileIO/FileSystem/Path.h"
#include "Frame/TFrame.h"

#include <string>
#include <vector>

namespace ph
{

class ExrFileReader
{
public:
	explicit ExrFileReader(const Path& filePath);

	bool load(HdrRgbFrame* out_frame);

private:
	Path m_filePath;

	std::string listAllLayersAndChannels() const;
};

}// end namespace ph