#pragma once

#include "ThirdParty/DearImGuiExperimental.h"
#include "EditorCore/FileSystemExplorer.h"

#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>

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
{
public:
	ImguiEditorUI();

	void initialize(Editor* editor, ImguiFontLibrary* fontLibrary, ImguiImageLibrary* imageLibrary);
	void build();

private:
	Editor& getEditor();
	DimensionHints& getDimensionHints();

	Editor* m_editor;
	ImguiFontLibrary* m_fontLibrary;
	ImguiImageLibrary* m_imageLibrary;

private:
	void buildMainMenuBar();
	void buildAssetBrowserWindow();
	void buildRootPropertiesWindow();
	void buildObjectBrowserWindow();
	void buildMainViewportWindow();
	void buildStatsMonitor();
	void buildImguiDemo();

	ImGuiID m_rootDockSpaceID;
	ImGuiID m_leftDockSpaceID;
	ImGuiID m_rightDockSpaceID;
	ImGuiID m_bottomDockSpaceID;
	ImGuiID m_centerDockSpaceID;
	bool m_shouldResetWindowLayout;
	bool m_shouldShowStatsMonitor;
	bool m_shouldShowImguiDemo;

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

private:
	static const char* const rootPropertiesWindowName;
	static const char* const mainViewportWindowName;
	static const char* const assetBrowserWindowName;
	static const char* const objectBrowserWindowName;
};

}// end namespace ph::editor
