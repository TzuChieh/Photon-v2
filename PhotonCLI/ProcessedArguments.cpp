#include "ProcessedArguments.h"
#include "util.h"

#include <iostream>
#include <string_view>
#include <limits>

namespace ph::cli
{

ProcessedArguments::ProcessedArguments(int argc, char* argv[]) : 
	ProcessedArguments(CommandLineArguments(argc, argv))
{}

ProcessedArguments::ProcessedArguments(CommandLineArguments arguments) :
	m_sceneFilePath             ("./scene.p2"),
	m_imageOutputPath           ("./rendered_scene"),
	m_imageFileFormat           ("png"),
	m_numRenderThreads          (1),
	m_isPostProcessRequested    (true),
	m_isHelpMessageRequested    (false),
	m_isImageSeriesRequested    (false),
	m_wildcardStart             (""),
	m_wildcardFinish            (""),
	m_intermediateOutputInverval(std::numeric_limits<float>::max()),
	m_intervalUnit              (EIntervalUnit::PERCENTAGE),
	m_isOverwriteRequested      (false),

	// HACK
	m_isFrameDiagRequested(false),
	m_port(0)
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
			const auto values = arguments.retrieveMultiple(2);

			m_isOverwriteRequested = (values[1] == "true" || values[1] == "TRUE");

			if(values[0].length() >= 2)
			{
				const std::string inverval = values[0].substr(0, values[0].length() - 1);
				m_intermediateOutputInverval = std::stof(inverval);

				const char unit = values[0].back();
				if(unit == '%')
				{
					m_intervalUnit = EIntervalUnit::PERCENTAGE;
				}
				else if(unit == 's')
				{
					m_intervalUnit = EIntervalUnit::SECOND;
				}
				else
				{
					std::cerr << "warning: unknown intermediate output interval unit <"
					          << values[0] << "> specified" << std::endl;
				}
			}
			else
			{
				std::cerr << "warning: unrecognizable intermediate output interval <"
				          << values[0] << "> specified" << std::endl;
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
		else if(argument == "--port")
		{
			m_port = static_cast<unsigned short>(arguments.retrieveOneInt());
		}
		else
		{
			std::cerr << "warning: unknown command <" << argument << "> specified, ignoring" << std::endl;
		}
	}// end while more arguments exist

	// TODO: argument sanity check
}

}// end namespace ph::cli
