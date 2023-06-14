#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <string_view>
#include <cstdio>
#include <utility>

#define PH_IMGUI_PROPERTIES_ICON       ICON_MD_TUNE
#define PH_IMGUI_VIEWPORT_ICON         ICON_MD_CAMERA
#define PH_IMGUI_ASSET_ICON            ICON_MD_FOLDER_OPEN
#define PH_IMGUI_OBJECT_ICON           ICON_MD_CATEGORY
#define PH_IMGUI_SCENE_CREATION_ICON   ICON_MD_ADD_PHOTO_ALTERNATE
#define PH_IMGUI_SCENE_MANAGER_ICON    ICON_MD_PHOTO_LIBRARY
#define PH_IMGUI_LOG_ICON              ICON_MD_WYSIWYG
#define PH_IMGUI_SETTINGS_ICON         ICON_MD_SETTINGS
#define PH_IMGUI_STATS_ICON            ICON_MD_INSIGHTS

namespace ph::editor
{

namespace
{

constexpr const char* ROOT_PROPERTIES_WINDOW_NAME = PH_IMGUI_PROPERTIES_ICON " Properties";
constexpr const char* MAIN_VIEWPORT_WINDOW_NAME = PH_IMGUI_VIEWPORT_ICON " Viewport";
constexpr const char* ASSET_BROWSER_WINDOW_NAME = PH_IMGUI_ASSET_ICON " Asset Browser";
constexpr const char* OBJECT_BROWSER_WINDOW_NAME = PH_IMGUI_OBJECT_ICON " Object Browser";
constexpr const char* SIDEBAR_WINDOW_NAME = "##sidebar_window";

constexpr const char* SCENE_CREATOR_WINDOW_NAME = PH_IMGUI_SCENE_CREATION_ICON " Scene Creator";
constexpr const char* SCENE_MANAGER_WINDOW_NAME = PH_IMGUI_SCENE_MANAGER_ICON " Scene Manager";
constexpr const char* LOG_WINDOW_NAME = PH_IMGUI_LOG_ICON " Log";

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
	, m_editorLog()
	, m_sceneCreator()
	, m_sceneManager()
	, m_assetBrowser()

	, m_generalFileSystemDialog()
{
	// If no main editor was specified, the first editor created after is the main one
	if(!mainEditor)
	{
		mainEditor = this;
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
			m_editor->dimensionHints.propertyPanelPreferredWidth * 0.6f /
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
		ImGui::DockBuilderDockWindow(SCENE_MANAGER_WINDOW_NAME, upperRightDockSpaceID);
		ImGui::DockBuilderDockWindow(LOG_WINDOW_NAME, bottomDockSpaceID);

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
	buildSceneCreatorWindow();
	buildSceneManagerWindow();
	buildSidebarWindow();

	buildEditorSettingsWindow();
	buildLogWindow();

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
		m_generalFileSystemDialog.openPopup(ImguiFileSystemDialog::OPEN_FILE_TITLE);
	}
	m_generalFileSystemDialog.buildFileSystemDialogPopupModal(
		ImguiFileSystemDialog::OPEN_FILE_TITLE,
		*this,
		ImVec2(
			getDimensionHints().fileDialogPreferredWidth,
			getDimensionHints().fileDialogPreferredHeight));
	//buildFileSystemDialogPopupModal(OPEN_FILE_DIALOG_POPUP_NAME, m_fsDialogExplorer);
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
	m_assetBrowser.buildWindow(ASSET_BROWSER_WINDOW_NAME, *this);
}

void ImguiEditorUI::buildRootPropertiesWindow()
{
	if(!ImGui::Begin(ROOT_PROPERTIES_WINDOW_NAME))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();
}

void ImguiEditorUI::buildObjectBrowserWindow()
{
	if(!ImGui::Begin(OBJECT_BROWSER_WINDOW_NAME))
	{
		ImGui::End();
		return;
	}

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
	if(!ImGui::Begin(MAIN_VIEWPORT_WINDOW_NAME))
	{
		ImGui::End();
		return;
	}

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

	auto buildSidebarButton = 
		[originalFont, posToCenter](
			const char* buttonIcon, 
			const char* tooltip,
			bool& toggleState)
		{
			ImGui::SetCursorPosX(posToCenter);
			if(ImGui::Button(buttonIcon))
			{
				toggleState = !toggleState;
			}
			ImGui::PushFont(originalFont);
			if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			{
				ImGui::SetTooltip(tooltip);
			}
			ImGui::PopFont();
		};

	ImGui::Spacing();

	buildSidebarButton(
		PH_IMGUI_SCENE_CREATION_ICON,
		"Scene Creator",
		m_sidebarState.showSceneCreator);

	ImGui::Spacing();

	buildSidebarButton(
		PH_IMGUI_SCENE_MANAGER_ICON,
		"Scene Manager",
		m_sidebarState.showSceneManager);

	ImGui::Spacing();

	buildSidebarButton(
		PH_IMGUI_LOG_ICON,
		"Log",
		m_sidebarState.showLog);

	ImGui::Spacing();

	buildSidebarButton(
		PH_IMGUI_SETTINGS_ICON,
		"Editor Settings",
		m_sidebarState.showEditorSettings);

	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::End();
}

void ImguiEditorUI::buildSceneCreatorWindow()
{
	if(!m_sidebarState.showSceneCreator)
	{
		return;
	}

	m_sceneCreator.buildWindow(
		SCENE_CREATOR_WINDOW_NAME,
		*this,
		&m_sidebarState.showSceneCreator);
}

void ImguiEditorUI::buildSceneManagerWindow()
{
	if(!m_sidebarState.showSceneManager)
	{
		return;
	}
	
	m_sceneManager.buildWindow(
		SCENE_MANAGER_WINDOW_NAME,
		*this,
		&m_sidebarState.showSceneManager);
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

	if(!ImGui::Begin(PH_IMGUI_SETTINGS_ICON " Editor Settings", &m_sidebarState.showEditorSettings))
	{
		ImGui::End();
		return;
	}

	PH_DEFAULT_LOG("{}", m_sidebarState.showEditorSettings);

	ImGui::End();
}

void ImguiEditorUI::buildLogWindow()
{
	// Only the main editor can have log window
	if(!isMainEditor())
	{
		return;
	}

	if(!m_sidebarState.showLog)
	{
		return;
	}

	m_editorLog.buildWindow(LOG_WINDOW_NAME, &m_sidebarState.showLog);
}

void ImguiEditorUI::buildStatsMonitor()
{
	PH_ASSERT(m_editor);

	if(ImGui::IsKeyReleased(ImGuiKey_F1))
	{
		m_shouldShowStatsMonitor = !m_shouldShowStatsMonitor;
	}

	if(!m_shouldShowStatsMonitor)
	{
		return;
	}

	constexpr float windowWidth = 300;
	constexpr float windowHeight = 300;

	ImGuiViewport* const viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos({
		viewport->WorkPos.x + viewport->WorkSize.x - windowWidth,
		viewport->WorkPos.y});
	ImGui::SetNextWindowSize({
		windowWidth,
		windowHeight});

	if(!ImGui::Begin(PH_IMGUI_STATS_ICON " Stats", &m_shouldShowStatsMonitor))
	{
		ImGui::End();
		return;
	}

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

bool ImguiEditorUI::isMainEditor() const
{
	PH_ASSERT(mainEditor);
	return mainEditor == this;
}

ImguiFileSystemDialog& ImguiEditorUI::getGeneralFileSystemDialog()
{
	return m_generalFileSystemDialog;
}

}// end namespace ph::editor
