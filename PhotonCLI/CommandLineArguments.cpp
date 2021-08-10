#include "CommandLineArguments.h"

#include <iostream>

namespace ph::cli
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

std::string CommandLineArguments::retrieveOne(const std::string& defaultValue)
{
	if(isEmpty())
	{
		std::cerr << "warning: retrieving a nonexistent argument, " 
		          << "defaulted to " << defaultValue << std::endl;
		return defaultValue;
	}

	std::string argument = m_arguments.front();
	m_arguments.pop();
	return argument;
}

std::vector<std::string> CommandLineArguments::retrieveMultiple(const std::size_t numValues)
{
	std::vector<std::string> arguments;
	for(std::size_t i = 0; i < numValues; ++i)
	{
		arguments.push_back(retrieveOne());
	}
	return arguments;
}

int CommandLineArguments::retrieveOneInt(const int defaultValue)
{
	const auto argument = retrieveOne();
	return !argument.empty() ? std::stoi(argument) : defaultValue;
}

float CommandLineArguments::retrieveOneFloat(const float defaultValue)
{
	const auto argument = retrieveOne();
	return !argument.empty() ? std::stof(argument) : defaultValue;
}

}// end namespace ph::cli
