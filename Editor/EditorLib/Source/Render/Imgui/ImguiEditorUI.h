#pragma once

#include "Render/Imgui/Editor/ImguiEditorSidebarState.h"
#include "Render/Imgui/Editor/ImguiEditorLog.h"
#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "Render/Imgui/Editor/ImguiEditorAssetBrowser.h"
#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"

#include "ThirdParty/DearImGuiExperimental.h"

#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/INoCopyAndMove.h>

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

class ImguiEditorUI final 
	// Copy makes no sense as almost all fields are unique to this instance and should not 
	// be duplicated. Move makes little sense as moving around editor UI is generally not needed.
	: private INoCopyAndMove
{
public:
	ImguiEditorUI();
	~ImguiEditorUI();

	void initialize(Editor* editor, ImguiFontLibrary* fontLibrary, ImguiImageLibrary* imageLibrary);
	void build();

	Editor& getEditor();
	ImguiFontLibrary& getFontLibrary();
	ImguiImageLibrary& getImageLibrary();
	DimensionHints& getDimensionHints();
	bool isMainEditor() const;
	ImguiFileSystemDialog& getGeneralFileSystemDialog();

private:
	Editor* m_editor;
	ImguiFontLibrary* m_fontLibrary;
	ImguiImageLibrary* m_imageLibrary;

	static const ImguiEditorUI* mainEditor;

private:
	void buildMainMenuBar();
	void buildAssetBrowserWindow();
	void buildRootPropertiesWindow();
	void buildObjectBrowserWindow();
	void buildMainViewportWindow();
	void buildSidebarWindow();

	void buildSceneManagerWindow();
	void buildEditorSettingsWindow();
	void buildLogWindow();
	void buildStatsMonitor();
	void buildImguiDemo();

	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;
	bool m_shouldShowImguiDemo;
	ImguiEditorSidebarState m_sidebarState;
	ImguiEditorLog m_editorLog;
	ImguiEditorSceneManager m_sceneManager;
	ImguiEditorAssetBrowser m_assetBrowser;

	ImguiFileSystemDialog m_generalFileSystemDialog;
};

}// end namespace ph::editor
