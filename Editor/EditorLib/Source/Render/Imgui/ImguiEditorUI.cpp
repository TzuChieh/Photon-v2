#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "App/Misc/EditorSettings.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Editor/ImguiEditorLog.h"
#include "Render/Imgui/Editor/ImguiEditorSceneCreator.h"
#include "Render/Imgui/Editor/ImguiEditorSceneManager.h"
#include "Render/Imgui/Editor/ImguiEditorOfflineTaskManager.h"
#include "Render/Imgui/Editor/ImguiEditorSettings.h"
#include "Render/Imgui/Editor/ImguiEditorSceneObjectBrowser.h"
#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Editor/ImguiEditorAssetBrowser.h"
#include "Render/Imgui/Editor/ImguiEditorImageViewer.h"
#include "Render/Imgui/Editor/ImguiEditorDebugPanel.h"
#include "Designer/DesignerScene.h"
#include "Render/Imgui/ImguiEditorUIProxy.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>
#include <string_view>
#include <cstdio>
#include <utility>

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
	PH_IMGUI_OBJECTS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Object Browser";

constexpr const char* SIDEBAR_WINDOW_NAME = "##sidebar_window";
constexpr const char* TOOLBAR_WINDOW_NAME = "##toolbar_window";

constexpr const char* SCENE_CREATOR_WINDOW_NAME = 
	PH_IMGUI_SCENE_CREATION_ICON PH_IMGUI_ICON_TIGHT_PADDING "Scene Creator";

constexpr const char* SCENE_MANAGER_WINDOW_NAME = 
	PH_IMGUI_SCENE_MANAGER_ICON PH_IMGUI_ICON_TIGHT_PADDING "Scene Manager";

constexpr const char* OFFLINE_TASK_MANAGER_WINDOW_NAME =
	PH_IMGUI_TASK_MANAGER_ICON PH_IMGUI_ICON_TIGHT_PADDING "Offline Task Manager";

constexpr const char* LOG_WINDOW_NAME = 
	PH_IMGUI_LOG_ICON PH_IMGUI_ICON_TIGHT_PADDING "Log";

constexpr const char* EDITOR_SETTINGS_WINDOW_NAME = 
	PH_IMGUI_SETTINGS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Editor Settings";

constexpr const char* OPEN_SCENE_DIALOG_TITLE = PH_IMGUI_OPEN_FILE_ICON " Open Scene";
constexpr const char* DEBUG_MODE_TITLE = PH_IMGUI_BUG_ICON " Debug Mode";

}// end anonymous namespace

const ImguiEditorUI* ImguiEditorUI::mainEditorUI = nullptr;

ImguiEditorUI::ImguiEditorUI(
	Editor& editor,
	ImguiFontLibrary& fontLibrary,
	ImguiImageLibrary& imageLibrary)

	: m_editor(editor)
	, m_fontLibrary(fontLibrary)
	, m_imageLibrary(imageLibrary)

	, m_shouldResetWindowLayout(false)
	, m_shouldShowStatsMonitor(false)
	, m_isOpeningScene(false)
	, m_enableDebug(false)
	, m_toolState()

	, m_panels()
	, m_panelEntries()
	, m_sceneCreator(nullptr)
	, m_imageViewer(nullptr)
	, m_debugPanel(nullptr)

	, m_sampleInspector()
	, m_generalFileSystemDialog()
	, m_objectTypeMenu()
	, m_theme()
{
	// If no main editor was specified, the first editor created after is the main one
	if(!mainEditorUI)
	{
		mainEditorUI = this;
	}

	m_panels.add(std::make_unique<ImguiEditorSceneManager>(*this));
	m_imageViewer = m_panels.add(std::make_unique<ImguiEditorImageViewer>(*this));
	m_panels.add(std::make_unique<ImguiEditorLog>(*this));
	m_sceneCreator = m_panels.add(std::make_unique<ImguiEditorSceneCreator>(*this));
	m_panels.add(std::make_unique<ImguiEditorOfflineTaskManager>(*this));
	m_panels.add(std::make_unique<ImguiEditorSettings>(*this));
	m_panels.add(std::make_unique<ImguiEditorSceneObjectBrowser>(*this));
	m_panels.add(std::make_unique<ImguiEditorPropertyPanel>(*this));
	m_panels.add(std::make_unique<ImguiEditorAssetBrowser>(*this));
	m_debugPanel = m_panels.add(std::make_unique<ImguiEditorDebugPanel>(*this));

	for(const auto& panel : m_panels)
	{
		auto attributes = panel->getAttributes();

		// Append an invisible counter to window name to avoid ID collision
		auto windowIdName = 
			attributes.icon + 
			PH_IMGUI_ICON_TIGHT_PADDING + 
			attributes.title + 
			"##" + std::to_string(m_panelEntries.size());

		m_panelEntries.push_back({
			.panel = panel.get(),
			.attributes = attributes,
			.windowIdName = windowIdName,
			.isOpening = attributes.isOpenedByDefault});
	}
}

ImguiEditorUI::~ImguiEditorUI()
{
	// Remove panels first as they may depend on some fundamental members of editor UI
	m_panels.removeAll();

	if(mainEditorUI == this)
	{
		mainEditorUI = nullptr;
	}
}

void ImguiEditorUI::build()
{
	PH_ASSERT(Threads::isOnMainThread());

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
			getEditor().dimensionHints.fontSize * 2.0f /
			viewport->WorkSize.y;
		ImGuiID childDockSpaceID;
		const ImGuiID topDockSpaceID = ImGui::DockBuilderSplitNode(
			rootDockSpaceID, ImGuiDir_Up, topNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating left node 
		const float leftNodeSplitRatio =
			getEditor().dimensionHints.largeFontSize * 1.5f /
			viewport->WorkSize.x;
		const ImGuiID leftDockSpaceID = ImGui::DockBuilderSplitNode(
			childDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating right node
		const float rightNodeSplitRatio =
			getEditor().dimensionHints.propertyPanelPreferredWidth /
			(viewport->WorkSize.x * (1.0f - leftNodeSplitRatio));
		const ImGuiID rightDockSpaceID = ImGui::DockBuilderSplitNode(
			childDockSpaceID, ImGuiDir_Right, rightNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating bottom node (after left & right nodes to not straddle them)
		const float bottomNodeSplitRatio =
			getEditor().dimensionHints.propertyPanelPreferredWidth * 0.6f /
			(viewport->WorkSize.y * (1.0f - topNodeSplitRatio));
		const ImGuiID bottomDockSpaceID = ImGui::DockBuilderSplitNode(
			childDockSpaceID, ImGuiDir_Down, bottomNodeSplitRatio, nullptr, &childDockSpaceID);

		// Creating child upper-right and upper-left nodes
		const float upperRightNodeSplitRatio = 0.4f;
		ImGuiID upperRightDockSpaceID, lowerRightDockSpaceID;
		ImGui::DockBuilderSplitNode(
			rightDockSpaceID, ImGuiDir_Up, upperRightNodeSplitRatio, &upperRightDockSpaceID, &lowerRightDockSpaceID);

		// Pre-dock some persistent windows
		ImGui::DockBuilderDockWindow(MAIN_VIEWPORT_WINDOW_NAME, childDockSpaceID);

		// Pre-dock other windows
		ImGui::DockBuilderDockWindow(SIDEBAR_WINDOW_NAME, leftDockSpaceID);
		ImGui::DockBuilderDockWindow(TOOLBAR_WINDOW_NAME, topDockSpaceID);

		// Pre-dock panel window if requested
		for(PanelEntry& entry : m_panelEntries)
		{
			ImGuiID dockSpaceID = 0;
			switch(entry.attributes.preferredDockingLot)
			{
			case EImguiPanelDockingLot::None: dockSpaceID = 0; break;
			case EImguiPanelDockingLot::Center: dockSpaceID = childDockSpaceID; break;
			case EImguiPanelDockingLot::Bottom: dockSpaceID = bottomDockSpaceID; break;
			case EImguiPanelDockingLot::UpperRight: dockSpaceID = upperRightDockSpaceID; break;
			case EImguiPanelDockingLot::LowerRight: dockSpaceID = lowerRightDockSpaceID; break;
			}

			if(dockSpaceID != 0)
			{
				ImGui::DockBuilderDockWindow(entry.windowIdName.c_str(), dockSpaceID);
			}
		}

		ImGui::DockBuilderFinish(rootDockSpaceID);
	}

	PH_ASSERT_NE(rootDockSpaceID, 0);

	// Submit the DockSpace
	ImGui::DockSpace(rootDockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);

	ImGui::End();

	buildSidebarWindow();
	buildToolbarWindow();
	buildMainViewportWindow();
	buildOpenSceneDialog();
	buildStatsMonitor();
	buildTool();

	// Build panel window if it is opening
	for(PanelEntry& entry : m_panelEntries)
	{
		if(!entry.isOpening || !entry.panel)
		{
			continue;
		}

		entry.panel->buildWindow(
			entry.windowIdName.c_str(), 
			entry.attributes.isCloseable ? &entry.isOpening : nullptr);
	}

	// DEBUG
	//getImageLibrary().imguiImage(EImguiImage::Folder, {200, 200});
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

auto ImguiEditorUI::getPanelEntry(ImguiEditorPanel* panel)
-> PanelEntry*
{
	for(PanelEntry& entry : m_panelEntries)
	{
		if(entry.panel == panel)
		{
			return &entry;
		}
	}
	return nullptr;
}

void ImguiEditorUI::buildMainMenuBar()
{
	EditorSettings& settings = getEditor().getSettings();
	m_shouldResetWindowLayout = false;

	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Open Scene"))
			{
				m_isOpeningScene = true;
			}

			if(ImGui::MenuItem("Save Active Scene"))
			{
				saveActiveScene();
			}

			// Menu item from panels
			ImGui::Separator();
			for(PanelEntry& entry : m_panelEntries)
			{
				if(!entry.attributes.useMenubar ||
				   entry.attributes.category != EImguiPanelCategory::File)
				{
					continue;
				}

				if(ImGui::MenuItem(entry.attributes.title.c_str()))
				{
					entry.isOpening = true;
				}
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

			// Menu item from panels
			ImGui::Separator();
			for(PanelEntry& entry : m_panelEntries)
			{
				if(!entry.attributes.useMenubar ||
				   entry.attributes.category != EImguiPanelCategory::Edit)
				{
					continue;
				}

				if(ImGui::MenuItem(entry.attributes.title.c_str()))
				{
					entry.isOpening = true;
				}
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Tools"))
		{
			if(ImGui::MenuItem("Sample Tool"))
			{
				m_toolState.showSampleTool = true;
			}

			if(settings.isDevelopmentMode)
			{
				ImGui::Separator();

				if(ImGui::MenuItem("DearImGui Demo"))
				{
					m_toolState.showDearImGuiDemo = true;
				}

				if(ImGui::MenuItem("ImPlot Demo"))
				{
					m_toolState.showImPlotDemo = true;
				}
			}

			// Menu item from panels
			ImGui::Separator();
			for(PanelEntry& entry : m_panelEntries)
			{
				if(!entry.attributes.useMenubar ||
				   entry.attributes.category != EImguiPanelCategory::Tools)
				{
					continue;
				}

				if(ImGui::MenuItem(entry.attributes.title.c_str()))
				{
					entry.isOpening = true;
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

			// Menu item from panels
			ImGui::Separator();
			for(PanelEntry& entry : m_panelEntries)
			{
				if(!entry.attributes.useMenubar ||
				   entry.attributes.category != EImguiPanelCategory::Window)
				{
					continue;
				}

				if(ImGui::MenuItem(entry.attributes.title.c_str()))
				{
					entry.isOpening = true;
				}
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Debug"))
		{
			if(!settings.isDevelopmentMode) { ImGui::BeginDisabled(); }
			ImGui::MenuItem("Debug Mode", nullptr, &m_enableDebug);
			if(!settings.isDevelopmentMode) { ImGui::EndDisabled(); }

			// Menu item from panels
			ImGui::Separator();
			for(PanelEntry& entry : m_panelEntries)
			{
				if(!entry.attributes.useMenubar ||
				   entry.attributes.category != EImguiPanelCategory::Debug)
				{
					continue;
				}

				if(ImGui::MenuItem(entry.attributes.title.c_str()))
				{
					entry.isOpening = true;
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
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

	const ImGuiStyle& style = ImGui::GetStyle();
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

	// Sidebar from panels
	for(PanelEntry& entry : m_panelEntries)
	{
		if(!entry.attributes.useSidebar)
		{
			continue;
		}

		ImGui::Spacing();

		buildSidebarButton(
			entry.attributes.icon.c_str(),
			entry.attributes.tooltip.c_str(),
			entry.isOpening);
	}

	if(m_enableDebug)
	{
		ImGui::Spacing();

		if(PanelEntry* entry = getPanelEntry(m_debugPanel))
		{
			buildSidebarButton(
				entry->attributes.icon.c_str(),
				entry->attributes.tooltip.c_str(),
				entry->isOpening);
		}
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

	const ImGuiStyle& style = ImGui::GetStyle();
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
		if(PanelEntry* entry = getPanelEntry(m_sceneCreator))
		{
			entry->isOpening = true;
		}
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

void ImguiEditorUI::buildStatsMonitor()
{
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
	ImGui::Text("Update: %f ms", getEditor().editorStats.mainThreadUpdateMs);
	ImGui::Text("Render: %f ms", getEditor().editorStats.mainThreadRenderMs);
	ImGui::Text("Event Flush: %f ms", getEditor().editorStats.mainThreadEventFlushMs);
	ImGui::Text("Frame: %f ms", getEditor().editorStats.mainThreadFrameMs);

	ImGui::Separator();

	ImGui::Text("Render Thread:");
	ImGui::Text("Frame: %f ms", getEditor().editorStats.renderThreadFrameMs);

	ImGui::Separator();

	ImGui::Text("GHI Thread:");
	ImGui::Text("Frame: %f ms", getEditor().editorStats.ghiThreadFrameMs);

	ImGui::End();
}

void ImguiEditorUI::buildTool()
{
	if(m_toolState.showSampleTool)
	{
		m_sampleInspector.buildWindow("Sample Inspector", &m_toolState.showSampleTool);
	}

	if(m_toolState.showDearImGuiDemo)
	{
		imgui_show_demo_window(&m_toolState.showDearImGuiDemo);
	}

	if(m_toolState.showImPlotDemo)
	{
		implot_show_demo_window(&m_toolState.showImPlotDemo);
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

DimensionHints& ImguiEditorUI::getDimensionHints()
{
	return getEditor().dimensionHints;
}

bool ImguiEditorUI::isMain() const
{
	PH_ASSERT(mainEditorUI);
	return mainEditorUI == this;
}

}// end namespace ph::editor
