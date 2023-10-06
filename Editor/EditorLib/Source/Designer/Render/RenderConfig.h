#pragma once

#include <DataIO/FileSystem/Path.h>
#include <DataIO/EPictureFile.h>

#include <string>

namespace ph::editor
{

class RenderConfig final
{
public:
	Path sceneFile;
	Path sceneWorkingDirectory;
	Path outputDirectory;
	std::string outputName;
	EPictureFile outputFileFormat;
	bool useCopiedScene = true;
	bool enableStatsRequest = true;
	bool enablePeekingFrame = true;
};

}// end namespace ph::editor
