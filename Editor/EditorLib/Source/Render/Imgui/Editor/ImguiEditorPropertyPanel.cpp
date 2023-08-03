#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

ImguiEditorPropertyPanel::ImguiEditorPropertyPanel()
	: m_layout()
{}

void ImguiEditorPropertyPanel::buildWindow(
	const char* title, 
	ImguiEditorUIProxy editorUI,
	bool* isOpening)
{
	if(!ImGui::Begin(title, isOpening))
	{
		ImGui::End();
		return;
	}

	Editor& editor = editorUI.getEditor();

	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	ImGui::End();
}

}// end namespace ph::editor
