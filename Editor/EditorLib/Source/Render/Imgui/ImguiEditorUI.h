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
	void buildStatsMonitor();

	Editor* m_editor;
	ImguiFontLibrary* m_fontLibrary;
	ImguiImageLibrary* m_imageLibrary;
	ImGuiID m_rootDockSpaceID;
	ImGuiID m_leftDockSpaceID;
	ImGuiID m_rightDockSpaceID;
	ImGuiID m_bottomDockSpaceID;
	ImGuiID m_centerDockSpaceID;
	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;

private:
	static const char* const rootPropertiesWindowName;
	static const char* const mainViewportWindowName;
	static const char* const assetBrowserWindowName;
	static const char* const objectBrowserWindowName;
};

}// end namespace ph::editor
