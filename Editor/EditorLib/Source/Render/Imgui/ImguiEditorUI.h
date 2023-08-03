#pragma once

#include "Render/Imgui/Editor/ImguiEditorSidebarState.h"
#include "Render/Imgui/Editor/ImguiEditorLog.h"
#include "Render/Imgui/Editor/ImguiEditorSceneCreator.h"
#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "Render/Imgui/Editor/ImguiEditorSettings.h"
#include "Render/Imgui/Editor/ImguiEditorSceneObjectBrowser.h"
#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Editor/ImguiEditorAssetBrowser.h"
#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"
#include "Render/Imgui/Editor/ImguiEditorTheme.h"
#include "Render/Imgui/Editor/ImguiEditorDebugPanel.h"
#include "Render/Imgui/Tool/ImguiSampleInspector.h"

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

	const ImguiEditorTheme& getTheme();

private:
	Editor* m_editor;
	ImguiFontLibrary* m_fontLibrary;
	ImguiImageLibrary* m_imageLibrary;

	static const ImguiEditorUI* mainEditor;

private:
	void buildMainMenuBar();
	void buildAssetBrowserWindow();
	void buildRootPropertyWindow();
	void buildObjectBrowserWindow();
	void buildMainViewportWindow();
	void buildSidebarWindow();
	void buildToolbarWindow();

	void buildSceneCreatorWindow();
	void buildSceneManagerWindow();
	void buildEditorSettingsWindow();
	void buildLogWindow();
	void buildStatsMonitor();
	void buildDebugPanelWindow();
	void buildTool();

	void buildOpenSceneDialog();
	void saveActiveScene();

	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;
	bool m_shouldShowSceneCreator;
	bool m_shouldShowDearImGuiDemo;
	bool m_shouldShowImPlotDemo;
	ImguiEditorSidebarState m_sidebarState;
	ImguiEditorLog m_editorLog;
	ImguiEditorSceneCreator m_sceneCreator;
	ImguiEditorSceneManager m_sceneManager;
	ImguiEditorSettings m_editorSettings;
	ImguiEditorSceneObjectBrowser m_sceneObjectBrowser;
	ImguiEditorPropertyPanel m_rootPropertyPanel;
	ImguiEditorAssetBrowser m_assetBrowser;
	ImguiEditorDebugPanel m_debugPanel;
	ImguiSampleInspector m_sampleInspector;

	bool m_isOpeningScene;
	bool m_isOnDebugMode;

	ImguiFileSystemDialog m_generalFileSystemDialog;

	ImguiEditorTheme m_theme;
};

inline const ImguiEditorTheme& ImguiEditorUI::getTheme()
{
	return m_theme;
}

}// end namespace ph::editor
