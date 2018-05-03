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

	std::string getSceneFilePath()       const;
	std::string getImageFilePath()       const;
	int         getNumRenderThreads()    const;
	bool        isPostProcessRequested() const;
	bool        isHelpMessageRequested() const;
	bool        isImageSeriesRequested() const;
	std::string wildcardBegin()          const;
	std::string wildcardEnd()            const;

private:
	std::string m_sceneFilePath;
	std::string m_imageFilePath;
	int         m_numRenderThreads;
	bool        m_isPostProcessRequested;
	bool        m_isHelpMessageRequested;
	bool        m_isImageSeriesRequested;
	std::string m_wildcardBegin;
	std::string m_wildcardEnd;
};

PH_CLI_NAMESPACE_END