#include "Render/Imgui/ImguiEditorUI.h"
#include "App/Editor.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/ImguiHelper.h"
#include "ThirdParty/DearImGuiExperimental.h"

#include <Common/assertion.h>

namespace ph::editor
{

ImguiEditorUI::ImguiEditorUI()
	: m_editor(nullptr)
	, m_helper(nullptr)
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

		ImGui::EndMainMenuBar();
	}

	// Experimental Docking API, see https://github.com/ocornut/imgui/issues/2109

	
	static bool hasInit = false;
	if(!hasInit)
	{
		const ImGuiID rootDockSpaceID = ImGui::DockSpaceOverViewport(
			ImGui::GetMainViewport(),
			ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::DockBuilderSetNodeSize(rootDockSpaceID, ImGui::GetMainViewport()->Size);

		const float leftNodeSplitRatio =
			m_editor->dimensionHints.mainViewportPreferredWidth /
			ImGui::GetMainViewport()->Size.x;
		const ImGuiID rootLeftDockSpaceID = ImGui::DockBuilderSplitNode(
			rootDockSpaceID, ImGuiDir_Left, leftNodeSplitRatio, nullptr, nullptr);

		ImGui::DockBuilderDockWindow("Window A", rootLeftDockSpaceID);

		ImGui::DockBuilderFinish(rootDockSpaceID);

		hasInit = true;
	}

	ImGui::Begin("Window A");
	ImGui::Text("This is window A");
	ImGui::End();

	//show_imgui_demo_window();
}

}// end namespace ph::editor
