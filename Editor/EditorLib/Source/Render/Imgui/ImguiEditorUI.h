#pragma once

#include "ThirdParty/DearImGuiExperimental.h"
#include "EditorCore/FileSystemExplorer.h"
#include "Render/Imgui/Editor/ImguiEditorSidebarState.h"

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

private:
	Editor& getEditor();
	ImguiFontLibrary& getFontLibrary();
	ImguiImageLibrary& getImageLibrary();
	DimensionHints& getDimensionHints();

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
	void buildStatsMonitor();
	void buildImguiDemo();

	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;
	bool m_shouldShowImguiDemo;
	ImguiEditorSidebarState m_sidebarState;

private:
	void buildFileSystemDialogPopupModal(
		const char* popupName, 
		FileSystemExplorer& explorer,
		bool canSelectFile = true,
		bool canSelectDirectory = false);

	void buildFileSystemDialogContent(
		FileSystemExplorer& explorer,
		bool canSelectFile,
		bool canSelectDirectory);

	void buildFileSystemDialogTreeNodeRecursive(
		FileSystemDirectoryEntry* baseEntry,
		FileSystemExplorer& explorer);

	FileSystemExplorer m_fsDialogExplorer;
	std::vector<std::string> m_fsDialogRootNames;
	std::size_t m_fsDialogSelectedRootIdx;
	FileSystemDirectoryEntry* m_fsDialogSelectedEntry;
	std::string m_fsDialogEntryPreview;
	std::vector<Path> m_fsDialogEntryItems;
	std::vector<std::string> m_fsDialogEntryItemNames;
	std::size_t m_fsDialogSelectedEntryItemIdx;
	std::vector<uint8> m_fsDialogEntryItemSelection;
};

}// end namespace ph::editor
