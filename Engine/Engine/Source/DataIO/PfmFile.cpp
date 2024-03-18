#include "DataIO/PfmFile.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/Stream/BinaryFileInputStream.h"
#include "DataIO/Stream/BinaryFileOutputStream.h"
#include "Utility/utility.h"
#include "DataIO/io_exceptions.h"

#include <Common/logging.h>
#include <Common/Utility/string_utils.h>

#include <string>
#include <string_view>
#include <limits>
#include <vector>
#include <array>
#include <cmath>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PfmFile, DataIO);

void PfmFile::load(const Path& filePath)
{
	BinaryFileInputStream file(filePath);
	float32 scale = 1;
	bool needEndianSwap = false;

	// Read header (3 lines of text)
	try
	{
		std::array<std::string, 3> headerLines;
		file.readLine(&headerLines[0]);
		file.readLine(&headerLines[1]);
		file.readLine(&headerLines[2]);

		if(headerLines[0] == "PF")
		{
			m_isMonochromatic = false;
		}
		else if(headerLines[0] == "Pf")
		{
			m_isMonochromatic = true;
		}
		else
		{
			throw_formatted<IOException>(
				"unknown image type specifier \"{}\"", headerLines[0]);
		}

		std::string_view dimensionsLine = headerLines[1];
		const auto widthPx = string_utils::parse_int<uint32>(
			string_utils::next_token(dimensionsLine, &dimensionsLine));
		const auto heightPx = string_utils::parse_int<uint32>(
			string_utils::next_token(dimensionsLine));
		m_frame.setSize(widthPx, heightPx);

		const auto endianIndicator = string_utils::parse_float<float32>(headerLines[2]);
		if(endianIndicator < 0)
		{
			needEndianSwap = is_big_endian();
		}
		else if(endianIndicator > 0)
		{
			needEndianSwap = !is_big_endian();
		}
		else
		{
			throw_formatted<IOException>(
				"invalid image endian indicator \"{}\"", headerLines[2]);
		}
		
		scale = std::abs(endianIndicator);
	}
	catch(const IOException& e)
	{
		throw FileIOError("error reading .pfm header: " + e.whatStr());
	}

	// Read raster data (a series of three 4-byte IEEE-754 single precision
	// floating point numbers for each pixel, specified in left to right, 
	// bottom to top order)
	try
	{
		if(needEndianSwap)
		{
			file.readData<float32, true>(m_frame.getPixelData());
		}
		else
		{
			file.readData(m_frame.getPixelData());
		}
	}
	catch(const IOException& e)
	{
		throw FileIOError("error reading .pfm raster data: " + e.whatStr());
	}

	if(scale != 1)
	{
		m_frame.forEachPixel(
			[scale](const TFrame<float32, 3>::PixelType& pixel)
			{
				return pixel * scale;
			});
	}
}

void PfmFile::save(const Path& filePath) const
{
	BinaryFileOutputStream file(filePath);

	// Write header (3 lines of text)
	try
	{
		std::string header;

		// "PF" for a 3-channel RGB color image
		header += std::string("PF\n");

		// Image resolution
		header += std::to_string(m_frame.widthPx()) + " " + std::to_string(m_frame.heightPx()) + "\n";

		// Endianness
		header += std::to_string(is_big_endian() ? 1 : -1) + "\n";

		file.write(header.length(), reinterpret_cast<const std::byte*>(header.data()));
	}
	catch(const IOException& e)
	{
		throw FileIOError("error writing .pfm header: " + e.whatStr());
	}

	// Write raster data (a series of three 4-byte IEEE-754 single precision
	// floating point numbers for each pixel, specified in left to right, 
	// bottom to top order)
	try
	{
		file.writeData(m_frame.getPixelData());
	}
	catch(const IOException& e)
	{
		throw FileIOError("error writing .pfm raster data: " + e.whatStr());
	}
}

}// end namespace ph
