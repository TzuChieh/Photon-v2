#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/ImguiHelper.h"

#include <Common/assertion.h>

namespace ph::editor
{

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
	, m_helper(nullptr)
	, m_rootDockSpaceID(0)
	, m_shouldResetRootDockSpace(false)
{}

void ImguiEditorUI::initialize(Editor* const editor, ImguiHelper* const helper)
{
	PH_ASSERT(editor);
	PH_ASSERT(helper);

	m_editor = editor;
	m_helper = helper;
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
	if(!ImGui::DockBuilderGetNode(m_rootDockSpaceID) || m_shouldResetRootDockSpace)
	{
		// Potentially clear out existing layout
		ImGui::DockBuilderRemoveNode(m_rootDockSpaceID);

		// Add an empty node with the size of viewport (minus menu bar)
		ImGui::DockBuilderAddNode(m_rootDockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(m_rootDockSpaceID, viewport->WorkSize);

		const float leftNodeSplitRatio =
			m_editor->dimensionHints.propertyPanelPreferredWidth /
			viewport->WorkSize.x;
		PH_DEFAULT_LOG("{}, {}", m_editor->dimensionHints.propertyPanelPreferredWidth, viewport->WorkSize.x);
		const ImGuiID rootLeftDockSpaceID = ImGui::DockBuilderSplitNode(
			m_rootDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, nullptr);

		ImGui::DockBuilderDockWindow("Window A", rootLeftDockSpaceID);
		ImGui::DockBuilderFinish(m_rootDockSpaceID);

		m_shouldResetRootDockSpace = false;
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

	ImGui::Begin("Window A");
	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	ImGui::End();

	ImGui::Begin("Window B");
	ImGui::Text("This is window B");
	ImGui::End();

	//show_imgui_demo_window();
}

void ImguiEditorUI::buildMainMenuBar()
{
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
				m_shouldResetRootDockSpace = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

}// end namespace ph::editor
