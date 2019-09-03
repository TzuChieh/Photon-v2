#pragma once

#include "util.h"

#include <string>
#include <queue>
#include <vector>
#include <cstddef>

PH_CLI_NAMESPACE_BEGIN

class CommandLineArguments
{
public:
	CommandLineArguments(int argc, char* argv[]);

	std::string getProgramName() const;
	bool isEmpty() const;
	std::string retrieveOne(const std::string& defaultValue = "");
	std::vector<std::string> retrieveMultiple(std::size_t numValues);
	int retrieveOneInt(int defaultValue = 0);
	float retrieveOneFloat(float defaultValue = 0.0f);

private:
	std::string             m_programName;
	std::queue<std::string> m_arguments;
};

// In-header Implementations:

inline std::string CommandLineArguments::getProgramName() const
{
	return m_programName;
}

inline bool CommandLineArguments::isEmpty() const
{
	return m_arguments.empty();
}

PH_CLI_NAMESPACE_END