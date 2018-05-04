#include "CommandLineArguments.h"
#include "util.h"

#include <iostream>

PH_CLI_NAMESPACE_BEGIN

CommandLineArguments::CommandLineArguments(const std::vector<std::string>& argv) : 
	m_sceneFilePath         ("./scene.p2"),
	m_imageFilePath         ("./rendered_scene.png"),
	m_numRenderThreads      (1),
	m_isPostProcessRequested(true),
	m_isHelpMessageRequested(false),
	m_isImageSeriesRequested(false),
	m_wildcardStart         (""),
	m_wildcardFinish        ("")
{
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

void CommandLineArguments::printHelpMessage()
{
	std::cout << R"(

	-s <path>      Specify path to scene file. To render an image series, you
	               can specify "myScene*.p2" as <path> where * is a wildcard 
	               for any string (and specify --series).

	-o <path>      Specify image output path. This should be a filename for
	               single image and a path for image series.

	-t <number>    Set number of threads for rendering.

	--raw          Do not perform any post-processing.

	--help         Print this help message then exit.

	--series       Render an image series. The order for rendering will be 
	               lexicographical order of the wildcarded string.

	--start <*>    Render image series starting from a specific wildcarded 
	               string.

	--finish <*>   Render image series until a specific wildcarded string is
	               matched. (inclusive)

	)" << std::endl;
}

PH_CLI_NAMESPACE_END
