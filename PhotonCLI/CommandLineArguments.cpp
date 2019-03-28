#include "CommandLineArguments.h"
#include "util.h"

#include <iostream>
#include <string_view>
#include <limits>

PH_CLI_NAMESPACE_BEGIN

namespace
{
	constexpr std::string_view DEFAULT_SCENE_FILE_PATH         = "./scene.p2";
	constexpr std::string_view DEFAULT_DEFAULT_IMAGE_FILE_PATH = "./rendered_scene.png";
}

CommandLineArguments::CommandLineArguments(const std::vector<std::string>& argv) : 
	m_sceneFilePath           (DEFAULT_SCENE_FILE_PATH),
	m_imageFilePath           (DEFAULT_DEFAULT_IMAGE_FILE_PATH),
	m_numRenderThreads        (1),
	m_isPostProcessRequested  (true),
	m_isHelpMessageRequested  (false),
	m_isImageSeriesRequested  (false),
	m_wildcardStart           (""),
	m_wildcardFinish          (""),
	m_outputPercentageProgress(std::numeric_limits<float>::max())
{
	std::string imageFileFormat;
	for(std::size_t i = 1; i < argv.size(); i++)
	{
		if(argv[i] == "-s")
		{
			i++;
			if(i < argv.size())
			{
				m_sceneFilePath = argv[i];
			}
		}
		else if(argv[i] == "-o")
		{
			i++;
			if(i < argv.size())
			{
				m_imageFilePath = argv[i];
			}
		}
		else if(argv[i] == "-of")
		{
			i++;
			if(i < argv.size())
			{
				imageFileFormat = argv[i];
			}
		}
		else if(argv[i] == "-t")
		{
			i++;
			if(i < argv.size())
			{
				const int numRenderThreads = std::stoi(argv[i]);
				if(numRenderThreads > 0)
				{
					m_numRenderThreads = numRenderThreads;
				}
				else
				{
					std::cerr << "warning: bad number of threads <" << argv[i] << ">" << std::endl;
					std::cerr << "use " << m_numRenderThreads << " instead" << std::endl;
				}
			}
		}
		else if(argv[i] == "-p")
		{
			i++;
			if(i < argv.size())
			{
				const float outputPercentageProgress = std::stof(argv[i]);
				if(0 < outputPercentageProgress && outputPercentageProgress < 100)
				{
					m_outputPercentageProgress = outputPercentageProgress;
				}
				else
				{
					std::cerr << "warning: intermediate output percentage specified <" 
					          << outputPercentageProgress << "> is not sensible" << std::endl;
				}
			}
		}
		else if(argv[i] == "--raw")
		{
			m_isPostProcessRequested = false;
		}
		else if(argv[i] == "--help")
		{
			m_isHelpMessageRequested = true;
		}
		else if(argv[i] == "--series")
		{
			m_isImageSeriesRequested = true;
		}
		else if(argv[i] == "--start")
		{
			i++;
			if(i < argv.size())
			{
				m_wildcardStart = argv[i];
			}
			else
			{
				std::cerr << "warning: no wildcard string specified for --start" << std::endl;
			}
		}
		else if(argv[i] == "--finish")
		{
			i++;
			if(i < argv.size())
			{
				m_wildcardFinish = argv[i];
			}
			else
			{
				std::cerr << "warning: no wildcard string specified for --finish" << std::endl;
			}
		}
		else
		{
			std::cerr << "warning: unknown command <" << argv[i] << ">" << std::endl;
			std::cerr << "ignored" << std::endl;
		}
	}// end for each argument

	// possibly override image format if a more specific order is given
	if(!imageFileFormat.empty())
	{
		m_imageFilePath += "." + imageFileFormat;
	}

	// TODO: check arguments
}

std::string CommandLineArguments::getSceneFilePath() const
{
	return m_sceneFilePath;
}

std::string CommandLineArguments::getImageFilePath() const
{
	return m_imageFilePath;
}

int CommandLineArguments::getNumRenderThreads() const
{
	return m_numRenderThreads;
}

bool CommandLineArguments::isPostProcessRequested() const
{
	return m_isPostProcessRequested;
}

bool CommandLineArguments::isHelpMessageRequested() const
{
	return m_isHelpMessageRequested;
}

bool CommandLineArguments::isImageSeriesRequested() const
{
	return m_isImageSeriesRequested;
}

std::string CommandLineArguments::wildcardStart() const
{
	return m_wildcardStart;
}

std::string CommandLineArguments::wildcardFinish() const
{
	return m_wildcardFinish;
}

float CommandLineArguments::getOutputPercentageProgress() const
{
	return m_outputPercentageProgress;
}

void CommandLineArguments::printHelpMessage()
{
	std::cout << R"(
===============================================================================
-s <path>
 
Specify path to scene file. To render an image series, you can specify
"myScene*.p2" as <path> where * is a wildcard for any string (--series is
required in this case). (default path: "./scene.p2")
===============================================================================
-o <path>

Specify image output path. This should be a filename for single image and a
directory for image series. (default path: "./rendered_scene.png")
===============================================================================
-of <format>

Specify the format of output image. Supported formats are: png, jpg, bmp, tga,
hdr, exr. If this option is omitted, format is deduced from filename extension.
===============================================================================
-t <number>

Set number of threads used for rendering. (default: single thread)
===============================================================================
-p <number>

Output an intermediate image whenever the render has progressed <number>%.
(default: never output intermediate image)
===============================================================================
--raw

Do not perform any post-processing. (default: perform post-processing)
===============================================================================
--help

Print this help message then exit.
===============================================================================
--series

Render an image series. The order for rendering will be lexicographical order
of the wildcarded string. Currently only .png is supported.
===============================================================================
--start <*>

Render image series starting from a specific wildcarded string.
===============================================================================
--finish <*>

Render image series until a specific wildcarded string is matched. (inclusive)
===============================================================================
	)" << std::endl;
}

PH_CLI_NAMESPACE_END
