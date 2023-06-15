#pragma once

#include <DataIO/FileSystem/Path.h>

#include <string>
#include <string_view>
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
	void composeSceneWorkingDirectory();

	std::vector<char> m_sceneNameBuffer;
	Path m_baseWorkingDirectory;
	Path m_composedWorkingDirectory;
	std::string m_workingDirectoryPreview;
	std::string m_unsatisfactionMessage;
	bool m_withContainingFolder;
};

}// end namespace ph::editor
