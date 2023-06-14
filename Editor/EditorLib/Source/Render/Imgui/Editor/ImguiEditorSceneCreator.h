#pragma once

#include <DataIO/FileSystem/Path.h>

#include <string>
#include <vector>

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorSceneCreator final
{
public:
	ImguiEditorSceneCreator();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	std::vector<char> m_sceneNameBuffer;
	Path m_sceneWorkingDirectory;
	std::string m_sceneWorkingDirectoryPreview;
	bool m_withContainingFolder;
};

}// end namespace ph::editor
