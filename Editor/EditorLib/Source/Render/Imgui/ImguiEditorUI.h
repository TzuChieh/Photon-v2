#pragma once

#include "Render/Imgui/Editor/ImguiEditorSidebarState.h"
#include "Render/Imgui/Editor/ImguiEditorLog.h"
#include "Render/Imgui/Editor/ImguiEditorSceneCreator.h"
#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "Render/Imgui/Editor/ImguiEditorAssetBrowser.h"
#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"
#include "Render/Imgui/Editor/ImguiEditorTheme.h"

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
	void buildRootPropertiesWindow();
	void buildObjectBrowserWindow();
	void buildMainViewportWindow();
	void buildSidebarWindow();
	void buildToolbarWindow();

	void buildSceneCreatorWindow();
	void buildSceneManagerWindow();
	void buildEditorSettingsWindow();
	void buildLogWindow();
	void buildStatsMonitor();
	void buildImguiDemo();

	void saveActiveScene();

	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;
	bool m_shouldShowImguiDemo;
	bool m_shouldShowSceneCreator;
	ImguiEditorSidebarState m_sidebarState;
	ImguiEditorLog m_editorLog;
	ImguiEditorSceneCreator m_sceneCreator;
	ImguiEditorSceneManager m_sceneManager;
	ImguiEditorAssetBrowser m_assetBrowser;

	ImguiFileSystemDialog m_generalFileSystemDialog;

	ImguiEditorTheme m_theme;
};

inline const ImguiEditorTheme& ImguiEditorUI::getTheme()
{
	return m_theme;
}

}// end namespace ph::editor
