#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Frame/frame_fwd.h"

#include <string_view>

namespace ph
{

class ExrFileReader
{
public:
	explicit ExrFileReader(const Path& filePath);

	void load(HdrRgbFrame* out_frame);

	void loadFromFilesystem(
		HdrRgbFrame* out_frame,
		std::string_view redChannelName = "R",
		std::string_view greenChannelName = "G",
		std::string_view blueChannelName = "B");

private:
	static void loadStandaloneImageData(
		const Path filePath,
		HdrRgbFrame* out_frame,
		std::string_view redChannelName,
		std::string_view greenChannelName,
		std::string_view blueChannelName);

	Path m_filePath;
};

}// end namespace ph
