#include "ProcessedArguments.h"

#include <Common/logging.h>
#include <Common/assertion.h>
#include <Common/exception.h>

#include <string_view>
#include <limits>

namespace ph::film_merger
{

ProcessedArguments::ProcessedArguments(int argc, char* argv[]) : 
	ProcessedArguments(CommandLineArguments(argc, argv))
{}

ProcessedArguments::ProcessedArguments(CommandLineArguments arguments)
	: m_filmInfos      ()
	, m_imageOutputPath("./merged_image")
	, m_imageFileFormat("exr")
	, m_isHelpRequested(false)
{
	bool isParsingFilmInfo = false;
	while(!arguments.isEmpty())
	{
		const std::string argument = arguments.retrieveString();

		if(argument == "-o")
		{
			isParsingFilmInfo = false;

			m_imageOutputPath = arguments.retrieveString();
		}
		else if(argument == "-of")
		{
			isParsingFilmInfo = false;

			m_imageFileFormat = arguments.retrieveString();
		}
		else if(argument == "--help")
		{
			isParsingFilmInfo = false;

			m_isHelpRequested = true;
		}
		else if(argument == "-w")
		{
			if(!isParsingFilmInfo)
			{
				throw InvalidArgumentException(
					"The weighting factor argument must follow a film path.");
			}

			const auto weightStr = arguments.retrieveString();
			if(weightStr.empty())
			{
				PH_DEFAULT_LOG(Warning,
					"Expecting a weighting factor, none was found.");
				continue;
			}

			PH_ASSERT(!m_filmInfos.empty());
			m_filmInfos.back().weight = std::stod(weightStr);
		}
		else
		{
			isParsingFilmInfo = true;

			m_filmInfos.push_back({.file = Path(argument)});
		}
	}// end while more arguments exist
}

}// end namespace ph::film_merger
