#pragma once

#include <DataIO/FileSystem/Path.h>

namespace ph::editor
{

class RenderConfig final
{
public:
	Path sceneFile;
	Path sceneWorkingDirectory;
	bool useCopiedScene = true;
	bool enableStatsRequest = true;
	bool enablePeekingFrame = true;
};

}// end namespace ph::editor
