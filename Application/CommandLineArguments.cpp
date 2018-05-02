#include "CommandLineArguments.h"
#include "util.h"

#include <iostream>

PH_CLI_NAMESPACE_BEGIN

CommandLineArguments::CommandLineArguments(const std::vector<std::string>& argv) : 
	m_sceneFilePath       ("./scene.p2"),
	m_imageFilePath       ("./rendered_scene.png"),
	m_numRenderThreads    (1),
	m_performPostProcess  (true),
	m_helpMessageRequested(false)
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
		else if(argv[i] == "-i")
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
			m_performPostProcess = false;
		}
		else if(argv[i] == "--help")
		{
			m_helpMessageRequested = true;
		}
		else
		{
			std::cerr << "warning: unknown command <" << argv[i] << ">" << std::endl;
			std::cerr << "ignored" << std::endl;
		}
	}
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

bool CommandLineArguments::performPostPorcess() const
{
	return m_performPostProcess;
}

bool CommandLineArguments::helpMessageRequested() const
{
	return m_helpMessageRequested;
}

void CommandLineArguments::printHelpMessage()
{
	std::cout << R"(

	-s <path>      specify the scene file to render
	-i <path>      specify the output image
	-t <number>    number of thread for rendering
	--raw          do not perform any post-processing
	--help         print this help message then exit

	)" << std::endl;
}

PH_CLI_NAMESPACE_END
