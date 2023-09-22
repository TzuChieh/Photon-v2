#include "Render/Imgui/Editor/ImguiEditorPropertyPanel.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "Render/Imgui/Editor/ImguiEditorObjectTypeMenu.h"
#include "Render/Imgui/Font/imgui_icons.h"

#include "ThirdParty/DearImGui.h"

#include <utility>

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
	constexpr ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_AlwaysVerticalScrollbar;// so width can be fixed--no scrollbar popping

	if(!ImGui::Begin(title, isOpening, windowFlags))
	{
		ImGui::End();
		return;
	}

	Editor& editor = editorUI.getEditor();

	// TODO: disable this button if object is flat
	const SdlClass* selectedClass = nullptr;
	editorUI.getObjectTypeMenu().buildMenuButton(
		PH_IMGUI_PLUS_ICON PH_IMGUI_ICON_TIGHT_PADDING "Child Object ", 
		selectedClass);

	ImGui::Text("This is window A");
	ImGui::Text("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	ImGui::End();
}

void ImguiEditorPropertyPanel::setLayout(UIPropertyLayout layout)
{
	m_layout = std::move(layout);
}

void ImguiEditorPropertyPanel::clearLayout()
{
	m_layout.clear();
}

}// end namespace ph::editor
