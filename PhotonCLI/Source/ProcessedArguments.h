#pragma once

#include "util.h"

#include <Common/primitive_type.h>
#include <Common/Utility/CommandLineArguments.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

namespace ph::cli
{

inline constexpr std::string_view DEFAULT_IMAGE_OUTPUT_STEM = "./rendered_scene";

enum class EIntervalUnit
{
	Percentage,
	Second
};

enum class EExecutionMode
{
	Help,
	SingleImage,
	ImageSeries,
	Blender
};

class ProcessedArguments
{
	// TODO: a parameter telling whether intermediate output is requested
	friend class StaticImageRenderer;

public:
	static void printHelpMessage();

public:
	ProcessedArguments(int argc, char* argv[]);
	explicit ProcessedArguments(CommandLineArguments arguments);

	EExecutionMode getExecutionMode() const;
	std::string    getSceneFilePath() const;
	std::string    getPrimaryImageOutputStem() const;
	std::string    getImageFilePath(int32 imageIndex, int32 numImages) const;
	std::string    getImageFileFormat() const;
	uint32         numThreads() const;
	bool           isPostProcessRequested() const;
	std::string    wildcardStart() const;
	std::string    wildcardFinish() const;
	bool           isIntermediateOutputRequested() const;
	float32        getIntermediateOutputInterval() const;
	EIntervalUnit  getIntermediateOutputIntervalUnit() const;
	bool           isOverwriteRequested() const;
	uint16         getPort() const;
	float32        getBlenderPeekInterval() const;
	EIntervalUnit  getBlenderPeekIntervalUnit() const;

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
	void setSceneFilePath(const std::string& sceneFilePath);
	void setImageOutputStem(const std::string& imageOutputStem);
	void setImageOutputStemOverrides(std::vector<std::string> imageOutputStemOverrides);
	std::string getImageOutputStem(int32 imageIndex, int32 numImages) const;

	EExecutionMode           m_executionMode;
	std::string              m_sceneFilePath;
	std::string              m_defaultImageOutputStem;
	std::vector<std::string> m_imageOutputStemOverrides;
	std::string              m_imageFileFormat;
	uint32                   m_numThreads;
	bool                     m_isPostProcessRequested;
	std::string              m_wildcardStart;
	std::string              m_wildcardFinish;
	bool                     m_isIntermediateOutputRequested;
	float32                  m_intermediateOutputInterval;
	EIntervalUnit            m_intermediateOutputIntervalUnit;
	bool                     m_isOverwriteRequested;
	uint16                   m_port;
	float32                  m_blenderPeekInterval;
	EIntervalUnit            m_blenderPeekIntervalUnit;

	// FIXME: this should move to other utilities such as the editor
	bool m_isFrameDiagRequested;
	std::string m_framePathA;
	std::string m_framePathB;
};

// In-header Implementations:

inline EExecutionMode ProcessedArguments::getExecutionMode() const
{
	return m_executionMode;
}

inline std::string ProcessedArguments::getSceneFilePath() const
{
	return m_sceneFilePath;
}

inline std::string ProcessedArguments::getPrimaryImageOutputStem() const
{
	return !m_imageOutputStemOverrides.empty() && !m_imageOutputStemOverrides.front().empty()
		? m_imageOutputStemOverrides.front()
		: m_defaultImageOutputStem;
}

inline std::string ProcessedArguments::getImageFileFormat() const
{
	return m_imageFileFormat;
}

inline uint32 ProcessedArguments::numThreads() const
{
	return m_numThreads;
}

inline bool ProcessedArguments::isPostProcessRequested() const
{
	return m_isPostProcessRequested;
}

inline std::string ProcessedArguments::wildcardStart() const
{
	return m_wildcardStart;
}

inline std::string ProcessedArguments::wildcardFinish() const
{
	return m_wildcardFinish;
}

inline bool ProcessedArguments::isIntermediateOutputRequested() const
{
	return m_isIntermediateOutputRequested;
}

inline float32 ProcessedArguments::getIntermediateOutputInterval() const
{
	return m_intermediateOutputInterval;
}

inline EIntervalUnit ProcessedArguments::getIntermediateOutputIntervalUnit() const
{
	return m_intermediateOutputIntervalUnit;
}

inline bool ProcessedArguments::isOverwriteRequested() const
{
	return m_isOverwriteRequested;
}

inline uint16 ProcessedArguments::getPort() const
{
	return m_port;
}

inline float32 ProcessedArguments::getBlenderPeekInterval() const
{
	return m_blenderPeekInterval;
}

inline EIntervalUnit ProcessedArguments::getBlenderPeekIntervalUnit() const
{
	return m_blenderPeekIntervalUnit;
}

inline void ProcessedArguments::setSceneFilePath(const std::string& sceneFilePath)
{
	m_sceneFilePath = sceneFilePath;
}

inline void ProcessedArguments::setImageOutputStem(const std::string& imageOutputStem)
{
	m_defaultImageOutputStem = imageOutputStem;
	m_imageOutputStemOverrides.clear();
}

inline void ProcessedArguments::printHelpMessage()
{
	std::cout << R"(
===============================================================================
[-s <path>]
 
Specify path to scene file. To render an image series, you can specify
"myScene*.p2" as <path> where * is a wildcard for any string (--series is
required in this case). 
(default path: "./scene.p2")
===============================================================================
[-o <path>]

Specify output stem(s), e.g., "-o beauty,variance". Empty or omitted stems use
the default name for that image index. In --series mode, this is the output
directory.
(default stem: "./rendered_scene")
===============================================================================
[-of <format>]

Specify the format of output image. Supported formats are: png, jpg, bmp, tga,
hdr, exr, pfm.
(default format: png)
===============================================================================
[-t <number>]

Set number of threads used for rendering. 
(default: single thread)
===============================================================================
[-p <interval> <is_overwriting>]

Output an intermediate image whenever the specified <interval> has passed, 
e.g., write 2.3% to output whenever the rendering has progressed 2.3 percent; 
or write 7s to output every 7 seconds. Specify <is_overwriting> as true will 
make the program overwrite previous intermediate image; false for the 
opposite effect.
===============================================================================
[--raw]

Do not perform any post-processing. 
(default: perform post-processing)
===============================================================================
[--help]

Print this help message then exit.
===============================================================================
[--series]

Render an image series. The order for rendering will be lexicographical order
of the wildcarded string. Currently only .png is supported.
===============================================================================
[--start <*>]

Render image series starting from a specific wildcarded string.
===============================================================================
[--finish <*>]

Render image series until a specific wildcarded string is matched. (inclusive)
===============================================================================
[--blender <peek_interval>]

Tailor the renderer to support for use with Blender. <peek_interval> determines
how often intermediate image is retrieved from the renderer to display in
Blender (default to 1s).
===============================================================================
[--port <number>]

The port to use when running as a server.
(default: 7000)
===============================================================================
	)" << std::endl;
}

}// end namespace ph::cli
