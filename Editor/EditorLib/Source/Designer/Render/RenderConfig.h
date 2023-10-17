#pragma once

#include <Common/primitive_type.h>
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
	std::string outputName = "rendered_scene";
	EPictureFile outputFileFormat = EPictureFile::EXR;
	uint32 minStatsRequestPeriodMs = 100;
	uint32 minFramePeekPeriodMs = 300;
	bool useCopiedScene = true;
	bool enableStatsRequest = true;

	/*! @brief Whether rendered frame can be viewed from the editor.
	Disabling this will improve performance slightly (recommended for overnight renders).
	*/
	bool enablePeekingFrame = true;

	bool performToneMapping = true;
};

}// end namespace ph::editor
