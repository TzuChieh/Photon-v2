#pragma once

#include <Common/primitive_type.h>
#include <Common/Utility/CommandLineArguments.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/TSpan.h>

#include <string>
#include <vector>
#include <iostream>
#include <optional>

namespace ph::film_merger
{

struct FilmInfo final
{
	Path file;
	std::optional<float64> weight;
};

class ProcessedArguments
{
public:
	static void printHelpMessage();

public:
	ProcessedArguments(int argc, char* argv[]);
	explicit ProcessedArguments(CommandLineArguments arguments);

	TSpanView<FilmInfo> getFilmInfos() const;
	std::string getImageOutputPath() const;
	std::string getImageFilePath() const;
	std::string getImageFileFormat() const;
	bool isHelpRequested() const;

private:
	std::vector<FilmInfo> m_filmInfos;
	std::string           m_imageOutputPath;
	std::string           m_imageFileFormat;
	bool                  m_isHelpRequested;
};

inline TSpanView<FilmInfo> ProcessedArguments::getFilmInfos() const
{
	return m_filmInfos;
}

inline std::string ProcessedArguments::getImageOutputPath() const
{
	return m_imageOutputPath;
}

inline std::string ProcessedArguments::getImageFilePath() const
{
	return m_imageOutputPath + "." + m_imageFileFormat;
}

inline std::string ProcessedArguments::getImageFileFormat() const
{
	return m_imageFileFormat;
}

inline bool ProcessedArguments::isHelpRequested() const
{
	return m_isHelpRequested;
}

inline void ProcessedArguments::printHelpMessage()
{
	std::cout << R"(
===============================================================================
Primary usage:

[<film-path> [-w <weight>]]... [-o <image-path>] [-of <image-format>] [--help]

See below for more information about the arguments.
===============================================================================
[<film-path>]
 
File path of the film to merge.
===============================================================================
[-w <weight>]
 
Specify a weighting factor for the film. If the film format does not come with
weighting factors, 1 will be used.
===============================================================================
[-o <image-path>]

Specify output path for the merged image. This should be a filename without the
extension part. Note that the application will not create the directory for you
if it is not already exists.
(default path: "./merged_image")
===============================================================================
[-of <image-format>]

Specify the format of the merged image. Supported formats are: png, jpg, bmp,
tga, hdr, exr, pfm.
(default format: exr)
===============================================================================
[--help]

Print this help message then exit.
===============================================================================
	)" << std::endl;
}

}// end namespace ph::film_merger
