#pragma once

#include "ThirdParty/DearImGuiExperimental.h"

namespace ph::editor
{

class Editor;
class ImguiFontLibrary;
class ImguiImageLibrary;

class ImguiEditorUI final
{
public:
	ImguiEditorUI();

	void initialize(Editor* editor, ImguiFontLibrary* fontLibrary, ImguiImageLibrary* imageLibrary);
	void build();

private:
	void buildMainMenuBar();

	Editor* m_editor;
	ImguiFontLibrary* m_fontLibrary;
	ImguiImageLibrary* m_imageLibrary;
	ImGuiID m_rootDockSpaceID;
	bool m_shouldResetRootDockSpace;
};

}// end namespace ph::editor
