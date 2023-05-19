#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <string_view>
#include <cstdio>
#include <utility>

#define PH_IMGUI_PROPERTIES_ICON   ICON_MD_TUNE
#define PH_IMGUI_VIEWPORT_ICON     ICON_MD_CAMERA
#define PH_IMGUI_ASSET_ICON        ICON_MD_FOLDER_OPEN
#define PH_IMGUI_OBJECT_ICON       ICON_MD_CATEGORY
#define PH_IMGUI_SCENE_ICON        ICON_MD_LANDSCAPE
#define PH_IMGUI_LOG_ICON          ICON_MD_WYSIWYG
#define PH_IMGUI_SETTINGS_ICON     ICON_MD_SETTINGS
#define PH_IMGUI_STATS_ICON        ICON_MD_INSIGHTS
#define PH_IMGUI_GENERAL_FILE_ICON ICON_MD_DESCRIPTION
#define PH_IMGUI_OPEN_FILE_ICON    ICON_MD_FOLDER_OPEN
#define PH_IMGUI_SAVE_FILE_ICON    ICON_MD_SAVE

namespace ph::editor
{

namespace
{

constexpr const char* ROOT_PROPERTIES_WINDOW_NAME = PH_IMGUI_PROPERTIES_ICON " Properties";
constexpr const char* MAIN_VIEWPORT_WINDOW_NAME = PH_IMGUI_VIEWPORT_ICON " Viewport";
constexpr const char* ASSET_BROWSER_WINDOW_NAME = PH_IMGUI_ASSET_ICON " Asset Browser";
constexpr const char* OBJECT_BROWSER_WINDOW_NAME = PH_IMGUI_OBJECT_ICON " Object Browser";
constexpr const char* SIDEBAR_WINDOW_NAME = "##sidebar_window";

constexpr const char* SCENE_BROWSER_WINDOW_NAME = PH_IMGUI_SCENE_ICON " Scene Manager";

constexpr const char* OPEN_FILE_DIALOG_POPUP_NAME = PH_IMGUI_OPEN_FILE_ICON " Open File";
constexpr const char* SAVE_FILE_DIALOG_POPUP_NAME = PH_IMGUI_SAVE_FILE_ICON " Save File";

}// end anonymous namespace

const ImguiEditorUI* ImguiEditorUI::mainEditor = nullptr;

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
	, m_fontLibrary(nullptr)
	, m_imageLibrary(nullptr)

	, m_shouldResetWindowLayout(false)
	, m_shouldShowStatsMonitor(false)
	, m_shouldShowImguiDemo(false)
	, m_sidebarState()

	, m_fsDialogExplorer()
	, m_fsDialogRootNames()
	, m_fsDialogSelectedRootIdx(static_cast<std::size_t>(-1))
	, m_fsDialogSelectedEntry(nullptr)
	, m_fsDialogEntryPreview()
	, m_fsDialogEntryItems()
	, m_fsDialogSelectedEntryItemIdx(static_cast<std::size_t>(-1))
	, m_fsDialogEntryItemSelection()
{
	if(!mainEditor)
	{
		mainEditor = this;
	}

	for(const Path& rootPath : m_fsDialogExplorer.getRootPaths())
	{
		m_fsDialogRootNames.push_back(rootPath.toString());
	}

	if(!m_fsDialogRootNames.empty())
	{
		m_fsDialogSelectedRootIdx = 0;
		m_fsDialogExplorer.setCurrentRootPath(m_fsDialogSelectedRootIdx);
	}
}

ImguiEditorUI::~ImguiEditorUI()
{
	if(mainEditor == this)
	{
		mainEditor = nullptr;
	}
}

void ImguiEditorUI::initialize(
	Editor* const editor, 
	ImguiFontLibrary* const fontLibrary, 
	ImguiImageLibrary* const imageLibrary)
{
	PH_ASSERT(editor);
	PH_ASSERT(fontLibrary);
	PH_ASSERT(imageLibrary);

	m_editor = editor;
	m_fontLibrary = fontLibrary;
	m_imageLibrary = imageLibrary;
}

void ImguiEditorUI::build()
{
	PH_ASSERT(Threads::isOnMainThread());
	PH_ASSERT(m_editor);


	buildMainMenuBar();

	// Experimental Docking API
	// References:
	// [1] https://github.com/ocornut/imgui/issues/2109
	// [2] `ShowExampleAppDockSpace()` from `imgui_demo.cpp`

	ImGuiViewport* const viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags rootWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	rootWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	rootWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	
	if(dockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		rootWindowFlags |= ImGuiWindowFlags_NoBackground;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Hidden DockSpace Root Window", nullptr, rootWindowFlags);

	ImGui::PopStyleVar(3);

	// Dock builder nodes are retained--we create them if they do not exist or a reset is requested
	const ImGuiID rootDockSpaceID = ImGui::GetID("RootDockSpace");
	if(!ImGui::DockBuilderGetNode(rootDockSpaceID) || m_shouldResetWindowLayout)
	{
		// Potentially clear out existing layout
		ImGui::DockBuilderRemoveNode(rootDockSpaceID);

		// Add an empty node with the size of viewport (minus menu bar)
		ImGui::DockBuilderAddNode(rootDockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(rootDockSpaceID, viewport->WorkSize);

		// Note that `ImGui::DockBuilderSplitNode()` is like using the splitting icon in the 
		// docking UI, while using the dock space ID without splitting is like using the central 
		// square icon in the docking UI.

		// Creating bottom node
		const float bottomNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth * 0.5f /
			viewport->WorkSize.y;
		ImGuiID centerDockSpaceID;
		const ImGuiID bottomDockSpaceID = ImGui::DockBuilderSplitNode(
			rootDockSpaceID, ImGuiDir_Down, bottomNodeSplitRatio, nullptr, &centerDockSpaceID);

		// Creating left node (after bottom node so it can have the full height)
		const float leftNodeSplitRatio =
			m_editor->dimensionHints.largeFontSize * 1.5f /
			viewport->WorkSize.x;
		const ImGuiID leftDockSpaceID = ImGui::DockBuilderSplitNode(
			rootDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, nullptr);

		// Creating right node (after bottom node so it can have the full height)
		const float rightNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.x;
		const ImGuiID rightDockSpaceID = ImGui::DockBuilderSplitNode(
			rootDockSpaceID, ImGuiDir_Right, rightNodeSplitRatio, nullptr, nullptr);

		// Creating child upper-right and upper-left nodes
		const float upperRightNodeSplitRatio = 0.4f;
		ImGuiID upperRightDockSpaceID, lowerRightDockSpaceID;
		ImGui::DockBuilderSplitNode(
			rightDockSpaceID, ImGuiDir_Up, upperRightNodeSplitRatio, &upperRightDockSpaceID, &lowerRightDockSpaceID);

		// Pre-dock some persistent windows
		ImGui::DockBuilderDockWindow(ASSET_BROWSER_WINDOW_NAME, bottomDockSpaceID);
		ImGui::DockBuilderDockWindow(ROOT_PROPERTIES_WINDOW_NAME, lowerRightDockSpaceID);
		ImGui::DockBuilderDockWindow(OBJECT_BROWSER_WINDOW_NAME, upperRightDockSpaceID);
		ImGui::DockBuilderDockWindow(MAIN_VIEWPORT_WINDOW_NAME, centerDockSpaceID);
		ImGui::DockBuilderDockWindow(SIDEBAR_WINDOW_NAME, leftDockSpaceID);

		// Pre-dock other windows
		ImGui::DockBuilderDockWindow(SCENE_BROWSER_WINDOW_NAME, upperRightDockSpaceID);

		ImGui::DockBuilderFinish(rootDockSpaceID);
	}

	PH_ASSERT_NE(rootDockSpaceID, 0);

	// Submit the DockSpace
	ImGui::DockSpace(rootDockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);

	ImGui::End();

	/*
	static bool hasInit = false;
	if(!hasInit)*/
	//{
	//	const ImGuiID rootDockSpaceID = ImGui::DockSpaceOverViewport(
	//		ImGui::GetMainViewport(),
	//		ImGuiDockNodeFlags_PassthruCentralNode);

	//	ImGui::DockBuilderSetNodeSize(rootDockSpaceID, ImGui::GetMainViewport()->Size);

	//	const float leftNodeSplitRatio =
	//		m_editor->dimensionHints.propertyPanelPreferredWidth /
	//		ImGui::GetMainViewport()->Size.x;
	//	const ImGuiID rootLeftDockSpaceID = ImGui::DockBuilderSplitNode(
	//		rootDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, nullptr);

	//	ImGui::DockBuilderDockWindow("Window A", rootLeftDockSpaceID);

	//	ImGui::DockBuilderFinish(rootDockSpaceID);

	//	//hasInit = true;
	//}

	buildAssetBrowserWindow();
	buildRootPropertiesWindow();
	buildObjectBrowserWindow();
	buildMainViewportWindow();
	buildSceneManagerWindow();
	buildSidebarWindow();

	buildEditorSettingsWindow();

	/*ImGui::SetNextWindowDockID(m_centerDockSpaceID, ImGuiCond_FirstUseEver);
	ImGui::Begin("whatever###TTT");
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();*/

	ImGui::Begin("Window B");
	ImGui::Text("This is window B");
	ImGui::End();


	//ImGui::Image(*m_imageLibrary->get(EImguiImage::Image), {200, 200});
	//m_imageLibrary->imguiImage(EImguiImage::Image, {200, 200});
	static int yyy = 0;
	if(m_imageLibrary->imguiImageButton(EImguiImage::Warning, "", {128, 128}))
	{
		ImGui::Text("asdasdasdasdsadsadas");

		PH_DEFAULT_LOG("pressed {}", ++yyy);
	}

	buildStatsMonitor();
	buildImguiDemo();

	// DEBUG
	if(ImGui::Button("fs"))
	{
		ImGui::OpenPopup(OPEN_FILE_DIALOG_POPUP_NAME);
	}
	buildFileSystemDialogPopupModal(OPEN_FILE_DIALOG_POPUP_NAME, m_fsDialogExplorer);
	//buildFileSystemDialogContent(m_fsDialogExplorer);
}

void ImguiEditorUI::buildMainMenuBar()
{
	m_shouldResetWindowLayout = false;

	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Edit"))
		{
			if(ImGui::MenuItem("Undo", "CTRL+Z"))
			{}

			// Disabled item
			if(ImGui::MenuItem("Redo", "CTRL+Y", false, false))
			{}  

			ImGui::Separator();

			if(ImGui::MenuItem("Cut", "CTRL+X"))
			{}

			if(ImGui::MenuItem("Copy", "CTRL+C"))
			{}

			if(ImGui::MenuItem("Paste", "CTRL+V"))
			{}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Window"))
		{
			if(ImGui::MenuItem("Reset Window Layout"))
			{
				m_shouldResetWindowLayout = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ImguiEditorUI::buildAssetBrowserWindow()
{
	ImGui::Begin(ASSET_BROWSER_WINDOW_NAME);


	ImGui::End();
}

void ImguiEditorUI::buildRootPropertiesWindow()
{
	ImGui::Begin(ROOT_PROPERTIES_WINDOW_NAME);

	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();
}

void ImguiEditorUI::buildObjectBrowserWindow()
{
	ImGui::Begin(OBJECT_BROWSER_WINDOW_NAME);

	if(ImGui::TreeNode("Basic"))
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if(ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if(ImGui::BeginTabItem("Avocado"))
			{
				ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Broccoli"))
			{
				ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Cucumber"))
			{
				ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::Separator();
		ImGui::TreePop();
	}

	ImGui::End();
}

void ImguiEditorUI::buildMainViewportWindow()
{
	ImGui::Begin(MAIN_VIEWPORT_WINDOW_NAME);

	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();
}

void ImguiEditorUI::buildSidebarWindow()
{
	ImGuiWindowClass windowClass;
	windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowClass(&windowClass);
	ImGui::Begin(SIDEBAR_WINDOW_NAME, nullptr, windowFlags);

	ImFont* const originalFont = ImGui::GetFont();

	ImGui::PushFont(getFontLibrary().largeFont);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));

	ImGuiStyle& style = ImGui::GetStyle();
	const float iconButtonSize = getDimensionHints().largeFontSize + style.FramePadding.x * 2.0f;
	const float posToCenter = (ImGui::GetWindowContentRegionMax().x - iconButtonSize) * 0.5f;

	ImGui::Spacing();
	ImGui::SetCursorPosX(posToCenter);
	if(ImGui::Button(PH_IMGUI_SCENE_ICON))
	{
		m_sidebarState.showSceneManager = !m_sidebarState.showSceneManager;
	}

	ImGui::PushFont(originalFont);
	if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("Scene Manager");
	}
	ImGui::PopFont();

	ImGui::Spacing();
	ImGui::SetCursorPosX(posToCenter);
	if(ImGui::Button(PH_IMGUI_LOG_ICON))
	{
		m_sidebarState.showLog = !m_sidebarState.showLog;
	}

	ImGui::PushFont(originalFont);
	if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("Log");
	}
	ImGui::PopFont();

	ImGui::Spacing();
	ImGui::SetCursorPosX(posToCenter);
	if(ImGui::Button(PH_IMGUI_SETTINGS_ICON))
	{
		m_sidebarState.showEditorSettings = !m_sidebarState.showEditorSettings;
	}

	ImGui::PushFont(originalFont);
	if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("Editor Settings");
	}
	ImGui::PopFont();

	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::End();
}

void ImguiEditorUI::buildSceneManagerWindow()
{
	if(!m_sidebarState.showSceneManager)
	{
		return;
	}
	
	ImGui::Begin(SCENE_BROWSER_WINDOW_NAME, &m_sidebarState.showSceneManager);

	ImGui::Text("Active Scene");

	// Custom size: use all width, 5 items tall
	if(ImGui::BeginListBox("##Active Scene", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for(std::size_t sceneIdx = 0; sceneIdx < getEditor().numScenes(); ++sceneIdx)
		{
			DesignerScene* const scene = getEditor().getScene(sceneIdx);
			const bool isSelected = scene == getEditor().getActiveScene();
			if(ImGui::Selectable(scene->getName().c_str(), isSelected))
			{
				if(!isSelected)
				{
					getEditor().setActiveScene(sceneIdx);
				}
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	ImGui::End();
}

void ImguiEditorUI::buildEditorSettingsWindow()
{
	if(!m_sidebarState.showEditorSettings)
	{
		return;
	}

	// Always center this window when appearing
	ImGuiCond windowLayoutCond = ImGuiCond_Appearing;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(
		viewport->GetCenter(),
		windowLayoutCond,
		ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(
		{viewport->WorkSize.x * 0.5f, viewport->WorkSize.y * 0.8f}, 
		windowLayoutCond);

	ImGui::Begin(PH_IMGUI_SETTINGS_ICON " Editor Settings", &m_sidebarState.showEditorSettings);

	PH_DEFAULT_LOG("{}", m_sidebarState.showEditorSettings);

	ImGui::End();
}

void ImguiEditorUI::buildStatsMonitor()
{
	PH_ASSERT(m_editor);

	if(ImGui::IsKeyReleased(ImGuiKey_F1))
	{
		m_shouldShowStatsMonitor = !m_shouldShowStatsMonitor;
	}

	if(m_shouldShowStatsMonitor)
	{
		constexpr float windowWidth = 300;
		constexpr float windowHeight = 300;

		ImGuiViewport* const viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos({
			viewport->WorkPos.x + viewport->WorkSize.x - windowWidth,
			viewport->WorkPos.y});
		ImGui::SetNextWindowSize({
			windowWidth,
			windowHeight});

		ImGui::Begin(PH_IMGUI_STATS_ICON " Stats", &m_shouldShowStatsMonitor);

		ImGui::Text("Main Thread:");
		ImGui::Text("Update: %f ms", m_editor->editorStats.mainThreadUpdateMs);
		ImGui::Text("Render: %f ms", m_editor->editorStats.mainThreadRenderMs);
		ImGui::Text("Event Flush: %f ms", m_editor->editorStats.mainThreadEventFlushMs);
		ImGui::Text("Frame: %f ms", m_editor->editorStats.mainThreadFrameMs);

		ImGui::Separator();

		ImGui::Text("Render Thread:");
		ImGui::Text("Frame: %f ms", m_editor->editorStats.renderThreadFrameMs);

		ImGui::Separator();

		ImGui::Text("GHI Thread:");
		ImGui::Text("Frame: %f ms", m_editor->editorStats.ghiThreadFrameMs);

		ImGui::End();
	}
}

void ImguiEditorUI::buildFileSystemDialogPopupModal(
	const char* const popupName,
	FileSystemExplorer& explorer,
	const bool canSelectFile,
	const bool canSelectDirectory)
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if(ImGui::BeginPopupModal(popupName, NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		buildFileSystemDialogContent(explorer, canSelectFile, canSelectDirectory);

		ImGui::Separator();

		if(ImGui::Button("OK", ImVec2(120, 0)))
		{
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

void ImguiEditorUI::buildFileSystemDialogContent(
	FileSystemExplorer& explorer,
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
				explorer.setCurrentRootPath(m_fsDialogSelectedRootIdx);
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
		ImVec2(
			getDimensionHints().fileDialogPreferredWidth * 0.4f,
			getDimensionHints().fileDialogPreferredHeight),
		true, 
		windowFlags);
	buildFileSystemDialogTreeNodeRecursive(explorer.getCurrentDirectoryEntry(), explorer);
	ImGui::EndChild();

	ImGui::SameLine();

	// Right child: file system item view
	ImGui::BeginChild(
		"fs_dialog_selectable",
		ImVec2(
			getDimensionHints().fileDialogPreferredWidth * 0.6f, 
			getDimensionHints().fileDialogPreferredHeight),
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
		if(m_fsDialogSelectedEntry)
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

void ImguiEditorUI::buildFileSystemDialogTreeNodeRecursive(
	FileSystemDirectoryEntry* baseEntry,
	FileSystemExplorer& explorer)
{
	if(!baseEntry)
	{
		return;
	}

	const bool isRootEntry = baseEntry->getParent() == nullptr;

	ImGuiTreeNodeFlags nodeFlags = 0;
	if(baseEntry == m_fsDialogSelectedEntry)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	if(isRootEntry)
	{
		nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	
	const bool isNodeOpened = ImGui::TreeNodeEx(baseEntry->getDirectoryName().c_str(), nodeFlags);
	if(ImGui::IsItemClicked() || !m_fsDialogSelectedEntry)
	{
		m_fsDialogSelectedEntry = baseEntry;
		m_fsDialogEntryItems = explorer.makeItemListing(baseEntry, false);

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
		explorer.expand(baseEntry);
		for(std::size_t entryIdx = 0; entryIdx < baseEntry->numChildren(); ++entryIdx)
		{
			FileSystemDirectoryEntry* derivedEntry = baseEntry->getChild(entryIdx);
			buildFileSystemDialogTreeNodeRecursive(derivedEntry, explorer);
		}
		ImGui::TreePop();
	}
	else
	{
		explorer.collapse(baseEntry);
	}
}

void ImguiEditorUI::buildImguiDemo()
{
	if(ImGui::IsKeyReleased(ImGuiKey_F2))
	{
		m_shouldShowImguiDemo = !m_shouldShowImguiDemo;
	}

	if(m_shouldShowImguiDemo)
	{
		imgui_show_demo_window(&m_shouldShowImguiDemo);
	}
}

Editor& ImguiEditorUI::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

ImguiFontLibrary& ImguiEditorUI::getFontLibrary()
{
	PH_ASSERT(m_fontLibrary);
	return *m_fontLibrary;
}

ImguiImageLibrary& ImguiEditorUI::getImageLibrary()
{
	PH_ASSERT(m_imageLibrary);
	return *m_imageLibrary;
}

DimensionHints& ImguiEditorUI::getDimensionHints()
{
	return getEditor().dimensionHints;
}

}// end namespace ph::editor
