#include "ProcessedArguments.h"

#include <Common/logging.h>

#include <string_view>
#include <limits>

namespace ph::cli
{

PH_DEFINE_INTERNAL_LOG_GROUP(PhotonCliArgs, PhotonCLI);

ProcessedArguments::ProcessedArguments(int argc, char* argv[]) : 
	ProcessedArguments(CommandLineArguments(argc, argv))
{}

ProcessedArguments::ProcessedArguments(CommandLineArguments arguments)
	: m_executionMode                 (EExecutionMode::SingleImage)
	, m_sceneFilePath                 ("./scene.p2")
	, m_imageOutputPath               ("./rendered_scene")
	, m_imageFileFormat               ("png")
	, m_numThreads                    (1)
	, m_isPostProcessRequested        (true)
	, m_wildcardStart                 ("")
	, m_wildcardFinish                ("")
	, m_intermediateOutputInterval    (std::numeric_limits<float32>::max())
	, m_intermediateOutputIntervalUnit(EIntervalUnit::Percentage)
	, m_isOverwriteRequested          (false)
	, m_port                          (7000)
	, m_blenderPeekInterval           (1.0f)
	, m_blenderPeekIntervalUnit       (EIntervalUnit::Second)

	// HACK
	, m_isFrameDiagRequested(false)
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
				PH_LOG(PhotonCliArgs, Warning,
					"bad number of threads {} detected, using {} instead", numThreads, m_numThreads);
			}
		}
		else if(argument == "-p")
		{
			const auto values = arguments.retrieveStrings(2);

			m_isOverwriteRequested = (values[1] == "true" || values[1] == "TRUE");

			if(values[0].length() >= 2)
			{
				const std::string interval = values[0].substr(0, values[0].length() - 1);
				m_intermediateOutputInterval = std::stof(interval);

				const char unit = values[0].back();
				if(unit == '%')
				{
					m_intermediateOutputIntervalUnit = EIntervalUnit::Percentage;
				}
				else if(unit == 's')
				{
					m_intermediateOutputIntervalUnit = EIntervalUnit::Second;
				}
				else
				{
					PH_LOG(PhotonCliArgs, Warning,
						"unknown intermediate output interval unit <{}> specified", unit);
				}
			}
			else
			{
				PH_LOG(PhotonCliArgs, Warning,
					"unrecognizable intermediate output interval <{}> specified", values[0]);
			}
		}
		else if(argument == "--raw")
		{
			m_isPostProcessRequested = false;
		}
		else if(argument == "--help")
		{
			m_executionMode = EExecutionMode::Help;
		}
		else if(argument == "--series")
		{
			m_executionMode = EExecutionMode::ImageSeries;
		}
		else if(argument == "--blender")
		{
			m_executionMode = EExecutionMode::Blender;

			const std::string intervalWithUnit = arguments.retrieveString();
			if(intervalWithUnit.length() >= 2)
			{
				const std::string interval = intervalWithUnit.substr(0, intervalWithUnit.length() - 1);
				m_blenderPeekInterval = std::stof(interval);

				const char unit = intervalWithUnit.back();
				if(unit == '%')
				{
					m_blenderPeekIntervalUnit = EIntervalUnit::Percentage;
				}
				else if(unit == 's')
				{
					m_blenderPeekIntervalUnit = EIntervalUnit::Second;
				}
				else
				{
					PH_LOG(PhotonCliArgs, Warning,
						"unknown blender peek interval unit <{}> specified", unit);
				}
			}
			else
			{
				PH_LOG(PhotonCliArgs, Warning,
					"unrecognizable blender peek interval <{}> specified", intervalWithUnit);
			}
		}
		else if(argument == "--start")
		{
			m_wildcardStart = arguments.retrieveString();
			if(m_wildcardStart.empty())
			{
				PH_LOG(PhotonCliArgs, Warning, "no wildcard string specified for --start");
			}
		}
		else if(argument == "--finish")
		{
			m_wildcardFinish = arguments.retrieveString();
			if(m_wildcardFinish.empty())
			{
				PH_LOG(PhotonCliArgs, Warning, "no wildcard string specified for --finish");
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
			PH_LOG(PhotonCliArgs, Warning, "unknown command <{}> specified, ignoring",
				argument);
		}
	}// end while more arguments exist

	// TODO: argument sanity check
}

}// end namespace ph::cli
