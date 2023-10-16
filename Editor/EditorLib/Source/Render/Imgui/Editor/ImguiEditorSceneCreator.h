#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

#include <DataIO/FileSystem/Path.h>

#include <string>
#include <string_view>
#include <vector>

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiEditorSceneCreator : public ImguiEditorPanel
{
public:
	explicit ImguiEditorSceneCreator(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	void summarizeInitialContent();
	void composeSceneWorkingDirectory();

	std::vector<char> m_sceneNameBuffer;
	bool m_withInitialSceneDescription;
	Path m_initialSceneDescription;
	std::string m_initialContentSummary;
	Path m_baseWorkingDirectory;
	Path m_composedWorkingDirectory;
	std::string m_workingDirectoryPreview;
	std::string m_unsatisfactionMessage;
	bool m_withContainingFolder;
};

}// end namespace ph::editor
