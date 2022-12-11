#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph::editor
{

const char* const ImguiEditorUI::rootPropertiesWindowName = ICON_MD_TUNE " Properties";
const char* const ImguiEditorUI::mainViewportWindowName = ICON_MD_CAMERA " Viewport";
const char* const ImguiEditorUI::assetBrowserWindowName = ICON_MD_FOLDER_OPEN " Asset Browser";

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
	, m_fontLibrary(nullptr)
	, m_imageLibrary(nullptr)
	, m_rootDockSpaceID(0)
	, m_shouldResetWindowLayout(false)
	, m_shouldShowStatsMonitor(false)
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

	m_rootDockSpaceID = ImGui::GetID("RootDockSpace");
	if(!ImGui::DockBuilderGetNode(m_rootDockSpaceID) || m_shouldResetWindowLayout)
	{
		// Potentially clear out existing layout
		ImGui::DockBuilderRemoveNode(m_rootDockSpaceID);

		// Add an empty node with the size of viewport (minus menu bar)
		ImGui::DockBuilderAddNode(m_rootDockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_rootDockSpaceID, viewport->WorkSize);

		// Pre-dock bottom node
		const float bottomNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.y;
		const ImGuiID rootBottomDockSpaceID = ImGui::DockBuilderSplitNode(
			m_rootDockSpaceID, ImGuiDir_Down, bottomNodeSplitRatio, nullptr, nullptr);
		ImGui::DockBuilderDockWindow(assetBrowserWindowName, rootBottomDockSpaceID);

		// Pre-dock left node (after bottom node so it can have the full height)
		const float leftNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.x;
		//PH_DEFAULT_LOG("{}, {}", m_editor->dimensionHints.propertyPanelPreferredWidth, viewport->WorkSize.x);
		ImGuiID childRightDockSpaceID = 0;
		const ImGuiID rootLeftDockSpaceID = ImGui::DockBuilderSplitNode(
			m_rootDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, &childRightDockSpaceID);
		ImGui::DockBuilderDockWindow(rootPropertiesWindowName, rootLeftDockSpaceID);
		
		// Pre-dock center node as the other child of left node
		ImGui::DockBuilderDockWindow(mainViewportWindowName, childRightDockSpaceID);

		ImGui::DockBuilderFinish(m_rootDockSpaceID);
	}

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

	ImGui::Begin(rootPropertiesWindowName);
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();

	ImGui::Begin(assetBrowserWindowName);
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();

	ImGui::Begin(mainViewportWindowName);
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();

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

	//show_imgui_demo_window();
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

		ImGui::Begin(ICON_MD_INSIGHTS " Stats");

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

}// end namespace ph::editor
