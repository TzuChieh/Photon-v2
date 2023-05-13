#pragma once

#include "ThirdParty/DearImGuiExperimental.h"
#include "EditorCore/FileSystemExplorer.h"

#include <DataIO/FileSystem/Path.h>

#include <vector>
#include <cstddef>
#include <string>

namespace ph::editor
{

class Editor;
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
	void buildFilesystemDialogContent(FileSystemExplorer& explorer);
	void buildFilesystemDialogTreeNodeRecursive(
		FileSystemDirectoryEntry* baseEntry,
		FileSystemExplorer& explorer);

	FileSystemExplorer m_fsDialogExplorer;
	FileSystemDirectoryEntry* m_fsDialogSelectedEntry;
	std::vector<std::string> m_fsDialogEntryItems;
	std::size_t m_fsDialogSelectedItemIdx;

private:
	static const char* const rootPropertiesWindowName;
	static const char* const mainViewportWindowName;
	static const char* const assetBrowserWindowName;
	static const char* const objectBrowserWindowName;
};

}// end namespace ph::editor
