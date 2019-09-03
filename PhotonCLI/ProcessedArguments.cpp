#include "ProcessedArguments.h"
#include "util.h"

#include <iostream>
#include <string_view>
#include <limits>

PH_CLI_NAMESPACE_BEGIN

ProcessedArguments::ProcessedArguments(int argc, char* argv[]) : 
	ProcessedArguments(CommandLineArguments(argc, argv))
{}

ProcessedArguments::ProcessedArguments(CommandLineArguments arguments) :
	m_sceneFilePath           ("./scene.p2"),
	m_imageOutputPath         ("./rendered_scene"),
	m_imageFileFormat         ("png"),
	m_numRenderThreads        (1),
	m_isPostProcessRequested  (true),
	m_isHelpMessageRequested  (false),
	m_isImageSeriesRequested  (false),
	m_wildcardStart           (""),
	m_wildcardFinish          (""),
	m_outputPercentageProgress(std::numeric_limits<float>::max()),

	// HACK
	m_isFrameDiagRequested(false)
{
	while(!arguments.isEmpty())
	{
		const std::string argument = arguments.retrieveOne();

		if(argument == "-s")
		{
			m_sceneFilePath = arguments.retrieveOne();
		}
		else if(argument == "-o")
		{
			m_imageOutputPath = arguments.retrieveOne();
		}
		else if(argument == "-of")
		{
			m_imageFileFormat = arguments.retrieveOne();
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
}

PH_CLI_NAMESPACE_END
