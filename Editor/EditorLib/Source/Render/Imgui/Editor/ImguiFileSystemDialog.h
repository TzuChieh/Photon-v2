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

	/*!
	Note that popup identifiers are relative to the current ID-stack (so for example, `openPopup()`
	and `buildFileSystemDialogPopupModal()` needs to be at the same level).
	*/
	void openPopup(
		const char* popupName);

	/*!
	This overload builds a popup modal with dialog size determined by editor's dimension hints.
	See other overloads for more detail.
	*/
	void buildFileSystemDialogPopupModal(
		const char* popupName,
		ImguiEditorUIProxy editorUI,
		const ImguiFileSystemDialogParameters& params = {});

	/*!
	Treat this call like `ImGui::BeginPopupModal()`. `ImGui::BeginPopupModal()` will establish
	a new ID relative to the current window (like many other built-in widgets), so different
	popups with the same name will not conflict with each other (as long as they are not within
	the same window).
	*/
	void buildFileSystemDialogPopupModal(
		const char* popupName,
		ImguiEditorUIProxy editorUI,
		const ImVec2& dialogSize,
		const ImguiFileSystemDialogParameters& params = {});

	void clearSelection();

	/*!
	@return True if the opened dialog is closed. Will not return true on subsequent calls unless 
	the dialog is opened and closed again.
	*/
	bool dialogClosed();

	/*! @brief Get the selected directory.
	Call after `dialogClosed()` for a complete result. If the dialog is not closed, intermediate result
	will be returned.
	@return Selected directory. May be none (by checking `Path::isEmpty()`).
	*/
	Path getSelectedDirectory() const;

	/*! @brief Get the selected item.
	Call after `dialogClosed()` for a complete result. If the dialog is not closed, intermediate result
	will be returned.
	@return Selected item. May be none (by checking `Path::isEmpty()`).
	*/
	Path getSelectedItem() const;

	/*! @brief Get the selected filesystem target.
	The result can be either the selected directory or a full path to the selected item. This method
	effectively combines the result of getSelectedDirectory() and getSelectedItem().
	Call after `dialogClosed()` for a complete result. If the dialog is not closed, intermediate result
	will be returned.
	@return Selected target. May be none (by checking `Path::isEmpty()`).
	*/
	Path getSelectedTarget() const;

	/*! @brief Get all selected items.
	Call after `dialogClosed()` for a complete result. If the dialog is not closed, intermediate result
	will be returned.
	*/
	std::vector<Path> getSelectedItems() const;

	/*! @brief Fast way to check whether a directory has been selected.
	Call after `dialogClosed()` for a complete result. If the dialog is not closed, intermediate result
	will be returned.
	*/
	bool hasSelectedDirectory() const;

	/*! @brief Fast way to check whether an item has been selected.
	Call after `dialogClosed()` for a complete result. If the dialog is not closed, intermediate result
	will be returned.
	*/
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
	bool m_dialogClosedFlag;

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
