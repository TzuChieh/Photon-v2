#pragma once

#include "EditorCore/FileSystemExplorer.h"

#include "ThirdParty/DearImGui.h"

#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>

#include <cstddef>
#include <vector>
#include <string>
#include <string_view>

namespace ph::editor
{

class ImguiEditorUIProxy;

struct ImguiFileSystemDialogParameters
{
	bool canSelectItem = true;
	bool canSelectDirectory = false;

	/*! Show a notification if no item was selected. */
	bool requiresItemSelection = false;

	/*! Show a notification if no directory was selected. */
	bool requiresDirectorySelection = false;
};

class ImguiFileSystemDialog final
{
public:
	static const char* const OPEN_FILE_TITLE;
	static const char* const SAVE_FILE_TITLE;
	static const char* const OPEN_FOLDER_TITLE;
	static const char* const SAVE_FOLDER_TITLE;

public:
	ImguiFileSystemDialog();

	void openPopup(
		const char* popupName);

	void buildFileSystemDialogPopupModal(
		const char* popupName,
		ImguiEditorUIProxy editorUI,
		const ImguiFileSystemDialogParameters& params = {});

	void buildFileSystemDialogPopupModal(
		const char* popupName,
		ImguiEditorUIProxy editorUI,
		const ImVec2& dialogSize,
		const ImguiFileSystemDialogParameters& params = {});

	void clearSelection();
	bool selectionConfirmed();

	/*! @brief Get the selected directory.
	*/
	Path getSelectedDirectory() const;

	/*! @brief Get the selected item.
	*/
	Path getSelectedItem() const;

	/*! @brief Get the selected filesystem target.
	The result can be either the selected directory or a full path to the selected item. This method 
	effectively combines the result of getSelectedDirectory() and getSelectedItem().
	*/
	Path getSelectedTarget() const;

	/*! @brief Get all selected items.
	*/
	std::vector<Path> getSelectedItems() const;

	bool hasSelectedDirectory() const;
	bool hasSelectedItem() const;

private:
	void buildFileSystemDialogContent(
		const ImVec2& dialogSize,
		const ImguiFileSystemDialogParameters& params);

	void buildFileSystemDialogTreeNodeRecursive(
		FileSystemDirectoryEntry* baseEntry);

	/*! @brief Lightweight helper to get an undecorated item name from current entry.
	*/
	std::string_view getEntryItemNameWithoutDecorations(std::size_t itemIndex) const;

	FileSystemExplorer m_explorer;
	FileSystemDirectoryEntry* m_selectedEntry;
	bool m_selectionConfirmedFlag;

	// `std::vector` of chars since input text seems to cache buffer size internally (cannot alter 
	// input text buffer size without closing then repoening the dialog window)
	std::vector<char> m_fsDialogEntryPreviewBuffer;
	std::vector<char> m_fsDialogItemPreviewBuffer;
	bool m_isEditingEntry;
	bool m_isEditingItem;

	// Cached for display
	std::string m_fsDialogEntryPathName;
	std::vector<std::string> m_fsDialogEntryItemNames;

	std::vector<std::string> m_fsDialogRootNames;
	std::size_t m_fsDialogSelectedRootIdx;
	std::vector<Path> m_fsDialogEntryItems;
	std::size_t m_fsDialogSelectedEntryItemIdx;
	std::size_t m_fsDialogNumSelectedItems;
	std::vector<uint8> m_fsDialogEntryItemSelection;
};

}// end namespace ph::editor
