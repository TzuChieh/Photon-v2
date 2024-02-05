#include "Common/Utility/CommandLineArguments.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

CommandLineArguments::CommandLineArguments(int argc, char* argv[]) : 
	m_programName(),
	m_arguments()
{
	if(argc >= 1)
	{
		m_programName = argv[0];
	}

	for(int i = 1; i < argc; ++i)
	{
		m_arguments.push_back(argv[i]);
	}
}

std::string CommandLineArguments::retrieveString(const std::string& defaultString)
{
	auto optString = retrieve<std::string>();
	return optString ? *optString : defaultString;
}

std::vector<std::string> CommandLineArguments::retrieveStrings(const std::size_t numValues)
{
	std::vector<std::string> arguments;
	for(std::size_t i = 0; i < numValues; ++i)
	{
		arguments.push_back(retrieveString());
	}
	return arguments;
}

std::vector<std::string> CommandLineArguments::retrieveOptionArguments(const std::string& optionPrefix)
{
	return retrieveStrings(optionPrefix, "-", false, false);
}

std::vector<std::string> CommandLineArguments::retrieveStrings(
	const std::string& startingPrefix,
	const std::string& endingPrefix,
	const bool shouldIncludeStart,
	const bool shouldIncludeEnd)
{
	auto startIter = std::find_if(
		m_arguments.begin(), m_arguments.end(), 
		[&startingPrefix](const std::string& argument)
		{
			// Using rfind() with pos=0 to limit the search to prefix only
			return argument.rfind(startingPrefix, 0) != std::string::npos;
		});

	auto endIter = std::find_if(
		startIter, m_arguments.end(),
		[&endingPrefix](const std::string& argument)
		{
			// Using rfind() with pos=0 to limit the search to prefix only
			return argument.rfind(endingPrefix, 0) != std::string::npos;
		});

	PH_ASSERT(startIter <= endIter);

	// By default includes start
	if(!shouldIncludeStart && startIter < endIter)
	{
		startIter += 1;
	}

	// By default excludes end
	if(shouldIncludeEnd && endIter != m_arguments.end())
	{
		endIter += 1;
	}

	PH_ASSERT(startIter <= endIter);

	std::vector<std::string> arguments(startIter, endIter);
	m_arguments.erase(startIter, endIter);
	return arguments;
}

}// end namespace ph
