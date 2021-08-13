#include "Utility/CommandLineArguments.h"

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
		m_arguments.push(argv[i]);
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

std::vector<std::string> CommandLineArguments::retrieveStrings(
	const std::string& startingPrefix,
	const std::string& endingPrefix,
	const bool shouldIncludeStart,
	const bool shouldIncludeEnd)
{
	std::vector<std::string> arguments;
	while(!isEmpty())
	{
		auto argument = retrieveString();
		
	}

	for(std::size_t i = 0; i < numValues; ++i)
	{
		arguments.push_back(retrieveString());
	}
	return arguments;
}

}// end namespace ph
