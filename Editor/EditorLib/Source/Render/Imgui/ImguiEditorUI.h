#pragma once

#include "ThirdParty/DearImGuiExperimental.h"

namespace ph::editor
{

class Editor;
class ImguiHelper;

class ImguiEditorUI final
{
public:
	ImguiEditorUI();

	void initialize(Editor* editor, ImguiHelper* helper);
	void build();

private:
	void buildMainMenuBar();

	Editor* m_editor;
	ImguiHelper* m_helper;
	ImGuiID m_rootDockSpaceID;
	bool m_shouldResetRootDockSpace;
};

}// end namespace ph::editor
