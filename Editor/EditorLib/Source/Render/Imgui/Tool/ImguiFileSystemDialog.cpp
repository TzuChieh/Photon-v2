#include "Render/Imgui/Tool/ImguiFileSystemDialog.h"
#include "Render/Imgui/ImguiEditorUIProxy.h"
#include "EditorCore/FileSystemExplorer.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "App/Editor.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cstdio>
#include <algorithm>

namespace ph::editor
{

const char* const ImguiFileSystemDialog::OPEN_FILE_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open File";
const char* const ImguiFileSystemDialog::SAVE_FILE_TITLE = PH_IMGUI_SAVE_FILE_ICON " Save File";
const char* const ImguiFileSystemDialog::OPEN_FOLDER_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open Folder";
const char* const ImguiFileSystemDialog::SAVE_FOLDER_TITLE = PH_IMGUI_SAVE_FILE_ICON " Save Folder";

namespace
{

constexpr const char* REQUIRES_SELECTION_TITLE = "Target Specification Required";

constexpr std::string_view FILE_ITEM_NAME_PREFIX = PH_IMGUI_GENERAL_FILE_ICON " ";

}// end anonymous namespace

ImguiFileSystemDialog::ImguiFileSystemDialog()
	: m_explorer()
	, m_browsingEntry(nullptr)
	, m_selectedEntry(nullptr)
	, m_dialogClosedFlag(false)

	, m_fsDialogEntryPreviewBuffer(512)
	, m_fsDialogItemPreviewBuffer(256)
	, m_isEditingEntry(false)
	, m_isEditingItem(false)

	, m_fsDialogEntryPathName()
	, m_fsDialogEntryItemNames()

	, m_fsDialogRootNames()
	, m_fsDialogSelectedRootIdx(static_cast<std::size_t>(-1))
	, m_fsDialogEntryItems()
	, m_fsDialogSelectedEntryItemIdx(static_cast<std::size_t>(-1))
	, m_fsDialogNumSelectedItems(0)
	, m_fsDialogEntryItemSelection()
{}

void ImguiFileSystemDialog::openPopup(
	const char* const popupName)
{
	if(ImGui::IsPopupOpen(popupName))
	{
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
	ImguiEditorUIProxy editorUI,
	const ImguiFileSystemDialogParameters& params)
{
	buildFileSystemDialogPopupModal(
		popupName, 
		editorUI, 
		ImVec2(
			editorUI.getEditor().dimensionHints.fileDialogPreferredWidth,
			editorUI.getEditor().dimensionHints.fileDialogPreferredHeight),
		params);
}

void ImguiFileSystemDialog::buildFileSystemDialogPopupModal(
	const char* const popupName,
	ImguiEditorUIProxy editorUI,
	const ImVec2& dialogSize,
	const ImguiFileSystemDialogParameters& params)
{
	// Potentially skip more code before `ImGui::BeginPopupModal()`
	if(!ImGui::IsPopupOpen(popupName))
	{
		return;
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if(ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		buildFileSystemDialogContent(dialogSize, params);

		ImGui::Separator();

		if(ImGui::Button("OK", ImVec2(120, 0)))
		{
			if((params.requiresItemSelection && !hasSelectedItem()) || 
			   (params.requiresDirectorySelection && !hasSelectedDirectory()))
			{
				m_dialogClosedFlag = false;
				ImGui::OpenPopup(REQUIRES_SELECTION_TITLE);
			}
			else
			{
				m_dialogClosedFlag = true;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if(ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_dialogClosedFlag = true;

			// We are canceling--no selection is expected
			clearSelection();

			ImGui::CloseCurrentPopup();
		}

		if(ImGui::BeginPopupModal(REQUIRES_SELECTION_TITLE, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PushFont(editorUI.getFontLibrary().largeFont);
			ImGui::Text(PH_IMGUI_NOTIFICATION_ICON " ");
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::Text("Please specify a target for action (such as a directory or file).");
			if(ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::EndPopup();
	}
}

void ImguiFileSystemDialog::clearSelection()
{
	m_selectedEntry = nullptr;

	m_fsDialogSelectedRootIdx = static_cast<std::size_t>(-1);
	m_fsDialogSelectedEntryItemIdx = static_cast<std::size_t>(-1);
	std::fill(m_fsDialogEntryItemSelection.begin(), m_fsDialogEntryItemSelection.end(), 0);
	m_fsDialogNumSelectedItems = 0;

	m_isEditingEntry = false;
	m_isEditingItem = false;
	m_fsDialogEntryPreviewBuffer.clear();
	m_fsDialogItemPreviewBuffer.clear();
}

bool ImguiFileSystemDialog::dialogClosed()
{
	if(m_dialogClosedFlag)
	{
		m_dialogClosedFlag = false;
		return true;
	}
		
	return false;
}

Path ImguiFileSystemDialog::getSelectedDirectory() const
{
	// If is editing, the preview buffer contains an edited path
	if(m_isEditingEntry)
	{
		if(!m_fsDialogEntryPreviewBuffer.isEmpty())
		{
			return Path(m_fsDialogEntryPreviewBuffer.getContent());
		}
		else
		{
			return Path{};
		}
	}

	if(m_selectedEntry)
	{
		return m_selectedEntry->getDirectoryPath();
	}

	return Path{};
}

Path ImguiFileSystemDialog::getSelectedItem() const
{
	// If is editing, the preview buffer contains an edited item
	if(m_isEditingItem)
	{
		if(!m_fsDialogItemPreviewBuffer.isEmpty())
		{
			return Path(m_fsDialogItemPreviewBuffer.getContent());
		}
		else
		{
			return Path{};
		}
	}

	if(m_fsDialogSelectedEntryItemIdx != static_cast<std::size_t>(-1))
	{
		PH_ASSERT_LT(m_fsDialogSelectedEntryItemIdx, m_fsDialogEntryItems.size());
		return m_fsDialogEntryItems[m_fsDialogSelectedEntryItemIdx];
	}

	return Path{};
}

Path ImguiFileSystemDialog::getSelectedTarget() const
{
	Path selectedDirectory = getSelectedDirectory();

	// Cannot identify a target if directory is none
	if(selectedDirectory.isEmpty())
	{
		return Path{};
	}

	return selectedDirectory / getSelectedItem();
}

std::vector<Path> ImguiFileSystemDialog::getSelectedItems() const
{
	if(m_fsDialogNumSelectedItems <= 1)
	{
		auto item = getSelectedItem();
		if(item.isEmpty())
		{
			return {};
		}
		else
		{
			return {item};
		}
	}

	// Currently does not support edited multiple items

	PH_ASSERT_EQ(m_fsDialogEntryItemSelection.size(), m_fsDialogEntryItems.size());
	
	std::vector<Path> selectedItems;
	for(std::size_t itemIdx = 0; itemIdx < m_fsDialogEntryItemSelection.size(); ++itemIdx)
	{
		if(m_fsDialogEntryItemSelection[itemIdx] == 1)
		{
			selectedItems.push_back(m_fsDialogEntryItems[itemIdx]);
		}
	}

	PH_ASSERT_EQ(selectedItems.size(), m_fsDialogNumSelectedItems);

	return selectedItems;
}

bool ImguiFileSystemDialog::hasSelectedDirectory() const
{
	// If is editing, the preview buffer contains an edited path
	if(m_isEditingEntry)
	{
		return !m_fsDialogEntryPreviewBuffer.isEmpty();
	}

	return m_selectedEntry;
}

bool ImguiFileSystemDialog::hasSelectedItem() const
{
	// If is editing, the preview buffer contains an edited item
	if(m_isEditingItem)
	{
		return !m_fsDialogItemPreviewBuffer.isEmpty();
	}

	return m_fsDialogSelectedEntryItemIdx != static_cast<std::size_t>(-1);
}

void ImguiFileSystemDialog::buildFileSystemDialogContent(
	const ImVec2& dialogSize,
	const ImguiFileSystemDialogParameters& params)
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

	// Disable item view if item selection is not allowed. Not hiding the item view since it can help
	// if the user can still see what is inside a directory.
	if(!params.canSelectItem)
	{
		ImGui::BeginDisabled();
	}

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
				m_fsDialogNumSelectedItems = 0;
			}

			m_fsDialogSelectedEntryItemIdx = itemIdx;
			m_fsDialogEntryItemSelection[itemIdx] = 1;
			++m_fsDialogNumSelectedItems;
		}

		if(ImGui::IsItemClicked())
		{
			// We are selecting, not editing item input text, if the item selectable is clicked
			m_isEditingItem = false;

			// Clicking on item implicitly selects an entry, too
			PH_ASSERT(m_browsingEntry);
			m_selectedEntry = m_browsingEntry;
		}
	}

	if(!params.canSelectItem)
	{
		ImGui::EndDisabled();
	}

	ImGui::EndChild();

	ImGui::PopStyleVar();

	if(params.canSelectItem || params.canSelectDirectory)
	{
		// Only set the input text content if not editing
		if(!m_isEditingEntry)
		{
			if(m_selectedEntry)
			{
				m_fsDialogEntryPreviewBuffer.resizableCopy(m_fsDialogEntryPathName);
			}
			else
			{
				m_fsDialogEntryPreviewBuffer.resizableCopy("(no directory selected)");
			}
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		m_fsDialogEntryPreviewBuffer.inputText("##entry_preview");

		// We are editing if the entry text is clicked
		if(ImGui::IsItemClicked())
		{
			m_isEditingEntry = true;
		}
	}

	// Only show the result of item select/edit if selecting item is allowed
	if(params.canSelectItem)
	{
		// Only set the input text content if not editing
		if(!m_isEditingItem)
		{
			if(m_fsDialogNumSelectedItems == 0)
			{
				m_fsDialogItemPreviewBuffer.resizableCopy("(no item selected)");
			}
			else if(m_fsDialogNumSelectedItems == 1)
			{
				m_fsDialogItemPreviewBuffer.resizableCopy(getEntryItemNameWithoutDecorations(m_fsDialogSelectedEntryItemIdx));
			}
			else
			{
				std::snprintf(
					m_fsDialogItemPreviewBuffer.getData().data(),
					m_fsDialogItemPreviewBuffer.getData().size(),
					"(%d items selected)", 
					static_cast<int>(m_fsDialogNumSelectedItems));
			}
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		m_fsDialogItemPreviewBuffer.inputText("##item_preview");

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

	// User may want to select an entry only. Opening/closing node on single click has bad UX as 
	// the node listing is changing inconsistently and is irrelevant to selecting an entry. We also
	// make the node hit box span available width to improve UX (easier to select for short names).
	ImGuiTreeNodeFlags nodeFlags = 
		ImGuiTreeNodeFlags_OpenOnDoubleClick | 
		ImGuiTreeNodeFlags_OpenOnArrow | 
		ImGuiTreeNodeFlags_SpanAvailWidth;

	if(baseEntry == m_browsingEntry)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	if(isRootEntry)
	{
		nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	
	const bool isNodeOpened = ImGui::TreeNodeEx(baseEntry->getDirectoryName().c_str(), nodeFlags);
	const bool isNodeClicked = ImGui::IsItemClicked();

	// Initialize entry related data when an entry is clicked or if nothing was being browsed
	if(isNodeClicked || !m_browsingEntry)
	{
		m_browsingEntry = baseEntry;
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
		m_fsDialogEntryPathName = baseEntry->getDirectoryPath().toAbsoluteString();
	}

	// We are selecting, not editing entry, if the entry node is clicked
	if(isNodeClicked)
	{
		m_isEditingEntry = false;
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
	if(itemIndex == static_cast<std::size_t>(-1))
	{
		return "";
	}

	PH_ASSERT_LT(itemIndex, m_fsDialogEntryItemNames.size());
	std::string_view itemName = m_fsDialogEntryItemNames[itemIndex];
	
	PH_ASSERT_LE(FILE_ITEM_NAME_PREFIX.size(), itemName.size());
	itemName.remove_prefix(FILE_ITEM_NAME_PREFIX.size());

	return itemName;
}

}// end namespace ph::editor
