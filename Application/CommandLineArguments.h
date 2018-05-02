#pragma once

#include "util.h"

#include <string>
#include <vector>

PH_CLI_NAMESPACE_BEGIN

class CommandLineArguments final
{
public:
	static void printHelpMessage();

public:
	CommandLineArguments(const std::vector<std::string>& argv);

	std::string getSceneFilePath()     const;
	std::string getImageFilePath()     const;
	int         getNumRenderThreads()  const;
	bool        performPostPorcess()   const;
	bool        helpMessageRequested() const;

private:
	std::string m_sceneFilePath;
	std::string m_imageFilePath;
	int         m_numRenderThreads;
	bool        m_performPostProcess;
	bool        m_helpMessageRequested;
};

PH_CLI_NAMESPACE_END