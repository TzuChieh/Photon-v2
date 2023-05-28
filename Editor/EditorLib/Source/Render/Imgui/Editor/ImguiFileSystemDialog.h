#pragma once

#include "EditorCore/FileSystemExplorer.h"

#include "ThirdParty/DearImGui.h"

#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>

#include <cstddef>
#include <vector>
#include <string>
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

	FileSystemExplorer m_explorer;
	FileSystemDirectoryEntry* m_selectedEntry;
	bool m_selectionConfirmedFlag;

	std::vector<std::string> m_fsDialogRootNames;
	std::size_t m_fsDialogSelectedRootIdx;
	std::string m_fsDialogEntryPreview;
	std::vector<Path> m_fsDialogEntryItems;
	std::vector<std::string> m_fsDialogEntryItemNames;
	std::size_t m_fsDialogSelectedEntryItemIdx;
	std::vector<uint8> m_fsDialogEntryItemSelection;
};

}// end namespace ph::editor
