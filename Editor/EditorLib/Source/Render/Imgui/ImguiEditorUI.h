#pragma once

#include "ThirdParty/DearImGuiExperimental.h"

namespace ph::editor
{

class Editor;
class ImguiFontLibrary;

class ImguiEditorUI final
{
public:
	ImguiEditorUI();

	void initialize(Editor* editor, ImguiFontLibrary* fontLibrary);
	void build();

private:
	void buildMainMenuBar();

	Editor* m_editor;
	ImguiFontLibrary* m_fontLibrary;
	ImGuiID m_rootDockSpaceID;
	bool m_shouldResetRootDockSpace;
};

}// end namespace ph::editor
