#pragma once

#include "util.h"
#include "CommandLineArguments.h"

#include <string>
#include <vector>

PH_CLI_NAMESPACE_BEGIN

class ProcessedArguments
{
public:
	static void printHelpMessage();

public:
	ProcessedArguments(int argc, char* argv[]);
	explicit ProcessedArguments(CommandLineArguments arguments);

	std::string getSceneFilePath()            const;
	std::string getImageFilePath()            const;
	int         getNumRenderThreads()         const;
	bool        isPostProcessRequested()      const;
	bool        isHelpMessageRequested()      const;
	bool        isImageSeriesRequested()      const;
	std::string wildcardStart()               const;
	std::string wildcardFinish()              const;
	float       getOutputPercentageProgress() const;

	bool isFrameDiagRequested() const
	{
		return m_isFrameDiagRequested;
	}

	std::string getFramePathA() const
	{
		return m_framePathA;
	}
	std::string getFramePathB() const
	{
		return m_framePathB;
	}

private:
	std::string m_sceneFilePath;
	std::string m_imageFilePath;
	std::string m_intermediateImageFileFormat;
	int         m_numRenderThreads;// FIXME: use unsigned integer
	bool        m_isPostProcessRequested;
	bool        m_isHelpMessageRequested;
	bool        m_isImageSeriesRequested;
	std::string m_wildcardStart;
	std::string m_wildcardFinish;
	float       m_outputPercentageProgress;

	bool m_isFrameDiagRequested;
	std::string m_framePathA;
	std::string m_framePathB;
};

PH_CLI_NAMESPACE_END