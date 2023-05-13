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

#include <string_view>

namespace ph::editor
{

const char* const ImguiEditorUI::rootPropertiesWindowName = ICON_MD_TUNE " Properties";
const char* const ImguiEditorUI::mainViewportWindowName = ICON_MD_CAMERA " Viewport";
const char* const ImguiEditorUI::assetBrowserWindowName = ICON_MD_FOLDER_OPEN " Asset Browser";
const char* const ImguiEditorUI::objectBrowserWindowName = ICON_MD_CATEGORY " Object Browser";

namespace
{

//constexpr std::string_view OPEN_FILE_DIALOG_POPUP_NAME = "Open File";
//constexpr std::string_view SAVE_FILE_DIALOG_POPUP_NAME = "Save File";

}// end anonymous namespace

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
	, m_fontLibrary(nullptr)
	, m_imageLibrary(nullptr)
	, m_rootDockSpaceID(0)
	, m_leftDockSpaceID(0)
	, m_rightDockSpaceID(0)
	, m_bottomDockSpaceID(0)
	, m_centerDockSpaceID(0)
	, m_shouldResetWindowLayout(false)
	, m_shouldShowStatsMonitor(false)
	, m_shouldShowImguiDemo(false)
	, m_fsDialogExplorer()
	, m_fsDialogSelectedEntry(nullptr)
	, m_fsDialogEntryItems()
	, m_fsDialogSelectedItemIdx(static_cast<std::size_t>(-1))
{}

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
	m_rootDockSpaceID = ImGui::GetID("RootDockSpace");
	if(!ImGui::DockBuilderGetNode(m_rootDockSpaceID) || m_shouldResetWindowLayout)
	{
		// Potentially clear out existing layout
		ImGui::DockBuilderRemoveNode(m_rootDockSpaceID);

		// Add an empty node with the size of viewport (minus menu bar)
		ImGui::DockBuilderAddNode(m_rootDockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_rootDockSpaceID, viewport->WorkSize);

		// Note that `ImGui::DockBuilderSplitNode()` is like using the splitting icon in the 
		// docking UI, while using the dock space ID without splitting is like using the central 
		// square icon in the docking UI.

		// Creating bottom node
		const float bottomNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.y;
		ImGuiID childTopDockSpaceID = 0;
		const ImGuiID rootBottomDockSpaceID = ImGui::DockBuilderSplitNode(
			m_rootDockSpaceID, ImGuiDir_Down, bottomNodeSplitRatio, nullptr, &childTopDockSpaceID);

		// Creating left node (after bottom node so it can have the full height)
		const float leftNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.x;
		//PH_DEFAULT_LOG("{}, {}", m_editor->dimensionHints.propertyPanelPreferredWidth, viewport->WorkSize.x);
		const ImGuiID rootLeftDockSpaceID = ImGui::DockBuilderSplitNode(
			m_rootDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, nullptr);

		// Creating right node (after bottom node so it can have the full height)
		const float rightNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.x;
		const ImGuiID rootRightDockSpaceID = ImGui::DockBuilderSplitNode(
			m_rootDockSpaceID, ImGuiDir_Right, rightNodeSplitRatio, nullptr, nullptr);

		// Pre-dock some persistent windows
		ImGui::DockBuilderDockWindow(assetBrowserWindowName, rootBottomDockSpaceID);
		ImGui::DockBuilderDockWindow(rootPropertiesWindowName, rootLeftDockSpaceID);
		ImGui::DockBuilderDockWindow(objectBrowserWindowName, rootRightDockSpaceID);
		ImGui::DockBuilderDockWindow(mainViewportWindowName, childTopDockSpaceID);

		ImGui::DockBuilderFinish(m_rootDockSpaceID);
	}

	PH_ASSERT_NE(m_rootDockSpaceID, 0);

	// Submit the DockSpace
	ImGui::DockSpace(m_rootDockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);

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
	buildFilesystemDialogContent(m_fsDialogExplorer);
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
	ImGui::Begin(assetBrowserWindowName);
	m_bottomDockSpaceID = ImGui::GetWindowDockID();
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();
}

void ImguiEditorUI::buildRootPropertiesWindow()
{
	ImGui::Begin(rootPropertiesWindowName);
	m_leftDockSpaceID = ImGui::GetWindowDockID();
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();
}

void ImguiEditorUI::buildObjectBrowserWindow()
{
	ImGui::Begin(objectBrowserWindowName);
	m_rightDockSpaceID = ImGui::GetWindowDockID();

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

	ImGui::Separator();

	ImGui::Text("Active Scene:");

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

void ImguiEditorUI::buildMainViewportWindow()
{
	ImGui::Begin(mainViewportWindowName);
	m_centerDockSpaceID = ImGui::GetWindowDockID();
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
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

		ImGui::Begin(ICON_MD_INSIGHTS " Stats", &m_shouldShowStatsMonitor);

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

void ImguiEditorUI::buildFilesystemDialogTreeNodeRecursive(
	FileSystemDirectoryEntry* baseEntry,
	FileSystemExplorer& explorer)
{
	if(!baseEntry)
	{
		return;
	}

	const bool isNodeOpened = ImGui::TreeNode(baseEntry->getDirectoryName().c_str());
	if(ImGui::IsItemClicked())
	{
		m_fsDialogEntryItems.clear();
		for(const Path& itemPath : explorer.makeItemListing(baseEntry, false))
		{
			m_fsDialogEntryItems.push_back(itemPath.toString());
		}
	}

	if(isNodeOpened)
	{
		explorer.expand(baseEntry);
		for(std::size_t entryIdx = 0; entryIdx < baseEntry->numChildren(); ++entryIdx)
		{
			FileSystemDirectoryEntry* derivedEntry = baseEntry->getChild(entryIdx);
			buildFilesystemDialogTreeNodeRecursive(derivedEntry, explorer);
		}
		ImGui::TreePop();
	}
	else
	{
		explorer.collapse(baseEntry);
	}
}

void ImguiEditorUI::buildFilesystemDialogContent(FileSystemExplorer& explorer)
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
	ImGui::BeginChild("fs_tree", ImVec2(300, 0), true, windowFlags);

	//explorer.getCurrentDirectoryEntry()->
	FileSystemDirectoryEntry* entry = explorer.getCurrentDirectoryEntry();

	buildFilesystemDialogTreeNodeRecursive(entry, explorer);

	for(const std::string& item : m_fsDialogEntryItems)
	{
		ImGui::Text(item.c_str());
	}

	if(ImGui::TreeNode("Advanced, with Selectable nodes"))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static bool align_label_with_current_x_position = false;
		static bool test_drag_and_drop = false;
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow", &base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", &base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth", &base_flags, ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine();
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth", &base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
		ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
		ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
		ImGui::Text("Hello!");
		if(align_label_with_current_x_position)
			ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

		// 'selection_mask' is dumb representation of what may be user-side selection state.
		//  You may retain selection state inside or outside your objects in whatever format you see fit.
		// 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
		/// of the loop. May be a pointer to your own node type, etc.
		static int selection_mask = (1 << 2);
		int node_clicked = -1;
		for(int i = 0; i < 6; i++)
		{
			// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
			// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (selection_mask & (1 << i)) != 0;
			if(is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			if(i < 3)
			{
				// Items 0..2 are Tree Node
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
				if(ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					node_clicked = i;
				if(test_drag_and_drop && ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
					ImGui::Text("This is a drag and drop source");
					ImGui::EndDragDropSource();
				}
				if(node_open)
				{
					ImGui::BulletText("Blah blah\nBlah Blah");
					ImGui::TreePop();
				}
			}
			else
			{
				// Items 3..5 are Tree Leaves
				// The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
				// use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
				if(ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					node_clicked = i;
				if(test_drag_and_drop && ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
					ImGui::Text("This is a drag and drop source");
					ImGui::EndDragDropSource();
				}
			}
		}
		if(node_clicked != -1)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if(ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
		if(align_label_with_current_x_position)
			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::TreePop();
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
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

}// end namespace ph::editor
