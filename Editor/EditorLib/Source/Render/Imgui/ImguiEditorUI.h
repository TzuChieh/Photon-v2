#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"
#include "Render/Imgui/ImguiEditorTheme.h"
#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"
#include "Render/Imgui/Tool/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/Tool/ImguiSampleInspector.h"
#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Editor/ImguiEditorToolState.h"

#include "ThirdParty/DearImGuiExperimental.h"

#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/INoCopyAndMove.h>
#include <Utility/TUniquePtrVector.h>

#include <vector>
#include <cstddef>
#include <string>

namespace ph::editor
{

class Editor;
class DimensionHints;
class ImguiFontLibrary;
class ImguiImageLibrary;
class FileSystemExplorer;
class ImguiEditorSceneCreator;
class ImguiEditorDebugPanel;

class ImguiEditorUI final 
	// Copy makes no sense as almost all fields are unique to this instance and should not 
	// be duplicated. Move makes little sense as moving around editor UI is generally not needed.
	: private INoCopyAndMove
{
public:
	ImguiEditorUI(
		Editor& editor, 
		ImguiFontLibrary& fontLibrary, 
		ImguiImageLibrary& imageLibrary);

	~ImguiEditorUI();

	void build();

	Editor& getEditor();
	ImguiFontLibrary& getFontLibrary();
	ImguiImageLibrary& getImageLibrary();
	DimensionHints& getDimensionHints();
	bool isMain() const;
	ImguiFileSystemDialog& getGeneralFileSystemDialog();
	ImguiEditorObjectTypeMenu& getObjectTypeMenu();
	const ImguiEditorTheme& getTheme();

private:
	Editor& m_editor;
	ImguiFontLibrary& m_fontLibrary;
	ImguiImageLibrary& m_imageLibrary;

	static const ImguiEditorUI* mainEditorUI;

private:
	struct PanelEntry
	{
		ImguiEditorPanel* panel = nullptr;
		ImguiEditorPanel::Attributes attributes;
		std::string windowIdName;
		bool isOpening = false;
	};

	auto getPanelEntry(ImguiEditorPanel* panel) -> PanelEntry*;

	void buildMainMenuBar();
	void buildMainViewportWindow();
	void buildSidebarWindow();
	void buildToolbarWindow();
	void buildTool();
	void buildOpenSceneDialog();
	void buildStatsMonitor();
	void saveActiveScene();

	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;
	bool m_isOpeningScene;
	bool m_enableDebug;
	ImguiEditorToolState m_toolState;

	TUniquePtrVector<ImguiEditorPanel> m_panels;
	std::vector<PanelEntry> m_panelEntries;
	ImguiEditorSceneCreator* m_sceneCreator;
	ImguiEditorDebugPanel* m_debugPanel;
	//ImguiEditorImageViewer m_imageViewers;
	
	ImguiSampleInspector m_sampleInspector;
	ImguiFileSystemDialog m_generalFileSystemDialog;
	ImguiEditorObjectTypeMenu m_objectTypeMenu;
	ImguiEditorTheme m_theme;
};

inline Editor& ImguiEditorUI::getEditor()
{
	return m_editor;
}

inline ImguiFontLibrary& ImguiEditorUI::getFontLibrary()
{
	return m_fontLibrary;
}

inline ImguiImageLibrary& ImguiEditorUI::getImageLibrary()
{
	return m_imageLibrary;
}

inline ImguiFileSystemDialog& ImguiEditorUI::getGeneralFileSystemDialog()
{
	return m_generalFileSystemDialog;
}

inline ImguiEditorObjectTypeMenu& ImguiEditorUI::getObjectTypeMenu()
{
	return m_objectTypeMenu;
}

inline const ImguiEditorTheme& ImguiEditorUI::getTheme()
{
	return m_theme;
}

}// end namespace ph::editor
