#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"
#include "EditorCore/FileSystemExplorer.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cstdio>
#include <algorithm>

#define PH_IMGUI_OPEN_FILE_ICON    ICON_MD_FOLDER_OPEN
#define PH_IMGUI_SAVE_FILE_ICON    ICON_MD_SAVE
#define PH_IMGUI_GENERAL_FILE_ICON ICON_MD_DESCRIPTION

namespace ph::editor
{

const char* const ImguiFileSystemDialog::OPEN_FILE_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open File";
const char* const ImguiFileSystemDialog::SAVE_FILE_TITLE = PH_IMGUI_SAVE_FILE_ICON " Save File";

namespace
{

constexpr std::string_view FILE_ITEM_NAME_PREFIX = PH_IMGUI_GENERAL_FILE_ICON " ";

// Copy string to a vector buffer. The result is always null-terminated.
inline void copy_to_buffer(std::string_view srcStr, std::vector<char>& dstBuffer)
{
	PH_ASSERT(!dstBuffer.empty());

	// Can only copy what the buffer can hold
	const auto numCharsToCopy = std::min(srcStr.size(), dstBuffer.size());
	std::copy(srcStr.begin(), srcStr.begin() + numCharsToCopy, dstBuffer.begin());

	// Always make the result null-terminated
	if(numCharsToCopy < dstBuffer.size())
	{
		dstBuffer[numCharsToCopy] = '\0';
	}
	else
	{
		dstBuffer.back() = '\0';
	}
}

}// end anonymous namespace

ImguiFileSystemDialog::ImguiFileSystemDialog()
	: m_explorer()
	, m_selectedEntry(nullptr)
	, m_selectionConfirmedFlag(false)

	, m_fsDialogEntryPreviewBuffer()
	, m_fsDialogItemPreviewBuffer()
	, m_isEditingEntry(false)
	, m_isEditingItem(false)

	, m_fsDialogRootNames()
	, m_fsDialogSelectedRootIdx(static_cast<std::size_t>(-1))
	, m_fsDialogEntryItems()
	, m_fsDialogSelectedEntryItemIdx(static_cast<std::size_t>(-1))
	, m_fsDialogEntryItemSelection()
{
	m_fsDialogEntryPreviewBuffer.resize(512);
	m_fsDialogItemPreviewBuffer.resize(256);
}

void ImguiFileSystemDialog::openPopup(
	const char* const popupName)
{
	if(ImGui::IsPopupOpen(popupName))
	{
		PH_DEFAULT_LOG_DEBUG(
			"recursive file dialog popup call (popup name = {})",
			popupName);
		return;
	}

	clearSelection();

	m_fsDialogRootNames.clear();
	for(const Path& rootPath : m_explorer.getRootPaths())
	{
		m_fsDialogRootNames.push_back(rootPath.toString());
	}

	if(!m_fsDialogRootNames.empty())
	{
		m_fsDialogSelectedRootIdx = 0;
		m_explorer.setCurrentRootPath(m_fsDialogSelectedRootIdx);
	}

	ImGui::OpenPopup(popupName);
}

void ImguiFileSystemDialog::buildFileSystemDialogPopupModal(
	const char* const popupName,
	const ImVec2& dialogSize,
	const bool canSelectFile,
	const bool canSelectDirectory)
{
	// Potentially skip more code before `ImGui::BeginPopupModal()`
	if(!ImGui::IsPopupOpen(popupName))
	{
		return;
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if(ImGui::BeginPopupModal(popupName, NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		buildFileSystemDialogContent(dialogSize, canSelectFile, canSelectDirectory);

		ImGui::Separator();

		if(ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_selectionConfirmedFlag = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if(ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ImguiFileSystemDialog::clearSelection()
{
	//m_selectedEntry = nullptr;

	m_fsDialogSelectedRootIdx = static_cast<std::size_t>(-1);
	m_fsDialogSelectedEntryItemIdx = static_cast<std::size_t>(-1);
	std::fill(m_fsDialogEntryItemSelection.begin(), m_fsDialogEntryItemSelection.end(), 0);
}

bool ImguiFileSystemDialog::selectionConfirmed()
{
	if(m_selectionConfirmedFlag)
	{
		m_selectionConfirmedFlag = false;
		return true;
	}
		
	return false;
}

std::optional<Path> ImguiFileSystemDialog::getSelectedDirectory() const
{
	if(!m_selectedEntry)
	{
		return std::nullopt;
	}

	return m_selectedEntry->getDirectoryPath();
}

std::optional<Path> ImguiFileSystemDialog::getSelectedItem() const
{
	if(m_fsDialogSelectedEntryItemIdx == static_cast<std::size_t>(-1))
	{
		return std::nullopt;
	}

	PH_ASSERT_LT(m_fsDialogSelectedEntryItemIdx, m_fsDialogEntryItems.size());
	return m_fsDialogEntryItems[m_fsDialogSelectedEntryItemIdx];
}

std::vector<Path> ImguiFileSystemDialog::getSelectedItems() const
{
	PH_ASSERT_EQ(m_fsDialogEntryItemSelection.size(), m_fsDialogEntryItems.size());
	
	std::vector<Path> selectedItems;
	for(std::size_t itemIdx = 0; itemIdx < m_fsDialogEntryItemSelection.size(); ++itemIdx)
	{
		if(m_fsDialogEntryItemSelection[itemIdx] == 1)
		{
			selectedItems.push_back(m_fsDialogEntryItems[itemIdx]);
		}
	}
	return selectedItems;
}

void ImguiFileSystemDialog::buildFileSystemDialogContent(
	const ImVec2& dialogSize,
	const bool canSelectFile,
	const bool canSelectDirectory)
{
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
	if(ImGui::BeginCombo("##root_combo", m_fsDialogRootNames[m_fsDialogSelectedRootIdx].c_str()))
	{
		for(std::size_t rootIdx = 0; rootIdx < m_fsDialogRootNames.size(); ++rootIdx)
		{
			const bool isSelected = (rootIdx == m_fsDialogSelectedRootIdx);
			if(ImGui::Selectable(m_fsDialogRootNames[m_fsDialogSelectedRootIdx].c_str(), isSelected))
			{
				m_fsDialogSelectedRootIdx = rootIdx;
				m_explorer.setCurrentRootPath(m_fsDialogSelectedRootIdx);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;

	// Left child: file system directory tree view
	ImGui::BeginChild(
		"fs_dialog_tree", 
		ImVec2(dialogSize.x * 0.4f, dialogSize.y),
		true, 
		windowFlags);
	buildFileSystemDialogTreeNodeRecursive(m_explorer.getCurrentDirectoryEntry());
	ImGui::EndChild();

	ImGui::SameLine();

	// Right child: file system item view
	ImGui::BeginChild(
		"fs_dialog_selectable",
		ImVec2(dialogSize.x * 0.6f, dialogSize.y),
		true,
		windowFlags);

	for(std::size_t itemIdx = 0; itemIdx < m_fsDialogEntryItemNames.size(); ++itemIdx)
	{
		const std::string& itemName = m_fsDialogEntryItemNames[itemIdx];
		const bool isSelected = m_fsDialogEntryItemSelection[itemIdx] != 0;
		if(ImGui::Selectable(itemName.c_str(), isSelected))
		{
			// Clear selection when CTRL is not held
			if(!ImGui::GetIO().KeyCtrl)
			{
				std::fill(m_fsDialogEntryItemSelection.begin(), m_fsDialogEntryItemSelection.end(), 0);
			}

			m_fsDialogSelectedEntryItemIdx = itemIdx;
			m_fsDialogEntryItemSelection[itemIdx] = 1;
		}

		// We are selecting, not editing item input text, if the item selectable is clicked
		if(ImGui::IsItemClicked())
		{
			m_isEditingItem = false;
		}
	}
	ImGui::EndChild();

	ImGui::PopStyleVar();

	if(canSelectFile || canSelectDirectory)
	{
		if(!m_selectedEntry)
		{
			copy_to_buffer("(no directory selected)", m_fsDialogEntryPreviewBuffer);
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText(
			"##directory_preview",
			m_fsDialogEntryPreviewBuffer.data(),
			m_fsDialogEntryPreviewBuffer.size(),
			ImGuiInputTextFlags_ReadOnly);
	}

	// Only show the result of item select/edit if selecting file is allowed
	if(canSelectFile)
	{
		// Only set the input text content if not editing
		if(!m_isEditingItem)
		{
			int numSelectedItems = 0;
			for(auto value : m_fsDialogEntryItemSelection)
			{
				numSelectedItems += value != 0;
			}

			if(numSelectedItems == 0)
			{
				copy_to_buffer("(no file selected)", m_fsDialogItemPreviewBuffer);
			}
			else if(numSelectedItems == 1)
			{
				copy_to_buffer(
					getEntryItemNameWithoutDecorations(m_fsDialogSelectedEntryItemIdx), 
					m_fsDialogItemPreviewBuffer);
			}
			else
			{
				std::snprintf(
					m_fsDialogItemPreviewBuffer.data(),
					m_fsDialogItemPreviewBuffer.size(),
					"(%d files selected)", 
					numSelectedItems);
			}
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText(
			"##file_item_preview",
			m_fsDialogItemPreviewBuffer.data(),
			m_fsDialogItemPreviewBuffer.size());

		// We are editing if the item input text is clicked
		if(ImGui::IsItemClicked())
		{
			m_isEditingItem = true;
		}
	}
}

void ImguiFileSystemDialog::buildFileSystemDialogTreeNodeRecursive(
	FileSystemDirectoryEntry* const baseEntry)
{
	if(!baseEntry)
	{
		return;
	}

	const bool isRootEntry = baseEntry->getParent() == nullptr;

	ImGuiTreeNodeFlags nodeFlags = 0;
	if(baseEntry == m_selectedEntry)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	if(isRootEntry)
	{
		nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	
	const bool isNodeOpened = ImGui::TreeNodeEx(baseEntry->getDirectoryName().c_str(), nodeFlags);
	if(ImGui::IsItemClicked() || !m_selectedEntry)
	{
		m_selectedEntry = baseEntry;
		m_fsDialogEntryItems = m_explorer.makeItemListing(baseEntry, false);

		m_fsDialogEntryItemNames.clear();
		for(const Path& item : m_fsDialogEntryItems)
		{
			m_fsDialogEntryItemNames.push_back(std::string(FILE_ITEM_NAME_PREFIX) + item.toString());
		}

		m_fsDialogSelectedEntryItemIdx = static_cast<std::size_t>(-1);
		m_fsDialogEntryItemSelection.resize(m_fsDialogEntryItems.size());
		std::fill(m_fsDialogEntryItemSelection.begin(), m_fsDialogEntryItemSelection.end(), 0);
		copy_to_buffer(baseEntry->getDirectoryPath().toAbsoluteString(), m_fsDialogEntryPreviewBuffer);
	}

	if(isNodeOpened)
	{
		m_explorer.expand(baseEntry);
		for(std::size_t entryIdx = 0; entryIdx < baseEntry->numChildren(); ++entryIdx)
		{
			FileSystemDirectoryEntry* derivedEntry = baseEntry->getChild(entryIdx);
			buildFileSystemDialogTreeNodeRecursive(derivedEntry);
		}
		ImGui::TreePop();
	}
	else
	{
		m_explorer.collapse(baseEntry);
	}
}

std::string_view ImguiFileSystemDialog::getEntryItemNameWithoutDecorations(
	const std::size_t itemIndex) const
{
	PH_ASSERT_LT(itemIndex, m_fsDialogEntryItemNames.size());

	std::string_view itemName = m_fsDialogEntryItemNames[itemIndex];
	
	PH_ASSERT_LE(FILE_ITEM_NAME_PREFIX.size(), itemName.size());
	itemName.remove_prefix(FILE_ITEM_NAME_PREFIX.size());

	return itemName;
}

}// end namespace ph::editor
