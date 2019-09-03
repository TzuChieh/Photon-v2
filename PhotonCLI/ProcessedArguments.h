#pragma once

#include "util.h"
#include "CommandLineArguments.h"

#include <string>
#include <vector>
#include <iostream>

PH_CLI_NAMESPACE_BEGIN

class ProcessedArguments
{
public:
	static void printHelpMessage();

public:
	ProcessedArguments(int argc, char* argv[]);
	explicit ProcessedArguments(CommandLineArguments arguments);

	std::string getSceneFilePath() const;
	std::string getImageOutputPath() const;
	std::string getImageFilePath() const;
	std::string getImageFileFormat() const;
	int         getNumRenderThreads() const;
	bool        isPostProcessRequested() const;
	bool        isHelpMessageRequested() const;
	bool        isImageSeriesRequested() const;
	std::string wildcardStart() const;
	std::string wildcardFinish() const;
	float       getOutputPercentageProgress() const;

	void setSceneFilePath(const std::string& sceneFilePath);
	void setImageOutputPath(const std::string& imageOutputPath);

	// TODO: other setters

	// HACK
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
	std::string m_imageOutputPath;
	std::string m_imageFileFormat;
	int         m_numRenderThreads;// FIXME: use unsigned integer
	bool        m_isPostProcessRequested;
	bool        m_isHelpMessageRequested;
	bool        m_isImageSeriesRequested;
	std::string m_wildcardStart;
	std::string m_wildcardFinish;
	float       m_outputPercentageProgress;

	// HACK
	bool m_isFrameDiagRequested;
	std::string m_framePathA;
	std::string m_framePathB;
};

// In-header Implementations:

inline std::string ProcessedArguments::getSceneFilePath() const
{
	return m_sceneFilePath;
}

inline std::string ProcessedArguments::getImageOutputPath() const
{
	return m_imageOutputPath;
}

inline std::string ProcessedArguments::getImageFilePath() const
{
	return m_imageOutputPath + "." + m_imageFileFormat;
}

inline std::string ProcessedArguments::getImageFileFormat() const
{
	return m_imageFileFormat;
}

inline int ProcessedArguments::getNumRenderThreads() const
{
	return m_numRenderThreads;
}

inline bool ProcessedArguments::isPostProcessRequested() const
{
	return m_isPostProcessRequested;
}

inline bool ProcessedArguments::isHelpMessageRequested() const
{
	return m_isHelpMessageRequested;
}

inline bool ProcessedArguments::isImageSeriesRequested() const
{
	return m_isImageSeriesRequested;
}

inline std::string ProcessedArguments::wildcardStart() const
{
	return m_wildcardStart;
}

inline std::string ProcessedArguments::wildcardFinish() const
{
	return m_wildcardFinish;
}

inline float ProcessedArguments::getOutputPercentageProgress() const
{
	return m_outputPercentageProgress;
}

inline void ProcessedArguments::setSceneFilePath(const std::string& sceneFilePath)
{
	m_sceneFilePath = sceneFilePath;
}

inline void ProcessedArguments::setImageOutputPath(const std::string& imageOutputPath)
{
	m_imageOutputPath = imageOutputPath;
}

inline void ProcessedArguments::printHelpMessage()
{
	std::cout << R"(
===============================================================================
-s <path>
 
Specify path to scene file. To render an image series, you can specify
"myScene*.p2" as <path> where * is a wildcard for any string (--series is
required in this case). 
(default path: "./scene.p2")
===============================================================================
-o <path>

Specify image output path. This should be a filename (without extension) for 
single image or a directory for image series. 
(default path: "./rendered_scene")
===============================================================================
-of <format>

Specify the format of output image. Supported formats are: png, jpg, bmp, tga,
hdr, exr.
(default format: png)
===============================================================================
-t <number>

Set number of threads used for rendering. 
(default: single thread)
===============================================================================
-p <interval> <is_overwriting>

Output an intermediate image whenever the specified <interval> has passed, 
e.g., write 2.3% to output whenever the rendering has progressed 2.3 percent; 
or write 7s to output every 7 seconds. Specify <is_overwriting> as true will 
make the program overwrite previous intermediate image; false for the 
opposite effect.
===============================================================================
--raw

Do not perform any post-processing. 
(default: perform post-processing)
===============================================================================
--help

Print this help message then exit.
===============================================================================
--series

Render an image series. The order for rendering will be lexicographical order
of the wildcarded string. Currently only .png is supported.
===============================================================================
--start <*>

Render image series starting from a specific wildcarded string.
===============================================================================
--finish <*>

Render image series until a specific wildcarded string is matched. (inclusive)
===============================================================================
	)" << std::endl;
}

PH_CLI_NAMESPACE_END