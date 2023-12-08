#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Frame/frame_fwd.h"

#include <string_view>

namespace ph
{

class ByteBuffer;

class ExrFileWriter
{
public:
	/*! @brief Create default writer with limited functionalities.
	Constructed without a path, the file writer may have limited functionalities in operations related
	to the filesystem.
	*/
	ExrFileWriter();

	explicit ExrFileWriter(const Path& filePath);

	bool save(const HdrRgbFrame& frame);
	bool saveHighPrecision(const HdrRgbFrame& frame);

	/*! @brief Save an EXR file to the filesystem.
	A channel will be ignored (not saved) if its name is empty.
	*/
	bool saveToFilesystem(
		const HdrRgbFrame& frame,
		bool saveInHighPrecision = false,
		std::string_view redChannelName = "R",
		std::string_view greenChannelName = "G",
		std::string_view blueChannelName = "B",
		std::string_view alphaChannelName = "",
		HdrComponent alphaValue = 1);

	/*! @brief Save an EXR file to memory instead of filesystem.
	A channel will be ignored (not saved) if its name is empty.
	*/
	bool saveToMemory(
		const HdrRgbFrame& frame, 
		ByteBuffer& buffer,
		std::string_view redChannelName = "R",
		std::string_view greenChannelName = "G",
		std::string_view blueChannelName = "B",
		std::string_view alphaChannelName = "",
		HdrComponent alphaValue = 1);

private:
	static bool saveStandaloneImageData(
		const Path& filePath,
		const HdrRgbFrame& frame,
		bool saveInHighPrecision,
		std::string_view redChannelName,
		std::string_view greenChannelName,
		std::string_view blueChannelName,
		std::string_view alphaChannelName,
		HdrComponent alphaValue);

	Path m_filePath;
};

}// end namespace ph
