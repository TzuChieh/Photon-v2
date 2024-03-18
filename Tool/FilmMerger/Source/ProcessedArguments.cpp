#include "ProcessedArguments.h"

#include <Common/logging.h>
#include <Common/assertion.h>
#include <Common/exceptions.h>

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

			PH_ASSERT(!m_filmInfos.empty());
			m_filmInfos.back().weight = arguments.retrieveFloat(1.0);
		}
		else
		{
			isParsingFilmInfo = true;

			m_filmInfos.push_back({.file = Path(argument)});
		}
	}// end while more arguments exist
}

}// end namespace ph::film_merger
