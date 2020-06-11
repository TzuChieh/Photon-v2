#include "DataIO/PfmFileWriter.h"
#include "DataIO/Stream/BinaryFileOutputStream.h"
#include "Utility/utility.h"
#include "Common/Logger.h"

#include <string>
#include <limits>
#include <vector>

namespace ph
{

namespace
{
	Logger logger(LogSender("PFM File Writer"));
}

PfmFileWriter::PfmFileWriter(const Path& filePath) :
	m_filePath(filePath)
{}

bool PfmFileWriter::save(const HdrRgbFrame& frame)
{
	BinaryFileOutputStream file(m_filePath);

	// Write header (3 lines of text)
	{
		std::string header;

		// "PF" for a 3-channel RGB color image
		header += std::string("PF\n");

		// Image resolution
		header += std::to_string(frame.widthPx()) + " " + std::to_string(frame.heightPx()) + "\n";

		// Endianness
		header += std::to_string(is_big_endian() ? 1 : -1) + "\n";

		if(!file.write(header.length(), reinterpret_cast<const std::byte*>(header.data())))
		{
			logger.log(ELogLevel::WARNING_MED,
				"error writing file header");

			return false;
		}
	}

	// Write raster data (a series of three 4-byte IEEE-754 single precision
	// floating point numbers for each pixel, specified in left to right, 
	// bottom to top order)

	static_assert(std::numeric_limits<HdrRgbFrame::Element>::is_iec559);

	std::vector<float> rasterData(static_cast<std::size_t>(3) * frame.widthPx() * frame.heightPx());
	for(uint32 y = 0; y < frame.heightPx(); ++y)
	{
		for(uint32 x = 0; x < frame.widthPx(); ++x)
		{
			const auto pixel     = frame.getPixel({x, y});
			const auto baseIndex = 3 * (static_cast<std::size_t>(y) * frame.widthPx() + x);

			rasterData[baseIndex]     = static_cast<float>(pixel[0]);
			rasterData[baseIndex + 1] = static_cast<float>(pixel[1]);
			rasterData[baseIndex + 2] = static_cast<float>(pixel[2]);
		}
	}

	if(!file.write(rasterData.size() * sizeof(float), reinterpret_cast<const std::byte*>(rasterData.data())))
	{
		logger.log(ELogLevel::WARNING_MED,
			"error writing file raster data");

		return false;
	}

	return true;
}

}// end namespace ph
