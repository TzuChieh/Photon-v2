#include "ProcessedArguments.h"
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

ProcessedArguments::ProcessedArguments(int argc, char* argv[]) : 
	ProcessedArguments(CommandLineArguments(argc, argv))
{}

ProcessedArguments::ProcessedArguments(CommandLineArguments arguments) :
	m_sceneFilePath           (DEFAULT_SCENE_FILE_PATH),
	m_imageFilePath           (DEFAULT_DEFAULT_IMAGE_FILE_PATH),
	m_numRenderThreads        (1),
	m_isPostProcessRequested  (true),
	m_isHelpMessageRequested  (false),
	m_isImageSeriesRequested  (false),
	m_wildcardStart           (""),
	m_wildcardFinish          (""),
	m_outputPercentageProgress(std::numeric_limits<float>::max()),

	m_isFrameDiagRequested(false)
{
	std::string imageFileFormat;
	while(!arguments.isEmpty())
	{
		const std::string argument = arguments.retrieveOne();

		if(argument == "-s")
		{
			m_sceneFilePath = arguments.retrieveOne();
		}
		else if(argument == "-o")
		{
			m_imageFilePath = arguments.retrieveOne();
		}
		else if(argument == "-of")
		{
			imageFileFormat = arguments.retrieveOne();
		}
		else if(argument == "-t")
		{
			const int numRenderThreads = arguments.retrieveOneInt(m_numRenderThreads);
			if(numRenderThreads > 0)
			{
				m_numRenderThreads = numRenderThreads;
			}
			else
			{
				std::cerr << "warning: bad number of threads <" << numRenderThreads << "> detected, "
				          << "using " << m_numRenderThreads << " instead" << std::endl;
			}
		}
		else if(argument == "-p")
		{
			const float outputPercentageProgress = arguments.retrieveOneFloat(m_outputPercentageProgress);
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
		else if(argument == "--raw")
		{
			m_isPostProcessRequested = false;
		}
		else if(argument == "--help")
		{
			m_isHelpMessageRequested = true;
		}
		else if(argument == "--series")
		{
			m_isImageSeriesRequested = true;
		}
		else if(argument == "--start")
		{
			m_wildcardStart = arguments.retrieveOne();
			if(m_wildcardStart.empty())
			{
				std::cerr << "warning: no wildcard string specified for --start" << std::endl;
			}
		}
		else if(argument == "--finish")
		{
			m_wildcardFinish = arguments.retrieveOne();
			if(m_wildcardFinish.empty())
			{
				std::cerr << "warning: no wildcard string specified for --finish" << std::endl;
			}
		}
		else if(argument == "-fd")
		{
			const auto values = arguments.retrieveMultiple(2);
			m_framePathA = values[0];
			m_framePathB = values[1];
			m_isFrameDiagRequested = true;
		}
		else
		{
			std::cerr << "warning: unknown command <" << argument << "> specified, ignoring" << std::endl;
		}
	}// end while more arguments exist

	// possibly override image format if a more specific order is given
	if(!imageFileFormat.empty())
	{
		m_imageFilePath += "." + imageFileFormat;
	}

	// TODO: check arguments
}

std::string ProcessedArguments::getSceneFilePath() const
{
	return m_sceneFilePath;
}

std::string ProcessedArguments::getImageFilePath() const
{
	return m_imageFilePath;
}

int ProcessedArguments::getNumRenderThreads() const
{
	return m_numRenderThreads;
}

bool ProcessedArguments::isPostProcessRequested() const
{
	return m_isPostProcessRequested;
}

bool ProcessedArguments::isHelpMessageRequested() const
{
	return m_isHelpMessageRequested;
}

bool ProcessedArguments::isImageSeriesRequested() const
{
	return m_isImageSeriesRequested;
}

std::string ProcessedArguments::wildcardStart() const
{
	return m_wildcardStart;
}

std::string ProcessedArguments::wildcardFinish() const
{
	return m_wildcardFinish;
}

float ProcessedArguments::getOutputPercentageProgress() const
{
	return m_outputPercentageProgress;
}

void ProcessedArguments::printHelpMessage()
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
-p <interval> <is_overwriting>

Output an intermediate image whenever the specified <interval> has passed, 
e.g., write 2.3% to output whenever the rendering has progressed 2.3 percent; 
or write 7s to output every 7 seconds. Specify <is_overwriting> as true will 
make the program overwrite previous intermediate image; false for the 
opposite effect.
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
