#include "ProcessedArguments.h"
#include "util.h"
#include "Common/logging.h"

#include <iostream>
#include <string_view>
#include <limits>

namespace ph::cli
{

PH_DEFINE_INTERNAL_LOG_GROUP(PhotonCliArgs, PhotonCLI);

ProcessedArguments::ProcessedArguments(int argc, char* argv[]) : 
	ProcessedArguments(CommandLineArguments(argc, argv))
{}

ProcessedArguments::ProcessedArguments(CommandLineArguments arguments) :
	m_sceneFilePath             ("./scene.p2"),
	m_imageOutputPath           ("./rendered_scene"),
	m_imageFileFormat           ("png"),
	m_numThreads                (1),
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
		const std::string argument = arguments.retrieveString();

		if(argument == "-s")
		{
			m_sceneFilePath = arguments.retrieveString();
		}
		else if(argument == "-o")
		{
			m_imageOutputPath = arguments.retrieveString();
		}
		else if(argument == "-of")
		{
			m_imageFileFormat = arguments.retrieveString();
		}
		else if(argument == "-t")
		{
			const int numThreads = arguments.retrieveInt(m_numThreads);
			if(numThreads > 0)
			{
				m_numThreads = numThreads;
			}
			else
			{
				PH_LOG_WARNING(PhotonCliArgs, "bad number of threads {} detected, using {} instead",
					numThreads, m_numThreads);
			}
		}
		else if(argument == "-p")
		{
			const auto values = arguments.retrieveStrings(2);

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
					PH_LOG_WARNING(PhotonCliArgs, "unknown intermediate output interval unit <{}> specified",
						values[0]);
				}
			}
			else
			{
				PH_LOG_WARNING(PhotonCliArgs, "unrecognizable intermediate output interval <{}> specified",
					values[0]);
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
			m_wildcardStart = arguments.retrieveString();
			if(m_wildcardStart.empty())
			{
				PH_LOG_WARNING(PhotonCliArgs, "no wildcard string specified for --start");
			}
		}
		else if(argument == "--finish")
		{
			m_wildcardFinish = arguments.retrieveString();
			if(m_wildcardFinish.empty())
			{
				PH_LOG_WARNING(PhotonCliArgs, "no wildcard string specified for --finish");
			}
		}
		else if(argument == "-fd")
		{
			const auto values = arguments.retrieveStrings(2);
			m_framePathA = values[0];
			m_framePathB = values[1];
			m_isFrameDiagRequested = true;
		}
		else if(argument == "--port")
		{
			m_port = arguments.retrieveInt<unsigned short>();
		}
		else
		{
			PH_LOG_WARNING(PhotonCliArgs, "unknown command <{}> specified, ignoring",
				argument);
		}
	}// end while more arguments exist

	// TODO: argument sanity check
}

}// end namespace ph::cli
