#pragma once

#include "EditorCore/FileSystemExplorer.h"

#include "ThirdParty/DearImGui.h"

#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>

#include <cstddef>
#include <vector>
#include <string>
#include <string_view>
#include <optional>

namespace ph::editor
{

class ImguiFileSystemDialog final
{
public:
	static const char* const OPEN_FILE_TITLE;
	static const char* const SAVE_FILE_TITLE;

public:
	ImguiFileSystemDialog();

	void openPopup(
		const char* popupName);

	void buildFileSystemDialogPopupModal(
		const char* popupName,
		const ImVec2& dialogSize = ImVec2(0, 0),
		bool canSelectFile = true,
		bool canSelectDirectory = false);

	void clearSelection();
	bool selectionConfirmed();

	std::optional<Path> getSelectedDirectory() const;
	std::optional<Path> getSelectedItem() const;
	std::vector<Path> getSelectedItems() const;

private:
	void buildFileSystemDialogContent(
		const ImVec2& dialogSize,
		bool canSelectFile,
		bool canSelectDirectory);

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
	std::vector<uint8> m_fsDialogEntryItemSelection;
};

}// end namespace ph::editor
