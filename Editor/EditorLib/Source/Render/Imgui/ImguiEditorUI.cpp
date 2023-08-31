#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "App/Misc/EditorSettings.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/ImguiImageLibrary.h"
//#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Render/Imgui/Font/IconsMaterialDesignIcons.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <string_view>
#include <cstdio>
#include <utility>

#define PH_IMGUI_PROPERTIES_ICON       ICON_MDI_TUNE
#define PH_IMGUI_VIEWPORT_ICON         ICON_MDI_CAMERA_IRIS
#define PH_IMGUI_ASSET_ICON            ICON_MDI_FOLDER_OPEN
#define PH_IMGUI_OBJECT_ICON           ICON_MDI_SHAPE_OUTLINE
#define PH_IMGUI_SCENE_CREATION_ICON   ICON_MDI_IMAGE_PLUS_OUTLINE
#define PH_IMGUI_SCENE_MANAGER_ICON    ICON_MDI_IMAGE_MULTIPLE_OUTLINE
#define PH_IMGUI_LOG_ICON              ICON_MDI_CONSOLE
#define PH_IMGUI_SETTINGS_ICON         ICON_MDI_COG_OUTLINE
#define PH_IMGUI_STATS_ICON            ICON_MDI_CHART_TIMELINE_VARIANT_SHIMMER
#define PH_IMGUI_SAVE_FILE_ICON        ICON_MDI_CONTENT_SAVE
#define PH_IMGUI_OPEN_FILE_ICON        ICON_MDI_FOLDER_OPEN
#define PH_IMGUI_BUG_ICON              ICON_MDI_LADYBUG

#define PH_IMGUI_ICON_TIGHT_PADDING " "
#define PH_IMGUI_ICON_LOOSE_PADDING "   "

namespace ph::editor
{

namespace
{

constexpr const char* ROOT_PROPERTY_WINDOW_NAME = 
	PH_IMGUI_PROPERTIES_ICON PH_IMGUI_ICON_TIGHT_PADDING "Properties";

constexpr const char* MAIN_VIEWPORT_WINDOW_NAME = 
	PH_IMGUI_VIEWPORT_ICON PH_IMGUI_ICON_TIGHT_PADDING "Viewport";

constexpr const char* ASSET_BROWSER_WINDOW_NAME = 
	PH_IMGUI_ASSET_ICON PH_IMGUI_ICON_TIGHT_PADDING "Asset Browser";

constexpr const char* OBJECT_BROWSER_WINDOW_NAME = 
	PH_IMGUI_OBJECT_ICON PH_IMGUI_ICON_TIGHT_PADDING "Object Browser";

constexpr const char* SIDEBAR_WINDOW_NAME = "##sidebar_window";
constexpr const char* TOOLBAR_WINDOW_NAME = "##toolbar_window";

constexpr const char* SCENE_CREATOR_WINDOW_NAME = 
	PH_IMGUI_SCENE_CREATION_ICON PH_IMGUI_ICON_TIGHT_PADDING "Scene Creator";

constexpr const char* SCENE_MANAGER_WINDOW_NAME = 
	PH_IMGUI_SCENE_MANAGER_ICON PH_IMGUI_ICON_TIGHT_PADDING "Scene Manager";

constexpr const char* LOG_WINDOW_NAME = 
	PH_IMGUI_LOG_ICON PH_IMGUI_ICON_TIGHT_PADDING "Log";

constexpr const char* EDITOR_SETTINGS_WINDOW_NAME = 
	PH_IMGUI_SETTINGS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Editor Settings";

constexpr const char* OPEN_SCENE_DIALOG_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open Scene";
constexpr const char* DEBUG_MODE_TITLE = PH_IMGUI_BUG_ICON " Debug Mode";

}// end anonymous namespace

const ImguiEditorUI* ImguiEditorUI::mainEditor = nullptr;

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
	, m_fontLibrary(nullptr)
	, m_imageLibrary(nullptr)

	, m_shouldResetWindowLayout(false)
	, m_shouldShowStatsMonitor(false)
	, m_shouldShowSceneCreator(false)
	, m_shouldShowDearImGuiDemo(false)
	, m_shouldShowImPlotDemo(false)
	, m_sidebarState()
	, m_editorLog()
	, m_sceneCreator()
	, m_sceneManager()
	, m_editorSettings()
	, m_sceneObjectBrowser()
	, m_rootPropertyPanel()
	, m_assetBrowser()
	, m_debugPanel()
	, m_sampleInspector()

	, m_isOpeningScene(false)
	, m_enableDebug(false)

	, m_generalFileSystemDialog()

	, m_theme()
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
	m_editor = editor;
	m_fontLibrary = fontLibrary;
	m_imageLibrary = imageLibrary;

	m_sceneObjectBrowser.initialize(*this);
}

void ImguiEditorUI::terminate()
{
	m_sceneObjectBrowser.terminate(*this);
}

void ImguiEditorUI::build()
{
	PH_ASSERT(Threads::isOnMainThread());
	PH_ASSERT(m_editor);

	if(!getEditor().getSettings().isDevelopmentMode)
	{
		m_enableDebug = false;
	}

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

	ImGuiWindowFlags rootWindowFlags = ImGuiWindowFlags_NoDocking;
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

		// Add an empty node with the size of available viewport (minus menu bar and toolbar)
		ImGui::DockBuilderAddNode(rootDockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(rootDockSpaceID, viewport->WorkSize);

		// Note that `ImGui::DockBuilderSplitNode()` is like using the splitting icon in the 
		// docking UI, while using the dock space ID without splitting is like using the central 
		// square icon in the docking UI.

		// Creating top node (first one to create, to cover full width)
		const float topNodeSplitRatio =
			m_editor->dimensionHints.fontSize * 2.0f /
			viewport->WorkSize.y;
		ImGuiID childDockSpaceID;
		const ImGuiID topDockSpaceID = ImGui::DockBuilderSplitNode(
			rootDockSpaceID, ImGuiDir_Up, topNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating left node 
		const float leftNodeSplitRatio =
			m_editor->dimensionHints.largeFontSize * 1.5f /
			viewport->WorkSize.x;
		const ImGuiID leftDockSpaceID = ImGui::DockBuilderSplitNode(
			childDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating right node
		const float rightNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			(viewport->WorkSize.x * (1.0f - leftNodeSplitRatio));
		const ImGuiID rightDockSpaceID = ImGui::DockBuilderSplitNode(
			childDockSpaceID, ImGuiDir_Right, rightNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating bottom node (after left & right nodes to not straddle them)
		const float bottomNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth * 0.6f /
			(viewport->WorkSize.y * (1.0f - topNodeSplitRatio));
		const ImGuiID bottomDockSpaceID = ImGui::DockBuilderSplitNode(
			childDockSpaceID, ImGuiDir_Down, bottomNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating child upper-right and upper-left nodes
		const float upperRightNodeSplitRatio = 0.4f;
		ImGuiID upperRightDockSpaceID, lowerRightDockSpaceID;
		ImGui::DockBuilderSplitNode(
			rightDockSpaceID, ImGuiDir_Up, upperRightNodeSplitRatio, &upperRightDockSpaceID, &lowerRightDockSpaceID);

		// Pre-dock some persistent windows
		ImGui::DockBuilderDockWindow(ASSET_BROWSER_WINDOW_NAME, bottomDockSpaceID);
		ImGui::DockBuilderDockWindow(ROOT_PROPERTY_WINDOW_NAME, lowerRightDockSpaceID);
		ImGui::DockBuilderDockWindow(OBJECT_BROWSER_WINDOW_NAME, upperRightDockSpaceID);
		ImGui::DockBuilderDockWindow(MAIN_VIEWPORT_WINDOW_NAME, childDockSpaceID);
		ImGui::DockBuilderDockWindow(SIDEBAR_WINDOW_NAME, leftDockSpaceID);
		ImGui::DockBuilderDockWindow(TOOLBAR_WINDOW_NAME, topDockSpaceID);

		// Pre-dock other windows
		ImGui::DockBuilderDockWindow(SCENE_MANAGER_WINDOW_NAME, upperRightDockSpaceID);
		ImGui::DockBuilderDockWindow(LOG_WINDOW_NAME, bottomDockSpaceID);

		ImGui::DockBuilderFinish(rootDockSpaceID);
	}

	PH_ASSERT_NE(rootDockSpaceID, 0);

	// Submit the DockSpace
	ImGui::DockSpace(rootDockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);

	ImGui::End();

	buildAssetBrowserWindow();
	buildRootPropertyWindow();
	buildObjectBrowserWindow();
	buildMainViewportWindow();
	buildSceneCreatorWindow();
	buildSceneManagerWindow();
	buildSidebarWindow();
	buildToolbarWindow();
	buildEditorSettingsWindow();
	buildLogWindow();
	buildOpenSceneDialog();
	buildStatsMonitor();
	buildTool();

	if(m_enableDebug)
	{
		buildDebugPanelWindow();
	}

	// DEBUG
	getImageLibrary().imguiImage(EImguiImage::Folder, {200, 200});
	/*if(ImGui::Button("fs"))
	{
		m_generalFileSystemDialog.openPopup(ImguiFileSystemDialog::OPEN_FILE_TITLE);
	}
	m_generalFileSystemDialog.buildFileSystemDialogPopupModal(
		ImguiFileSystemDialog::OPEN_FILE_TITLE,
		*this,
		ImVec2(
			getDimensionHints().fileDialogPreferredWidth,
			getDimensionHints().fileDialogPreferredHeight));*/
	//buildFileSystemDialogPopupModal(OPEN_FILE_DIALOG_POPUP_NAME, m_fsDialogExplorer);
	//buildFileSystemDialogContent(m_fsDialogExplorer);
}

void ImguiEditorUI::buildMainMenuBar()
{
	EditorSettings& settings = getEditor().getSettings();
	m_shouldResetWindowLayout = false;

	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("New Scene"))
			{
				m_shouldShowSceneCreator = true;
			}

			if(ImGui::MenuItem("Open Scene"))
			{
				m_isOpeningScene = true;
			}

			if(ImGui::MenuItem("Save Active Scene"))
			{
				saveActiveScene();
			}

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

		if(ImGui::BeginMenu("Tools"))
		{
			if(ImGui::MenuItem("Sample Tool"))
			{
				m_sampleInspector.isOpening = true;
			}

			if(settings.isDevelopmentMode)
			{
				ImGui::Separator();

				if(ImGui::MenuItem("DearImGui Demo"))
				{
					m_shouldShowDearImGuiDemo = true;
				}

				if(ImGui::MenuItem("ImPlot Demo"))
				{
					m_shouldShowImPlotDemo = true;
				}
			}

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

		if(ImGui::BeginMenu("Debug"))
		{
			if(!settings.isDevelopmentMode) { ImGui::BeginDisabled(); }
			ImGui::MenuItem("Debug Mode", nullptr, &m_enableDebug);
			if(!settings.isDevelopmentMode) { ImGui::EndDisabled(); }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ImguiEditorUI::buildAssetBrowserWindow()
{
	m_assetBrowser.buildWindow(ASSET_BROWSER_WINDOW_NAME, *this);
}

void ImguiEditorUI::buildRootPropertyWindow()
{
	m_rootPropertyPanel.buildWindow(ROOT_PROPERTY_WINDOW_NAME, *this);
}

void ImguiEditorUI::buildObjectBrowserWindow()
{
	m_sceneObjectBrowser.buildWindow(OBJECT_BROWSER_WINDOW_NAME, *this);
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
	const float buttonItemWidth = ImGui::GetFontSize() + style.FramePadding.x * 2.0f;
	const float posToCenter = (ImGui::GetWindowContentRegionMax().x - buttonItemWidth) * 0.5f;

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

	if(m_enableDebug)
	{
		ImGui::Spacing();

		buildSidebarButton(
			PH_IMGUI_BUG_ICON,
			"Debug Panel",
			m_sidebarState.showDebugPanel);
	}

	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::End();
}

void ImguiEditorUI::buildToolbarWindow()
{
	ImGuiWindowClass toolbarWindowClass;
	toolbarWindowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

	ImGuiWindowFlags toolbarWindowFlags = 
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::SetNextWindowClass(&toolbarWindowClass);
	ImGui::Begin(TOOLBAR_WINDOW_NAME, nullptr, toolbarWindowFlags);

	ImFont* const originalFont = ImGui::GetFont();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGuiStyle& style = ImGui::GetStyle();
	const float buttonItemHeight = ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
	float posToCenter = (ImGui::GetWindowContentRegionMax().y - buttonItemHeight) * 0.5f;

	auto toolbarButton = 
		[posToCenter](
			const char* buttonIcon, 
			const char* tooltip) 
		-> bool
		{
			// FIXME:
			// seems to be similar to `ImGuiStyleVar_WindowMinSize` (enforcing window min size), except
			// that this may be related to min size of dockspace
			// see https://github.com/ocornut/imgui/issues/4975
			const float offsetHack = ImGui::GetFontSize() * 0.3f;

			ImGui::SetCursorPosY(posToCenter + offsetHack);
			const bool isClicked = ImGui::Button(buttonIcon);
			if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			{
				ImGui::SetTooltip(tooltip);
			}

			return isClicked;
		};

	if(toolbarButton(PH_IMGUI_SCENE_CREATION_ICON, "New Scene"))
	{
		m_shouldShowSceneCreator = true;
	}

	ImGui::SameLine();

	if(toolbarButton(PH_IMGUI_OPEN_FILE_ICON, "Open Scene"))
	{
		m_isOpeningScene = true;
	}

	ImGui::SameLine();

	if(toolbarButton(PH_IMGUI_SAVE_FILE_ICON, "Save Active Scene"))
	{
		saveActiveScene();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	ImGui::End();
}

void ImguiEditorUI::buildSceneCreatorWindow()
{
	if(!m_shouldShowSceneCreator)
	{
		return;
	}

	m_sceneCreator.buildWindow(
		SCENE_CREATOR_WINDOW_NAME,
		*this,
		&m_shouldShowSceneCreator);
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

	m_editorSettings.buildWindow(
		EDITOR_SETTINGS_WINDOW_NAME, 
		*this,
		&m_sidebarState.showEditorSettings);
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

	m_editorLog.buildWindow(
		LOG_WINDOW_NAME, 
		*this, 
		&m_sidebarState.showLog);
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

void ImguiEditorUI::buildDebugPanelWindow()
{
	if(!m_sidebarState.showDebugPanel)
	{
		return;
	}

	m_debugPanel.buildWindow(
		DEBUG_MODE_TITLE,
		*this,
		&m_sidebarState.showDebugPanel);
}

void ImguiEditorUI::buildTool()
{
	if(m_sampleInspector.isOpening)
	{
		m_sampleInspector.buildWindow("Sample Inspector");
	}

	if(m_shouldShowDearImGuiDemo)
	{
		imgui_show_demo_window(&m_shouldShowDearImGuiDemo);
	}

	if(m_shouldShowImPlotDemo)
	{
		implot_show_demo_window(&m_shouldShowImPlotDemo);
	}
}

void ImguiEditorUI::buildOpenSceneDialog()
{
	if(!m_isOpeningScene)
	{
		return;
	}

	m_generalFileSystemDialog.openPopup(OPEN_SCENE_DIALOG_TITLE);
	m_generalFileSystemDialog.buildFileSystemDialogPopupModal(
		OPEN_SCENE_DIALOG_TITLE,
		*this,
		{.canSelectItem = true, .requiresItemSelection = true});

	if(m_generalFileSystemDialog.dialogClosed())
	{
		if(m_generalFileSystemDialog.hasSelectedItem())
		{
			Path sceneFile = m_generalFileSystemDialog.getSelectedTarget();
			getEditor().loadScene(sceneFile);
		}

		m_isOpeningScene = false;
	}
}

void ImguiEditorUI::saveActiveScene()
{
	getEditor().saveScene();
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
