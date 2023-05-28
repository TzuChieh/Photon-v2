#include "Render/Imgui/Editor/ImguiFileSystemDialog.h"
#include "EditorCore/FileSystemExplorer.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#define PH_IMGUI_OPEN_FILE_ICON    ICON_MD_FOLDER_OPEN
#define PH_IMGUI_SAVE_FILE_ICON    ICON_MD_SAVE
#define PH_IMGUI_GENERAL_FILE_ICON ICON_MD_DESCRIPTION

namespace ph::editor
{

const char* const ImguiFileSystemDialog::OPEN_FILE_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open File";
const char* const ImguiFileSystemDialog::SAVE_FILE_TITLE = PH_IMGUI_SAVE_FILE_ICON " Save File";

ImguiFileSystemDialog::ImguiFileSystemDialog()
	: m_explorer()
	, m_selectedEntry(nullptr)
	, m_selectionConfirmedFlag(false)

	, m_fsDialogRootNames()
	, m_fsDialogSelectedRootIdx(static_cast<std::size_t>(-1))
	, m_fsDialogEntryPreview()
	, m_fsDialogEntryItems()
	, m_fsDialogSelectedEntryItemIdx(static_cast<std::size_t>(-1))
	, m_fsDialogEntryItemSelection()
{}

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
	}
	ImGui::EndChild();

	ImGui::PopStyleVar();

	if(canSelectFile || canSelectDirectory)
	{
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if(m_selectedEntry)
		{
			ImGui::InputText(
				"##directory_preview",
				m_fsDialogEntryPreview.data(),
				m_fsDialogEntryPreview.size(),
				ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			static std::string noSelectionMsg = "(no directory selected)";

			ImGui::InputText(
				"##directory_preview",
				noSelectionMsg.data(),
				noSelectionMsg.size(),
				ImGuiInputTextFlags_ReadOnly);
		}
	}

	if(canSelectFile)
	{
		int numSelectedItems = 0;
		for(auto value : m_fsDialogEntryItemSelection)
		{
			numSelectedItems += value != 0;
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if(numSelectedItems == 0)
		{
			static std::string noSelectionMsg = "(no file selected)";

			ImGui::InputText(
				"##file_preview",
				noSelectionMsg.data(),
				noSelectionMsg.size(),
				ImGuiInputTextFlags_ReadOnly);
		}
		else if(numSelectedItems == 1)
		{
			PH_ASSERT_LT(m_fsDialogSelectedEntryItemIdx, m_fsDialogEntryItemNames.size());

			ImGui::InputText(
				"##file_preview",
				m_fsDialogEntryItemNames[m_fsDialogSelectedEntryItemIdx].data(),
				m_fsDialogEntryItemNames[m_fsDialogSelectedEntryItemIdx].size(),
				ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			std::array<char, 32> buf;
			std::snprintf(buf.data(), buf.size(), "(%d files selected)", numSelectedItems);
			ImGui::InputText(
				"##file_preview",
				buf.data(),
				buf.size(),
				ImGuiInputTextFlags_ReadOnly);
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
			m_fsDialogEntryItemNames.push_back(PH_IMGUI_GENERAL_FILE_ICON " " + item.toString());
		}

		m_fsDialogSelectedEntryItemIdx = static_cast<std::size_t>(-1);
		m_fsDialogEntryItemSelection.resize(m_fsDialogEntryItems.size());
		std::fill(m_fsDialogEntryItemSelection.begin(), m_fsDialogEntryItemSelection.end(), 0);
		m_fsDialogEntryPreview = baseEntry->getDirectoryPath().toAbsoluteString();
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

}// end namespace ph::editor
